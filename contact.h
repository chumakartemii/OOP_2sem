#ifndef OOP_NEW_CONTACT_H
#define OOP_NEW_CONTACT_H

#include <string>
#include <vector>

class Contact {
public:
    // Конструкторы
    Contact();
    Contact(const std::string& firstName, const std::string& lastName,
            const std::string& email, const std::string& phone);

    // Геттеры
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getPatronymic() const;
    std::string getAddress() const;
    std::string getBirthDate() const;
    std::string getEmail() const;
    std::vector<std::string> getPhoneNumbers() const;

    // Сеттеры
    void setFirstName(const std::string& firstName);
    void setLastName(const std::string& lastName);
    void setPatronymic(const std::string& patronymic);
    void setAddress(const std::string& address);
    void setBirthDate(const std::string& birthDate);
    void setEmail(const std::string& email);
    void addPhoneNumber(const std::string& phone);
    void removePhoneNumber(int index);
    void setPhoneNumbers(const std::vector<std::string>& phones);

    // Методы сериализации
    std::string toString() const;
    static Contact fromString(const std::string& str);

    // Валидация полей
    static bool isValidName(const std::string& name);
    static bool isValidPhone(const std::string& phone);
    static bool isValidEmail(const std::string& email);
    static bool isValidDate(const std::string& date);

    // Вспомогательные методы
    static std::string trim(const std::string& str);
    static std::string normalizePhone(const std::string& phone);
    static std::string convertDateToStorageFormat(const std::string& date);
    static std::string convertDateToDisplayFormat(const std::string& date);

private:
    std::string firstName;
    std::string lastName;
    std::string patronymic;
    std::string address;
    std::string birthDate;
    std::string email;
    std::vector<std::string> phoneNumbers;
};

#endif //OOP_NEW_CONTACT_H