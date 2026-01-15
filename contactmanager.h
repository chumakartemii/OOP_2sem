#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "contact.h"

#include <QObject>
#include <QList>
#include <QString>
#include <QSettings>
#include "databasestorage.h"
enum StorageType {
    FileStorage,
    DatabaseStorageType
};

class ContactManager : public QObject {
    Q_OBJECT

public:
    explicit ContactManager(QObject* parent = nullptr);
    ContactManager(const QString& filename, QObject* parent = nullptr);
    ~ContactManager();

    // Основные операции
    bool addContact(const Contact& contact);
    bool removeContact(int index);
    bool editContact(int index, const Contact& newData);

    // Поиск и сортировка
    void sortByField(const QString& field);
    QList<Contact> search(const QString& query) const;
    QList<Contact> searchByField(const QString& field, const QString& value) const;

    // Работа с хранилищами
    bool load();
    bool save() const;
    bool loadFromFile();
    bool saveToFile() const;
    bool loadFromDatabase();
    bool saveToDatabase() const;

    // Геттеры
    QList<Contact> getContacts() const;
    int getContactCount() const;
    Contact getContact(int index) const;

    // Валидация
    static bool validateContact(const Contact& contact);

    // Управление хранилищем
    void setStorageType(StorageType type);
    StorageType getStorageType() const;

    // Настройки базы данных
    bool connectToDatabase(const QString& host, int port,
                           const QString& database, const QString& user,
                           const QString& password);
    void disconnectDatabase();
    bool isDatabaseConnected() const;

    // Настройки файла
    void setFileName(const QString& filename);
    QString getFileName() const;

signals:
    void contactsChanged();
    void storageTypeChanged(StorageType type);
    void databaseConnectionChanged(bool connected);

private:
    QList<Contact> contacts;
    QString filename;
    StorageType storageType;
    DatabaseStorage* dbStorage;
    QSettings settings;

    void saveSettings();
    void loadSettings();
};

#endif // CONTACTMANAGER_H
