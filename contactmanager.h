#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "contact.h"
#include <QObject>
#include <QList>
#include <QString>

class ContactManager : public QObject {
    Q_OBJECT

public:
    explicit ContactManager(QObject* parent = nullptr);
    ContactManager(const QString& filename, QObject* parent = nullptr);

    // Основные операции
    bool addContact(const Contact& contact);
    bool removeContact(int index);
    bool editContact(int index, const Contact& newData);

    // Поиск и сортировка
    void sortByField(const QString& field);
    QList<Contact> search(const QString& query) const;
    QList<Contact> searchByField(const QString& field, const QString& value) const;

    // Работа с файлами
    bool loadFromFile();
    bool saveToFile() const;

    // Геттеры
    QList<Contact> getContacts() const;
    int getContactCount() const;
    Contact getContact(int index) const;

    // Валидация
    static bool validateContact(const Contact& contact);

signals:
    void contactsChanged();

private:
    QList<Contact> contacts;
    QString filename;
};

#endif // CONTACTMANAGER_H
