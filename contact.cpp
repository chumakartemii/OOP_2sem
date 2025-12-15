#include "contact.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDate>
#include <QDebug>

Contact::Contact() : firstName(""), lastName(""), patronymic(""), 
                    address(""), email("") {}

Contact::Contact(const QString& firstName, const QString& lastName, 
                const QString& email, const QString& phone)
    : firstName(trim(firstName)), lastName(trim(lastName)), email(trim(email)) {
    if (!phone.isEmpty()) {
        phoneNumbers.append(normalizePhone(trim(phone)));
    }
}

// Геттеры
QString Contact::getFirstName() const { return firstName; }
QString Contact::getLastName() const { return lastName; }
QString Contact::getPatronymic() const { return patronymic; }
QString Contact::getAddress() const { return address; }
QDate Contact::getBirthDate() const { return birthDate; }
QString Contact::getBirthDateString() const { return convertDateToString(birthDate); }
QString Contact::getEmail() const { return email; }
QStringList Contact::getPhoneNumbers() const { return phoneNumbers; }

// Сеттеры с валидацией
void Contact::setFirstName(const QString& firstName) {
    if (isValidName(firstName)) {
        this->firstName = trim(firstName);
    }
}

void Contact::setLastName(const QString& lastName) {
    if (isValidName(lastName)) {
        this->lastName = trim(lastName);
    }
}

void Contact::setPatronymic(const QString& patronymic) {
    if (patronymic.isEmpty() || isValidName(patronymic)) {
        this->patronymic = trim(patronymic);
    }
}

void Contact::setAddress(const QString& address) {
    this->address = trim(address);
}

void Contact::setBirthDate(const QDate& birthDate) {
    this->birthDate = birthDate;
}

void Contact::setBirthDate(const QString& birthDate) {
    if (birthDate.isEmpty() || isValidDate(birthDate)) {
        this->birthDate = convertStringToDate(trim(birthDate));
    }
}

void Contact::setEmail(const QString& email) {
    if (isValidEmail(email)) {
        this->email = trim(email);
    }
}

void Contact::addPhoneNumber(const QString& phone) {
    if (isValidPhone(phone)) {
        phoneNumbers.append(normalizePhone(trim(phone)));
    }
}

void Contact::removePhoneNumber(int index) {
    if (index >= 0 && index < phoneNumbers.size()) {
        phoneNumbers.removeAt(index);
    }
}

void Contact::setPhoneNumbers(const QStringList& phones) {
    phoneNumbers.clear();
    for (const auto& phone : phones) {
        if (isValidPhone(phone)) {
            phoneNumbers.append(normalizePhone(trim(phone)));
        }
    }
}

QString Contact::toString() const {
    QString result = firstName + "|" + lastName + "|" + email + "|";
    for (const auto& p : phoneNumbers) {
        result += p + ";";
    }
    result += "|" + patronymic + "|" + address + "|" + 
              (birthDate.isValid() ? birthDate.toString("yyyy-MM-dd") : "");
    return result;
}

Contact Contact::fromString(const QString& str) {
    QStringList parts = str.split("|");
    
    if (parts.size() < 7) {
        throw std::invalid_argument("Invalid contact string format");
    }
    
    Contact c(parts[0], parts[1], parts[2], "");
    
    // Parse phone numbers
    QStringList phones = parts[3].split(";", QString::SkipEmptyParts);
    for (const auto& phone : phones) {
        if (!phone.isEmpty()) {
            c.phoneNumbers.append(phone);
        }
    }
    
    c.patronymic = parts[4];
    c.address = parts[5];
    c.birthDate = convertStringToDate(parts[6]);
    
    return c;
}

bool Contact::isValidName(const QString& name) {
    QString trimmed = trim(name);
    if (trimmed.isEmpty()) return false;
    
    // Регулярное выражение для имени: начинается с буквы, может содержать буквы, цифры, дефис и пробел
    QRegularExpression nameRegex("^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё0-9\\s-]*[A-Za-zА-Яа-яЁё0-9]$");
    
    // Проверка на дефисы в начале или конце
    if (trimmed.front() == '-' || trimmed.back() == '-') {
        return false;
    }
    
    // Проверка минимальной длины
    if (trimmed.length() < 2) {
        return false;
    }
    
    return nameRegex.match(trimmed).hasMatch();
}

bool Contact::isValidPhone(const QString& phone) {
    QString trimmed = trim(phone);
    if (trimmed.isEmpty()) return false;
    
    // Регулярное выражение для всех указанных форматов телефона
    QRegularExpression phoneRegex("^(\\+7|8)[\\s(-]*(\\d{3})[\\s)-]*(\\d{3})[\\s-]*(\\d{2})[\\s-]*(\\d{2})$");
    
    return phoneRegex.match(trimmed).hasMatch();
}

bool Contact::isValidEmail(const QString& email) {
    QString trimmed = trim(email);
    if (trimmed.isEmpty()) return false;
    
    // Удаляем пробелы вокруг @
    int at_pos = trimmed.indexOf('@');
    if (at_pos != -1) {
        QString local = trim(trimmed.left(at_pos));
        QString domain = trim(trimmed.mid(at_pos + 1));
        trimmed = local + "@" + domain;
    }
    
    // Регулярное выражение для email (только латинские буквы и цифры)
    QRegularExpression emailRegex("^[A-Za-z0-9]+([._-][A-Za-z0-9]+)*@[A-Za-z0-9-]+(\\.[A-Za-z0-9-]+)*\\.[A-Za-z]{2,}$");
    
    return emailRegex.match(trimmed).hasMatch();
}

bool Contact::isValidDate(const QString& date) {
    QString trimmed = trim(date);
    if (trimmed.isEmpty()) return true; // Пустая дата допустима
    
    // Проверка русского формата DD.MM.YYYY или DD-MM-YYYY
    QRegularExpression dateRegex("^(0[1-9]|[12][0-9]|3[01])[.-](0[1-9]|1[0-2])[.-](\\d{4})$");
    if (!dateRegex.match(trimmed).hasMatch()) {
        return false;
    }
    
    QDate d = convertStringToDate(trimmed);
    if (!d.isValid()) {
        return false;
    }
    
    // Проверка что дата не в будущем
    if (d > QDate::currentDate()) {
        return false;
    }
    
    return true;
}

QString Contact::trim(const QString& str) {
    return str.trimmed();
}

QString Contact::normalizePhone(const QString& phone) {
    // Упрощенная нормализация - удаляем все нецифровые символы кроме +
    QString result;
    for (QChar c : phone) {
        if (c.isDigit() || c == '+') {
            result.append(c);
        }
    }
    
    // Если номер начинается с 8, заменяем на +7
    if (!result.isEmpty() && result[0] == '8') {
        result = "+7" + result.mid(1);
    }
    
    return result;
}

QDate Contact::convertStringToDate(const QString& date) {
    if (date.isEmpty()) return QDate();
    
    // Если дата уже в формате YYYY-MM-DD
    if (date.contains('-')) {
        return QDate::fromString(date, "yyyy-MM-dd");
    }
    
    // Конвертация из русского формата DD.MM.YYYY
    return QDate::fromString(date, "dd.MM.yyyy");
}

QString Contact::convertDateToString(const QDate& date) {
    if (!date.isValid()) return "";
    return date.toString("dd.MM.yyyy");
}