#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    contactManager = new ContactManager(this);

    // Подключаем SQLite базу на рабочем столе
    // Подключаем SQLite базу на рабочем столе
    if (!db.connectToDatabase()) {
        qDebug() << "Не удалось подключиться к базе!";
    } else {
        qDebug() << "База данных успешно подключена";
    }


    // Привязываем кнопку "Сохранить" к слоту
    connect(ui->saveButton, &QPushButton::clicked,
            this, &MainWindow::onSaveButtonClicked);

    // Загружаем существующие контакты
    loadContactsToTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Слот: кнопка "Сохранить"
void MainWindow::onSaveButtonClicked()
{
    // Создаем объект контакта и заполняем его данными из формы
    Contact contact;
    contact.setFirstName(ui->firstNameLineEdit->text().trimmed());
    contact.setLastName(ui->lastNameLineEdit->text().trimmed());
    contact.setPatronymic(ui->patronymicLineEdit->text().trimmed());
    contact.setEmail(ui->emailLineEdit->text().trimmed());
    contact.setPhoneNumbers(ui->phoneLineEdit->text().split(",", Qt::SkipEmptyParts));
    contact.setAddress(ui->addressLineEdit->toPlainText().trimmed()); // <-- исправлено
    contact.setBirthDate(ui->birthDateEdit->date());

    // Добавляем контакт в ContactManager (кэш и текстовый файл)
    if (!contactManager->addContact(contact)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить контакт");
        return;
    }

    // Сохраняем контакт в текстовый файл на рабочем столе
    QFile file(QDir::homePath() + "/Desktop/contacts.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << contact.getFirstName() << ";"
            << contact.getLastName() << ";"
            << contact.getPatronymic() << ";"
            << contact.getEmail() << ";"
            << contact.getPhoneNumbers().join(",") << ";"
            << contact.getAddress() << ";"
            << contact.getBirthDate().toString("yyyy-MM-dd") << "\n";
        file.close();
        qDebug() << "Контакт сохранён в файл";
    } else {
        qDebug() << "Ошибка при сохранении в файл";
    }

    // Сохраняем контакт в SQLite через DatabaseStorage
    if (!db.addContact(contact)) {
        qDebug() << "Ошибка при добавлении контакта в базу!";
    } else {
        qDebug() << "Контакт успешно добавлен в базу!";
    }

    // Обновляем таблицу
    refreshTable();

    // Очищаем поля формы после добавления
    ui->firstNameLineEdit->clear();
    ui->lastNameLineEdit->clear();
    ui->patronymicLineEdit->clear();
    ui->emailLineEdit->clear();
    ui->phoneLineEdit->clear();
    ui->addressLineEdit->clear();
    ui->birthDateEdit->setDate(QDate::currentDate());
}


// Метод сохранения контакта
void MainWindow::saveContact(const Contact& contact)
{
    // 1️⃣ Сохраняем в ContactManager
    contactManager->addContact(contact);

    // 2️⃣ Сохраняем в текстовый файл на рабочем столе
    QFile file(QDir::homePath() + "/Desktop/contacts.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << contact.getFirstName() << ";"
            << contact.getLastName() << ";"
            << contact.getPatronymic() << ";"
            << contact.getEmail() << ";"
            << contact.getPhoneNumbers().join(",") << ";"
            << contact.getAddress() << ";"
            << contact.getBirthDate().toString("yyyy-MM-dd")
            << "\n";
        file.close();
        qDebug() << "Контакт сохранён в файл";
    } else {
        qDebug() << "Ошибка при сохранении в файл";
    }

    // 3️⃣ Сохраняем в SQLite
    if (!db.addContact(contact)) {
        qDebug() << "Ошибка при добавлении контакта в базу!";
    } else {
        qDebug() << "Контакт успешно добавлен в базу!";
    }
}

// Обновление таблицы контактов
void MainWindow::loadContactsToTable()
{
    ui->tableWidget->setRowCount(0);

    QList<Contact> contacts = contactManager->getContacts();

    for (int i = 0; i < contacts.size(); ++i) {
        const Contact& c = contacts[i];
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(c.getLastName()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(c.getFirstName()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(c.getPatronymic()));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(c.getEmail()));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(c.getPhoneNumbers().join(", ")));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(c.getBirthDateString()));
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(c.getAddress()));

        // Вычисление возраста
        if (c.getBirthDate().isValid()) {
            QDate today = QDate::currentDate();
            int age = today.year() - c.getBirthDate().year();
            if (today < QDate(today.year(), c.getBirthDate().month(), c.getBirthDate().day()))
                age--;
            ui->tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(age)));
        } else {
            ui->tableWidget->setItem(i, 7, new QTableWidgetItem(""));
        }
    }
}

void MainWindow::refreshTable()
{
    loadContactsToTable();
}
void MainWindow::on_addButton_clicked()
{
    // Реализация, например:
    qDebug() << "Добавить контакт";
}

void MainWindow::on_editButton_clicked()
{
    qDebug() << "Редактировать контакт";
}

void MainWindow::on_deleteButton_clicked()
{
    qDebug() << "Удалить контакт";
}

void MainWindow::on_searchButton_clicked()
{
    qDebug() << "Поиск";
}

void MainWindow::on_clearSearchButton_clicked()
{
    qDebug() << "Очистить поиск";
}

void MainWindow::on_sortComboBox_currentIndexChanged(int index)
{
    qDebug() << "Сортировка по индексу:" << index;
}
