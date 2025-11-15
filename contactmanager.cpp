#include "contactmanager.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>

ContactManager::ContactManager(const std::string& filename) : filename(filename) {
    loadFromFile();
}

bool ContactManager::addContact(const Contact& contact) {
    if (!validateContact(contact)) {
        return false;
    }
    
    contacts.push_back(contact);
    return saveToFile();
}

bool ContactManager::removeContact(int index) {
    if (index < 0 || index >= contacts.size()) {
        return false;
    }
    
    contacts.erase(contacts.begin() + index);
    return saveToFile();
}

bool ContactManager::editContact(int index, const Contact& newData) {
    if (index < 0 || index >= contacts.size() || !validateContact(newData)) {
        return false;
    }
    
    contacts[index] = newData;
    return saveToFile();
}

void ContactManager::sortByField(const std::string& field) {
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
}

std::vector<Contact> ContactManager::search(const std::string& query) const {
    std::vector<Contact> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& contact : contacts) {
        std::string firstName = contact.getFirstName();
        std::string lastName = contact.getLastName();
        std::string email = contact.getEmail();
        
        std::transform(firstName.begin(), firstName.end(), firstName.begin(), ::tolower);
        std::transform(lastName.begin(), lastName.end(), lastName.begin(), ::tolower);
        std::transform(email.begin(), email.end(), email.begin(), ::tolower);
        
        if (firstName.find(lowerQuery) != std::string::npos ||
            lastName.find(lowerQuery) != std::string::npos ||
            email.find(lowerQuery) != std::string::npos) {
            results.push_back(contact);
        }
    }
    
    return results;
}

std::vector<Contact> ContactManager::searchByField(const std::string& field, const std::string& value) const {
    std::vector<Contact> results;
    std::string lowerValue = value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    
    for (const auto& contact : contacts) {
        std::string fieldValue;
        
        if (field == "firstName") fieldValue = contact.getFirstName();
        else if (field == "lastName") fieldValue = contact.getLastName();
        else if (field == "email") fieldValue = contact.getEmail();
        else if (field == "patronymic") fieldValue = contact.getPatronymic();
        else continue;
        
        std::transform(fieldValue.begin(), fieldValue.end(), fieldValue.begin(), ::tolower);
        
        if (fieldValue.find(lowerValue) != std::string::npos) {
            results.push_back(contact);
        }
    }
    
    return results;
}

bool ContactManager::loadFromFile() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // Если файл не существует, это нормально - создадим его при сохранении
        return true;
    }
    
    contacts.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        if (!line.empty()) {
            try {
                Contact contact = Contact::fromString(line);
                contacts.push_back(contact);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing contact: " << e.what() << std::endl;
            }
        }
    }
    
    file.close();
    return true;
}

bool ContactManager::saveToFile() const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& contact : contacts) {
        file << contact.toString() << "\n";
    }
    
    file.close();
    return true;
}

const std::vector<Contact>& ContactManager::getContacts() const {
    return contacts;
}

int ContactManager::getContactCount() const {
    return static_cast<int>(contacts.size());
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
           !contact.getPhoneNumbers().empty();
}