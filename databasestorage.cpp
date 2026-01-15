#include "databasestorage.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QVariant>

DatabaseStorage::DatabaseStorage(QObject *parent)
    : QObject(parent), connected(false)
{
}

DatabaseStorage::~DatabaseStorage()
{
    disconnect();
}

bool DatabaseStorage::connectToDatabase(const QString& host, int port,
                                        const QString& database,
                                        const QString& user,
                                        const QString& password)
{
    Q_UNUSED(host);
    Q_UNUSED(port);
    Q_UNUSED(database);
    Q_UNUSED(user);
    Q_UNUSED(password);

    // Путь к файлу базы на рабочем столе
    QString dbPath = QDir::homePath() + "/Desktop/contacts.db";

    db = QSqlDatabase::addDatabase("QSQLITE", "contacts_connection");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Cannot open SQLite database:" << db.lastError().text();
        connected = false;
        return false;
    }

    connected = true;
    createTable(); // создаем таблицу, если её нет
    return true;
}

bool DatabaseStorage::isConnected() const
{
    return connected && db.isOpen();
}

void DatabaseStorage::disconnect()
{
    if (db.isOpen()) {
        db.close();
    }
    connected = false;
}

bool DatabaseStorage::createTable()
{
    if (!isConnected()) return false;

    QString queryStr = R"(
        CREATE TABLE IF NOT EXISTS contacts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            patronymic TEXT,
            email TEXT NOT NULL,
            phone_numbers TEXT,
            address TEXT,
            birth_date DATE,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

    QSqlQuery query(db);
    if (!query.exec(queryStr)) {
        qDebug() << "Create table error:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseStorage::loadContacts(QList<Contact>& contacts)
{
    if (!isConnected()) return false;

    contacts.clear();
    QSqlQuery query(db);
    if (!query.exec("SELECT id, first_name, last_name, patronymic, email, phone_numbers, address, birth_date "
                    "FROM contacts ORDER BY last_name, first_name")) {
        qDebug() << "Load contacts error:" << query.lastError().text();
        return false;
    }

    while (query.next()) {
        Contact contact;
        contact.setFirstName(query.value("first_name").toString());
        contact.setLastName(query.value("last_name").toString());
        contact.setPatronymic(query.value("patronymic").toString());
        contact.setEmail(query.value("email").toString());
        contact.setAddress(query.value("address").toString());

        QString phonesStr = query.value("phone_numbers").toString();
        if (!phonesStr.isEmpty()) {
            contact.setPhoneNumbers(phonesStr.split(";", Qt::SkipEmptyParts));
        }

        QDate birthDate = query.value("birth_date").toDate();
        if (birthDate.isValid()) {
            contact.setBirthDate(birthDate);
        }

        contacts.append(contact);
    }

    return true;
}

bool DatabaseStorage::saveContacts(const QList<Contact>& contacts)
{
    if (!isConnected()) return false;

    db.transaction();

    // Очищаем таблицу
    QSqlQuery clearQuery(db);
    if (!clearQuery.exec("DELETE FROM contacts")) {
        qDebug() << "Clear table error:" << clearQuery.lastError().text();
        db.rollback();
        return false;
    }

    QString queryStr = R"(
        INSERT INTO contacts (first_name, last_name, patronymic, email, phone_numbers, address, birth_date)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";

    QSqlQuery insertQuery(db);
    insertQuery.prepare(queryStr);

    for (const Contact& contact : contacts) {
        insertQuery.addBindValue(contact.getFirstName());
        insertQuery.addBindValue(contact.getLastName());
        insertQuery.addBindValue(contact.getPatronymic());
        insertQuery.addBindValue(contact.getEmail());
        insertQuery.addBindValue(contact.getPhoneNumbers().join(";"));
        insertQuery.addBindValue(contact.getAddress());
        QDate bd = contact.getBirthDate();
        insertQuery.addBindValue(bd.isValid() ? bd : QVariant());

        if (!insertQuery.exec()) {
            qDebug() << "Insert contact error:" << insertQuery.lastError().text();
            db.rollback();
            return false;
        }
    }

    db.commit();
    return true;
}

bool DatabaseStorage::addContact(const Contact& contact)
{
    if (!isConnected()) return false;

    QString queryStr = R"(
        INSERT INTO contacts (first_name, last_name, patronymic, email, phone_numbers, address, birth_date)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";

    QSqlQuery query(db);
    query.prepare(queryStr);

    query.addBindValue(contact.getFirstName());
    query.addBindValue(contact.getLastName());
    query.addBindValue(contact.getPatronymic());
    query.addBindValue(contact.getEmail());
    query.addBindValue(contact.getPhoneNumbers().join(";"));
    query.addBindValue(contact.getAddress());
    QDate bd = contact.getBirthDate();
    query.addBindValue(bd.isValid() ? bd : QVariant());

    if (!query.exec()) {
        qDebug() << "Add contact error:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseStorage::updateContact(int id, const Contact& contact)
{
    if (!isConnected()) return false;

    QString queryStr = R"(
        UPDATE contacts
        SET first_name = ?, last_name = ?, patronymic = ?, email = ?,
            phone_numbers = ?, address = ?, birth_date = ?,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = ?
    )";

    QSqlQuery query(db);
    query.prepare(queryStr);

    query.addBindValue(contact.getFirstName());
    query.addBindValue(contact.getLastName());
    query.addBindValue(contact.getPatronymic());
    query.addBindValue(contact.getEmail());
    query.addBindValue(contact.getPhoneNumbers().join(";"));
    query.addBindValue(contact.getAddress());
    QDate bd = contact.getBirthDate();
    query.addBindValue(bd.isValid() ? bd : QVariant());
    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "Update contact error:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DatabaseStorage::deleteContact(int id)
{
    if (!isConnected()) return false;

    QSqlQuery query(db);
    query.prepare("DELETE FROM contacts WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "Delete contact error:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

int DatabaseStorage::getLastInsertedId()
{
    if (!isConnected()) return -1;

    QSqlQuery query(db);
    if (query.exec("SELECT last_insert_rowid()")) {
        if (query.next()) {
            return query.value(0).toInt();
        }
    }

    return -1;
}
