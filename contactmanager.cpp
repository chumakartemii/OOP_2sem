#include "contactmanager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <algorithm>

ContactManager::ContactManager(QObject* parent) 
    : QObject(parent), filename("contacts.txt") {
    loadFromFile();
}

ContactManager::ContactManager(const QString& filename, QObject* parent)
    : QObject(parent), filename(filename) {
    loadFromFile();
}

bool ContactManager::addContact(const Contact& contact) {
    if (!validateContact(contact)) {
        qDebug() << "Contact validation failed";
        return false;
    }
    
    contacts.append(contact);
    bool saved = saveToFile();
    if (saved) {
        emit contactsChanged();
    }
    return saved;
}

bool ContactManager::removeContact(int index) {
    if (index < 0 || index >= contacts.size()) {
        qDebug() << "Invalid index for removal:" << index;
        return false;
    }
    
    contacts.removeAt(index);
    bool saved = saveToFile();
    if (saved) {
        emit contactsChanged();
    }
    return saved;
}

bool ContactManager::editContact(int index, const Contact& newData) {
    if (index < 0 || index >= contacts.size() || !validateContact(newData)) {
        qDebug() << "Invalid index or contact data for edit:" << index;
        return false;
    }
    
    contacts[index] = newData;
    bool saved = saveToFile();
    if (saved) {
        emit contactsChanged();
    }
    return saved;
}

void ContactManager::sortByField(const QString& field) {
    if (field == "firstName") {
        std::sort(contacts.begin(), contacts.end(), 
            [](const Contact& a, const Contact& b) { 
                return a.getFirstName() < b.getFirstName(); 
            });
    } else if (field == "lastName") {
        std::sort(contacts.begin(), contacts.end(), 
            [](const Contact& a, const Contact& b) { 
                return a.getLastName() < b.getLastName(); 
            });
    } else if (field == "email") {
        std::sort(contacts.begin(), contacts.end(), 
            [](const Contact& a, const Contact& b) { 
                return a.getEmail() < b.getEmail(); 
            });
    } else if (field == "birthDate") {
        std::sort(contacts.begin(), contacts.end(), 
            [](const Contact& a, const Contact& b) { 
                return a.getBirthDate() < b.getBirthDate(); 
            });
    }
    
    emit contactsChanged();
}

QList<Contact> ContactManager::search(const QString& query) const {
    QList<Contact> results;
    QString lowerQuery = query.toLower();
    
    for (const auto& contact : contacts) {
        QString firstName = contact.getFirstName().toLower();
        QString lastName = contact.getLastName().toLower();
        QString email = contact.getEmail().toLower();
        QString patronymic = contact.getPatronymic().toLower();
        
        if (firstName.contains(lowerQuery) ||
            lastName.contains(lowerQuery) ||
            email.contains(lowerQuery) ||
            patronymic.contains(lowerQuery) ||
            contact.getAddress().toLower().contains(lowerQuery)) {
            results.append(contact);
        }
    }
    
    return results;
}

QList<Contact> ContactManager::searchByField(const QString& field, const QString& value) const {
    QList<Contact> results;
    QString lowerValue = value.toLower();
    
    for (const auto& contact : contacts) {
        QString fieldValue;
        
        if (field == "firstName") fieldValue = contact.getFirstName();
        else if (field == "lastName") fieldValue = contact.getLastName();
        else if (field == "email") fieldValue = contact.getEmail();
        else if (field == "patronymic") fieldValue = contact.getPatronymic();
        else if (field == "address") fieldValue = contact.getAddress();
        else continue;
        
        if (fieldValue.toLower().contains(lowerValue)) {
            results.append(contact);
        }
    }
    
    return results;
}

bool ContactManager::loadFromFile() {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Если файл не существует, это нормально - создадим его при сохранении
        qDebug() << "File does not exist or cannot be opened:" << filename;
        return true;
    }
    
    contacts.clear();
    QTextStream in(&file);
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            try {
                Contact contact = Contact::fromString(line);
                contacts.append(contact);
            } catch (const std::exception& e) {
                qDebug() << "Error parsing contact:" << e.what();
            }
        }
    }
    
    file.close();
    return true;
}

bool ContactManager::saveToFile() const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for writing:" << filename;
        return false;
    }
    
    QTextStream out(&file);
    
    for (const auto& contact : contacts) {
        out << contact.toString() << "\n";
    }
    
    file.close();
    return true;
}

QList<Contact> ContactManager::getContacts() const {
    return contacts;
}

int ContactManager::getContactCount() const {
    return contacts.size();
}

Contact ContactManager::getContact(int index) const {
    if (index >= 0 && index < contacts.size()) {
        return contacts[index];
    }
    throw std::out_of_range("Index out of range");
}

bool ContactManager::validateContact(const Contact& contact) {
    return Contact::isValidName(contact.getFirstName()) &&
           Contact::isValidName(contact.getLastName()) &&
           Contact::isValidEmail(contact.getEmail()) &&
           !contact.getPhoneNumbers().isEmpty();
}