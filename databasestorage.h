#ifndef DATABASESTORAGE_H
#define DATABASESTORAGE_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include "contact.h"

class DatabaseStorage : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseStorage(QObject *parent = nullptr);
    ~DatabaseStorage();

    bool connectToDatabase(const QString& host = "", int port = 0,
                           const QString& database = "", const QString& user = "",
                           const QString& password = "");
    bool isConnected() const;
    void disconnect();

    bool loadContacts(QList<Contact>& contacts);
    bool saveContacts(const QList<Contact>& contacts);
    bool addContact(const Contact& contact);
    bool updateContact(int id, const Contact& contact);
    bool deleteContact(int id);
    int getLastInsertedId();

private:
    QSqlDatabase db;
    bool connected;

    bool createTable();
};

#endif // DATABASESTORAGE_H
