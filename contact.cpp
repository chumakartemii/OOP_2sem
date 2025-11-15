#include "contact.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <stdexcept>

Contact::Contact() : firstName(""), lastName(""), patronymic(""), 
                    address(""), birthDate(""), email("") {}

Contact::Contact(const std::string& firstName, const std::string& lastName, 
                const std::string& email, const std::string& phone)
    : firstName(trim(firstName)), lastName(trim(lastName)), email(trim(email)) {
    if (!phone.empty()) {
        phoneNumbers.push_back(normalizePhone(trim(phone)));
    }
}

// Геттеры
std::string Contact::getFirstName() const { return firstName; }
std::string Contact::getLastName() const { return lastName; }
std::string Contact::getPatronymic() const { return patronymic; }
std::string Contact::getAddress() const { return address; }
std::string Contact::getBirthDate() const { return birthDate; }
std::string Contact::getEmail() const { return email; }
std::vector<std::string> Contact::getPhoneNumbers() const { return phoneNumbers; }

// Сеттеры с валидацией
void Contact::setFirstName(const std::string& firstName) {
    if (isValidName(firstName)) {
        this->firstName = trim(firstName);
    }
}

void Contact::setLastName(const std::string& lastName) {
    if (isValidName(lastName)) {
        this->lastName = trim(lastName);
    }
}

void Contact::setPatronymic(const std::string& patronymic) {
    if (patronymic.empty() || isValidName(patronymic)) {
        this->patronymic = trim(patronymic);
    }
}

void Contact::setAddress(const std::string& address) {
    this->address = trim(address);
}

void Contact::setBirthDate(const std::string& birthDate) {
    if (birthDate.empty() || isValidDate(birthDate)) {
        this->birthDate = convertDateToStorageFormat(trim(birthDate));
    }
}

void Contact::setEmail(const std::string& email) {
    if (isValidEmail(email)) {
        this->email = trim(email);
    }
}

void Contact::addPhoneNumber(const std::string& phone) {
    if (isValidPhone(phone)) {
        phoneNumbers.push_back(normalizePhone(trim(phone)));
    }
}

void Contact::removePhoneNumber(int index) {
    if (index >= 0 && index < phoneNumbers.size()) {
        phoneNumbers.erase(phoneNumbers.begin() + index);
    }
}

void Contact::setPhoneNumbers(const std::vector<std::string>& phones) {
    phoneNumbers.clear();
    for (const auto& phone : phones) {
        if (isValidPhone(phone)) {
            phoneNumbers.push_back(normalizePhone(trim(phone)));
        }
    }
}

std::string Contact::toString() const {
    std::string result = firstName + "|" + lastName + "|" + email + "|";
    for (const auto& p : phoneNumbers) {
        result += p + ";";
    }
    result += "|" + patronymic + "|" + address + "|" + birthDate;
    return result;
}

Contact Contact::fromString(const std::string& str) {
    std::vector<std::string> parts;
    std::string part;
    std::istringstream stream(str);
    
    while (std::getline(stream, part, '|')) {
        parts.push_back(part);
    }
    
    if (parts.size() < 7) {
        throw std::invalid_argument("Invalid contact string format");
    }
    
    Contact c(parts[0], parts[1], parts[2], "");
    
    // Parse phone numbers
    std::istringstream phones(parts[3]);
    while (std::getline(phones, part, ';')) {
        if (!part.empty()) {
            c.phoneNumbers.push_back(part);
        }
    }
    
    c.patronymic = parts[4];
    c.address = parts[5];
    c.birthDate = parts[6];
    
    return c;
}

bool Contact::isValidName(const std::string& name) {
    std::string trimmed = trim(name);
    if (trimmed.empty()) return false;
    
    // Регулярное выражение для имени: начинается с буквы, может содержать буквы, цифры, дефис и пробел
    std::regex nameRegex("^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё0-9\\s-]*[A-Za-zА-Яа-яЁё0-9]$");
    
    // Проверка на дефисы в начале или конце
    if (trimmed.front() == '-' || trimmed.back() == '-') {
        return false;
    }
    
    return std::regex_match(trimmed, nameRegex);
}

bool Contact::isValidPhone(const std::string& phone) {
    std::string trimmed = trim(phone);
    if (trimmed.empty()) return false;
    
    // Регулярное выражение для всех указанных форматов телефона
    std::regex phoneRegex("^(\\+7|8)[\\s(-]*(\\d{3})[\\s)-]*(\\d{3})[\\s-]*(\\d{2})[\\s-]*(\\d{2})$");
    
    return std::regex_match(trimmed, phoneRegex);
}

bool Contact::isValidEmail(const std::string& email) {
    std::string trimmed = trim(email);
    if (trimmed.empty()) return false;
    
    // Удаляем пробелы вокруг @
    size_t at_pos = trimmed.find('@');
    if (at_pos != std::string::npos) {
        std::string local = trim(trimmed.substr(0, at_pos));
        std::string domain = trim(trimmed.substr(at_pos + 1));
        trimmed = local + "@" + domain;
    }
    
    // Регулярное выражение для email
    std::regex emailRegex("^[A-Za-z0-9]+([._-][A-Za-z0-9]+)*@[A-Za-z0-9-]+(\\.[A-Za-z0-9-]+)*\\.[A-Za-z]{2,}$");
    
    return std::regex_match(trimmed, emailRegex);
}

bool Contact::isValidDate(const std::string& date) {
    std::string trimmed = trim(date);
    if (trimmed.empty()) return true; // Пустая дата допустима
    
    // Проверка русского формата DD.MM.YYYY или DD-MM-YYYY
    std::regex dateRegex("^(0[1-9]|[12][0-9]|3[01])[.-](0[1-9]|1[0-2])[.-](\\d{4})$");
    if (!std::regex_match(trimmed, dateRegex)) {
        return false;
    }
    
    // Парсинг даты из русского формата
    int day, month, year;
    char delimiter1, delimiter2;
    std::istringstream ss(trimmed);
    ss >> day >> delimiter1 >> month >> delimiter2 >> year;
    
    if (ss.fail()) {
        return false;
    }
    
    // Проверка корректности даты (дни в месяце, високосные года)
    // Проверка дней в месяце
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (day > (isLeap ? 29 : 28)) {
            return false;
        }
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        if (day > 30) {
            return false;
        }
    }
    
    // Проверка что дата не в будущем
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;
    
    if (year > currentYear) return false;
    if (year == currentYear && month > currentMonth) return false;
    if (year == currentYear && month == currentMonth && day > currentDay) return false;
    
    return true;
}

std::string Contact::trim(const std::string& str) {
    if (str.empty()) return "";
    
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    
    if (start == std::string::npos) return "";
    return str.substr(start, end - start + 1);
}

std::string Contact::normalizePhone(const std::string& phone) {
    // Упрощенная нормализация - удаляем все нецифровые символы кроме +
    std::string result;
    for (char c : phone) {
        if (std::isdigit(c) || c == '+') {
            result += c;
        }
    }
    
    // Если номер начинается с 8, заменяем на +7
    if (!result.empty() && result[0] == '8') {
        result = "+7" + result.substr(1);
    }
    
    return result;
}

std::string Contact::convertDateToStorageFormat(const std::string& date) {
    if (date.empty()) return "";
    
    // Если дата уже в формате YYYY-MM-DD, возвращаем как есть
    std::regex storageFormatRegex("^\\d{4}-\\d{2}-\\d{2}$");
    if (std::regex_match(date, storageFormatRegex)) {
        return date;
    }
    
    // Конвертация из русского формата DD.MM.YYYY в YYYY-MM-DD
    std::regex russianFormatRegex("^(\\d{2})[.-](\\d{2})[.-](\\d{4})$");
    std::smatch match;
    
    if (std::regex_match(date, match, russianFormatRegex)) {
        std::string day = match[1];
        std::string month = match[2];
        std::string year = match[3];
        return year + "-" + month + "-" + day;
    }
    
    return ""; // Неизвестный формат
}

std::string Contact::convertDateToDisplayFormat(const std::string& date) {
    if (date.empty()) return "";
    
    // Конвертация из формата хранения YYYY-MM-DD в русский формат DD.MM.YYYY
    std::regex storageFormatRegex("^(\\d{4})-(\\d{2})-(\\d{2})$");
    std::smatch match;
    
    if (std::regex_match(date, match, storageFormatRegex)) {
        std::string year = match[1];
        std::string month = match[2];
        std::string day = match[3];
        return day + "." + month + "." + year;
    }
    
    return date; // Если формат неизвестен, возвращаем исходную строку
}