#include "contactmanager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <algorithm>
#include "databasestorage.h"

ContactManager::ContactManager(QObject* parent)
    : QObject(parent),
    filename("contact.txt"),
    storageType(FileStorage),
    dbStorage(nullptr),
    settings("MyCompany", "ContactManager")
{
    loadSettings();

    if (storageType == DatabaseStorageType) {
        dbStorage = new DatabaseStorage(this);
        loadFromDatabase();
    } else {
        loadFromFile();
    }
}

ContactManager::ContactManager(const QString& filename, QObject* parent)
    : QObject(parent),
    filename(filename),
    storageType(FileStorage),
    dbStorage(nullptr),
    settings("MyCompany", "ContactManager")
{
    if (storageType == DatabaseStorageType) {
        dbStorage = new DatabaseStorage(this);
        loadFromDatabase();
    } else {
        loadFromFile();
    }
}

ContactManager::~ContactManager()
{
    saveSettings();
    delete dbStorage;
}

void ContactManager::setStorageType(StorageType type)
{
    if (storageType != type) {
        storageType = type;
        saveSettings();
        emit storageTypeChanged(type);
        load();
    }
}

StorageType ContactManager::getStorageType() const
{
    return storageType;
}

bool ContactManager::connectToDatabase(const QString& host, int port,
                                       const QString& database,
                                       const QString& user,
                                       const QString& password)
{
    if (!dbStorage) {
        dbStorage = new DatabaseStorage(this);
    }

    bool connected = dbStorage->connectToDatabase(host, port, database, user, password);
    if (connected) {
        storageType = DatabaseStorageType;
        saveSettings();
        emit databaseConnectionChanged(true);
    }

    return connected;
}

void ContactManager::disconnectDatabase()
{
    if (dbStorage) {
        dbStorage->disconnect();
        emit databaseConnectionChanged(false);
    }
}

bool ContactManager::isDatabaseConnected() const
{
    return dbStorage && dbStorage->isConnected();
}

void ContactManager::setFileName(const QString& filename)
{
    this->filename = filename;
    if (storageType == FileStorage) {
        loadFromFile();
    }
}

QString ContactManager::getFileName() const
{
    return filename;
}

bool ContactManager::addContact(const Contact& contact)
{
    if (!validateContact(contact)) {
        qDebug() << "Contact validation failed";
        return false;
    }

    if (storageType == DatabaseStorageType && dbStorage) {
        bool success = dbStorage->addContact(contact);
        if (success) {
            contacts.append(contact);
            emit contactsChanged();
        }
        return success;
    }

    contacts.append(contact);
    bool saved = saveToFile();
    if (saved) {
        emit contactsChanged();
    }
    return saved;
}

bool ContactManager::removeContact(int index)
{
    if (index < 0 || index >= contacts.size()) {
        qDebug() << "Invalid index for removal:" << index;
        return false;
    }

    contacts.removeAt(index);

    bool saved = false;
    if (storageType == FileStorage) {
        saved = saveToFile();
    } else if (storageType == DatabaseStorageType && dbStorage) {
        saved = dbStorage->saveContacts(contacts);
    }

    if (saved) {
        emit contactsChanged();
    }
    return saved;
}

bool ContactManager::loadFromFile()
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File does not exist or cannot be opened:" << filename;
        return true;
    }

    contacts.clear();
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            try {
                contacts.append(Contact::fromString(line));
            } catch (const std::exception& e) {
                qDebug() << "Error parsing contact:" << e.what();
            }
        }
    }

    return true;
}

bool ContactManager::saveToFile() const
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for writing:" << filename;
        return false;
    }

    QTextStream out(&file);
    for (const auto& contact : contacts) {
        out << contact.toString() << "\n";
    }

    return true;
}

bool ContactManager::loadFromDatabase()
{
    if (!dbStorage || !dbStorage->isConnected()) {
        return false;
    }

    if (!dbStorage->loadContacts(contacts)) {
        return false;
    }

    emit contactsChanged();
    return true;
}

bool ContactManager::saveToDatabase() const
{
    if (!dbStorage || !dbStorage->isConnected()) {
        return false;
    }

    return dbStorage->saveContacts(contacts);
}

void ContactManager::saveSettings()
{
    settings.setValue("storageType", static_cast<int>(storageType));
    settings.setValue("filename", filename);
}

void ContactManager::loadSettings()
{
    storageType = static_cast<StorageType>(
        settings.value("storageType", FileStorage).toInt()
        );
    filename = settings.value("filename", "contact.txt").toString();
}




bool ContactManager::editContact(int index, const Contact& newData)
{
    if (index < 0 || index >= contacts.size()) {
        return false;
    }

    contacts[index] = newData;

    if (storageType == DatabaseStorageType && dbStorage) {
        return dbStorage->saveContacts(contacts);
    }

    return saveToFile();
}

void ContactManager::sortByField(const QString& field)
{
    std::sort(contacts.begin(), contacts.end(),
              [&](const Contact& a, const Contact& b) {
                  if (field == "lastName") return a.getLastName() < b.getLastName();
                  if (field == "firstName") return a.getFirstName() < b.getFirstName();
                  if (field == "email") return a.getEmail() < b.getEmail();
                  if (field == "birthDate") return a.getBirthDate() < b.getBirthDate();
                  return false;
              });

    emit contactsChanged();
}

bool ContactManager::validateContact(const Contact& contact)
{
    return Contact::isValidName(contact.getFirstName()) &&
           Contact::isValidName(contact.getLastName()) &&
           Contact::isValidEmail(contact.getEmail()) &&
           !contact.getPhoneNumbers().isEmpty();
}

bool ContactManager::load()
{
    if (storageType == DatabaseStorageType) {
        return loadFromDatabase();
    }
    return loadFromFile();
}

Contact ContactManager::getContact(int index) const
{
    if (index < 0 || index >= contacts.size()) {
        return Contact();
    }
    return contacts[index];
}

QList<Contact> ContactManager::getContacts() const
{
    return contacts;
}

int ContactManager::getContactCount() const
{
    return contacts.size();
}

QList<Contact> ContactManager::search(const QString& query) const
{
    QList<Contact> result;
    QString q = query.toLower();

    for (const Contact& c : contacts) {
        if (c.getFirstName().toLower().contains(q) ||
            c.getLastName().toLower().contains(q) ||
            c.getEmail().toLower().contains(q)) {
            result.append(c);
        }
    }
    return result;
}

QList<Contact> ContactManager::searchByField(const QString& field,
                                             const QString& value) const
{
    QList<Contact> result;
    QString v = value.toLower();

    for (const Contact& c : contacts) {
        if (field == "firstName" && c.getFirstName().toLower().contains(v))
            result.append(c);
        else if (field == "lastName" && c.getLastName().toLower().contains(v))
            result.append(c);
        else if (field == "email" && c.getEmail().toLower().contains(v))
            result.append(c);
    }

    return result;
}
