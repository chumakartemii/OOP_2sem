#ifndef OOP_NEW_CONTACTMANAGER_H
#define OOP_NEW_CONTACTMANAGER_H

#include "contact.h"
#include <vector>
#include <string>

class ContactManager {
public:
    ContactManager(const std::string& filename = "contacts.txt");

    // Основные операции
    bool addContact(const Contact& contact);
    bool removeContact(int index);
    bool editContact(int index, const Contact& newData);

    // Поиск и сортировка
    void sortByField(const std::string& field);
    std::vector<Contact> search(const std::string& query) const;
    std::vector<Contact> searchByField(const std::string& field, const std::string& value) const;

    // Работа с файлами
    bool loadFromFile();
    bool saveToFile() const;

    // Геттеры
    const std::vector<Contact>& getContacts() const;
    int getContactCount() const;
    Contact getContact(int index) const;

    // Валидация
    static bool validateContact(const Contact& contact);

private:
    std::vector<Contact> contacts;
    std::string filename;
};

#endif //OOP_NEW_CONTACTMANAGER_H