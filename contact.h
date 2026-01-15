#ifndef CONTACT_H
#define CONTACT_H

#include <QString>
#include <QStringList>
#include <QDate>
#include <QRegularExpression>

class Contact {
public:
    // Конструкторы
    Contact();
    Contact(const QString& firstName, const QString& lastName,
            const QString& email, const QString& phone);

    // Геттеры
    QString getFirstName() const;
    QString getLastName() const;
    QString getPatronymic() const;
    QString getAddress() const;
    QDate getBirthDate() const;
    QString getBirthDateString() const;
    QString getEmail() const;
    QStringList getPhoneNumbers() const;

    // Сеттеры
    void setFirstName(const QString& firstName);
    void setLastName(const QString& lastName);
    void setPatronymic(const QString& patronymic);
    void setAddress(const QString& address);
    void setBirthDate(const QDate& birthDate);
    void setBirthDate(const QString& birthDate);
    void setEmail(const QString& email);
    void addPhoneNumber(const QString& phone);
    void removePhoneNumber(int index);
    void setPhoneNumbers(const QStringList& phones);

    // Методы сериализации
    QString toString() const;
    static Contact fromString(const QString& str);

    // Валидация полей
    static bool isValidName(const QString& name);
    static bool isValidPhone(const QString& phone);
    static bool isValidEmail(const QString& email);
    static bool isValidDate(const QString& date);

    // Вспомогательные методы
    static QString trim(const QString& str);
    static QString normalizePhone(const QString& phone);
    static QDate convertStringToDate(const QString& date);
    static QString convertDateToString(const QDate& date);

private:
    QString firstName;
    QString lastName;
    QString patronymic;
    QString address;
    QDate birthDate;
    QString email;
    QStringList phoneNumbers;
};

#endif // CONTACT_H
