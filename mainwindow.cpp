#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "editcontactdialog.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDir>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

    QString desktopPath = QDir::homePath() + "/Desktop/contact.txt";
    contactManager = new ContactManager(desktopPath, this);

    ui->setupUi(this);
    setupTable();

    connect(contactManager, &ContactManager::contactsChanged, this, &MainWindow::refreshTable);

    ui->sortComboBox->addItem("Не сортировать", "");
    ui->sortComboBox->addItem("По фамилии", "lastName");
    ui->sortComboBox->addItem("По имени", "firstName");
    ui->sortComboBox->addItem("По email", "email");
    ui->sortComboBox->addItem("По дате рождения", "birthDate");

    refreshTable();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupTable() {
    // Настройка таблицы
    ui->tableWidget->setColumnCount(8);
    QStringList headers;
    headers << "Фамилия" << "Имя" << "Отчество" << "Email" 
            << "Телефоны" << "Дата рождения" << "Адрес" << "Возраст";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    
    // Настройка ширины столбцов
    ui->tableWidget->setColumnWidth(0, 120); // Фамилия
    ui->tableWidget->setColumnWidth(1, 100); // Имя
    ui->tableWidget->setColumnWidth(2, 120); // Отчество
    ui->tableWidget->setColumnWidth(3, 180); // Email
    ui->tableWidget->setColumnWidth(4, 150); // Телефоны
    ui->tableWidget->setColumnWidth(5, 110); // Дата рождения
    ui->tableWidget->setColumnWidth(6, 200); // Адрес
    ui->tableWidget->setColumnWidth(7, 80);  // Возраст
}

void MainWindow::loadContactsToTable() {
    ui->tableWidget->setRowCount(0);
    
    QList<Contact> contacts = contactManager->getContacts();
    
    for (int i = 0; i < contacts.size(); ++i) {
        const Contact& contact = contacts[i];
        
        ui->tableWidget->insertRow(i);
        
        // Фамилия
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(contact.getLastName()));
        // Имя
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(contact.getFirstName()));
        // Отчество
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(contact.getPatronymic()));
        // Email
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(contact.getEmail()));
        // Телефоны
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(contact.getPhoneNumbers().join(", ")));
        // Дата рождения
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(contact.getBirthDateString()));
        // Адрес
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(contact.getAddress()));
        // Возраст
        if (contact.getBirthDate().isValid()) {
            QDate today = QDate::currentDate();
            QDate birthDate = contact.getBirthDate();
            int age = today.year() - birthDate.year();
            if (today.month() < birthDate.month() || 
                (today.month() == birthDate.month() && today.day() < birthDate.day())) {
                age--;
            }
            ui->tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(age)));
        } else {
            ui->tableWidget->setItem(i, 7, new QTableWidgetItem(""));
        }
    }
}

void MainWindow::on_addButton_clicked() {
    EditContactDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Contact contact = dialog.getContact();
        if (contactManager->addContact(contact)) {
            QMessageBox::information(this, "Успех", "Контакт успешно добавлен");
            refreshTable();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось добавить контакт");
        }
    }
}

void MainWindow::on_editButton_clicked() {
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите контакт для редактирования");
        return;
    }
    
    Contact contact = contactManager->getContact(currentRow);
    
    EditContactDialog dialog(contact, this);
    if (dialog.exec() == QDialog::Accepted) {
        Contact updatedContact = dialog.getContact();
        if (contactManager->editContact(currentRow, updatedContact)) {
            QMessageBox::information(this, "Успех", "Контакт успешно обновлен");
            refreshTable();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось обновить контакт");
        }
    }
}

void MainWindow::on_deleteButton_clicked() {
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите контакт для удаления");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Подтверждение", 
        "Вы уверены, что хотите удалить выбранный контакт?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (contactManager->removeContact(currentRow)) {
            QMessageBox::information(this, "Успех", "Контакт успешно удален");
            refreshTable();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить контакт");
        }
    }
}

void MainWindow::on_searchButton_clicked() {
    QString searchText = ui->searchLineEdit->text().trimmed();
    if (searchText.isEmpty()) {
        loadContactsToTable();
        return;
    }
    
    QList<Contact> results = contactManager->search(searchText);
    
    ui->tableWidget->setRowCount(0);
    
    for (int i = 0; i < results.size(); ++i) {
        const Contact& contact = results[i];
        
        ui->tableWidget->insertRow(i);
        
        // Фамилия
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(contact.getLastName()));
        // Имя
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(contact.getFirstName()));
        // Отчество
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(contact.getPatronymic()));
        // Email
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(contact.getEmail()));
        // Телефоны
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(contact.getPhoneNumbers().join(", ")));
        // Дата рождения
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(contact.getBirthDateString()));
        // Адрес
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(contact.getAddress()));
        // Возраст
        if (contact.getBirthDate().isValid()) {
            QDate today = QDate::currentDate();
            QDate birthDate = contact.getBirthDate();
            int age = today.year() - birthDate.year();
            if (today.month() < birthDate.month() || 
                (today.month() == birthDate.month() && today.day() < birthDate.day())) {
                age--;
            }
            ui->tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(age)));
        } else {
            ui->tableWidget->setItem(i, 7, new QTableWidgetItem(""));
        }
    }
    
    updateStatistics();
}

void MainWindow::on_clearSearchButton_clicked() {
    ui->searchLineEdit->clear();
    refreshTable();
}

void MainWindow::on_sortComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index);
    QString sortField = ui->sortComboBox->currentData().toString();
    if (!sortField.isEmpty()) {
        contactManager->sortByField(sortField);
    }
}

void MainWindow::refreshTable() {
    loadContactsToTable();
    updateStatistics();
}

void MainWindow::updateStatistics() {
    int totalContacts = contactManager->getContactCount();
    int visibleContacts = ui->tableWidget->rowCount();
    
    ui->statusLabel->setText(
        QString("Всего контактов: %1 | Показано: %2")
            .arg(totalContacts)
            .arg(visibleContacts)
    );
}
