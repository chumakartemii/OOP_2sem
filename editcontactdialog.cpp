#include "editcontactdialog.h"
#include "ui_editcontactdialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDateTime>
#include <QRegularExpression>


EditContactDialog::EditContactDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::EditContactDialog), validForm(false) {
    ui->setupUi(this);
    setupConnections();

    // Установка валидаторов
    QRegularExpression nameRegex("^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё0-9\\s-]*[A-Za-zА-Яа-яЁё0-9]$");
    QRegularExpressionValidator* nameValidator = new QRegularExpressionValidator(nameRegex, this);
    ui->firstNameEdit->setValidator(nameValidator);
    ui->lastNameEdit->setValidator(nameValidator);
    ui->patronymicEdit->setValidator(nameValidator);

    QRegularExpression phoneRegex("^(\\+7|8)[\\s(-]*(\\d{3})[\\s)-]*(\\d{3})[\\s-]*(\\d{2})[\\s-]*(\\d{2})$");
    QRegularExpressionValidator* phoneValidator = new QRegularExpressionValidator(phoneRegex, this);
    ui->phoneEdit->setValidator(phoneValidator);

    QRegularExpression emailRegex("^[A-Za-z0-9]+([._-][A-Za-z0-9]+)*@[A-Za-z0-9-]+(\\.[A-Za-z0-9-]+)*\\.[A-Za-z]{2,}$");
    QRegularExpressionValidator* emailValidator = new QRegularExpressionValidator(emailRegex, this);
    ui->emailEdit->setValidator(emailValidator);

    // Настройка QDateEdit
    ui->birthDateEdit->setDate(QDate::currentDate());
    ui->birthDateEdit->setMaximumDate(QDate::currentDate());
    ui->birthDateEdit->setDisplayFormat("dd.MM.yyyy");
    ui->birthDateEdit->setCalendarPopup(true);
}

EditContactDialog::EditContactDialog(const Contact& contact, QWidget* parent)
    : QDialog(parent), ui(new Ui::EditContactDialog),
    currentContact(contact), validForm(true) {
    ui->setupUi(this);
    setupConnections();
    populateForm(contact);
}

EditContactDialog::~EditContactDialog() {
    delete ui;
}

void EditContactDialog::setupConnections() {
    connect(ui->firstNameEdit, &QLineEdit::textChanged, this, &EditContactDialog::validateForm);
    connect(ui->lastNameEdit, &QLineEdit::textChanged, this, &EditContactDialog::validateForm);
    connect(ui->emailEdit, &QLineEdit::textChanged, this, &EditContactDialog::validateForm);
    connect(ui->phoneEdit, &QLineEdit::textChanged, this, &EditContactDialog::validateForm);
}

void EditContactDialog::populateForm(const Contact& contact) {
    ui->firstNameEdit->setText(contact.getFirstName());
    ui->lastNameEdit->setText(contact.getLastName());
    ui->patronymicEdit->setText(contact.getPatronymic());
    ui->emailEdit->setText(contact.getEmail());
    ui->addressEdit->setPlainText(contact.getAddress());

    if (contact.getBirthDate().isValid()) {
        ui->birthDateEdit->setDate(contact.getBirthDate());
    } else {
        ui->birthDateEdit->setDate(QDate::currentDate());
    }

    ui->phoneListWidget->clear();
    for (const QString& phone : contact.getPhoneNumbers()) {
        ui->phoneListWidget->addItem(phone);
    }

    if (!contact.getPhoneNumbers().isEmpty()) {
        ui->phoneEdit->setText(contact.getPhoneNumbers().first());
    }
}

Contact EditContactDialog::getContact() const {
    return currentContact;
}

bool EditContactDialog::isValid() const {
    return validForm;
}

void EditContactDialog::on_addPhoneButton_clicked() {
    QString phone = ui->phoneEdit->text().trimmed();
    if (Contact::isValidPhone(phone)) {
        QString normalized = Contact::normalizePhone(phone);
        ui->phoneListWidget->addItem(normalized);
        ui->phoneEdit->clear();
        validateForm();
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный формат телефона");
    }
}

void EditContactDialog::on_removePhoneButton_clicked() {
    int row = ui->phoneListWidget->currentRow();
    if (row >= 0) {
        delete ui->phoneListWidget->takeItem(row);
        validateForm();
    }
}

void EditContactDialog::validateForm() {
    bool valid = true;

    // Проверка обязательных полей
    valid &= validateField(ui->firstNameEdit, Contact::isValidName,
                           "Имя обязательно и должно содержать только буквы, цифры, дефис и пробел");
    valid &= validateField(ui->lastNameEdit, Contact::isValidName,
                           "Фамилия обязательна и должна содержать только буквы, цифры, дефис и пробел");
    valid &= validateField(ui->emailEdit, Contact::isValidEmail,
                           "Email обязателен и должен быть в правильном формате");

    // Проверка хотя бы одного телефона
    valid &= (ui->phoneListWidget->count() > 0);

    // Проверка даты рождения (если указана)
    if (ui->birthDateEdit->date() > QDate::currentDate()) {
        valid = false;
        ui->birthDateEdit->setStyleSheet("border: 1px solid red;");
    } else {
        ui->birthDateEdit->setStyleSheet("");
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
    validForm = valid;
}

bool EditContactDialog::validateField(QLineEdit* field, bool (*validator)(const QString&),
                                      const QString& errorMessage) {
    QString text = field->text().trimmed();
    bool valid = validator(text);

    if (!valid && !text.isEmpty()) {
        field->setToolTip(errorMessage);
        field->setStyleSheet("border: 1px solid red;");
    } else {
        field->setToolTip("");
        field->setStyleSheet(valid ? "" : "border: 1px solid red;");
    }

    return valid || text.isEmpty();
}

void EditContactDialog::accept() {
    currentContact = createContactFromForm();
    if (isValid()) {
        QDialog::accept();
    } else {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, исправьте ошибки в форме");
    }
}

Contact EditContactDialog::createContactFromForm() const {
    Contact contact(
        ui->firstNameEdit->text().trimmed(),
        ui->lastNameEdit->text().trimmed(),
        ui->emailEdit->text().trimmed(),
        ""
        );

    contact.setPatronymic(ui->patronymicEdit->text().trimmed());
    contact.setAddress(ui->addressEdit->toPlainText().trimmed());
    contact.setBirthDate(ui->birthDateEdit->date());

    QStringList phones;
    for (int i = 0; i < ui->phoneListWidget->count(); ++i) {
        phones.append(ui->phoneListWidget->item(i)->text());
    }
    contact.setPhoneNumbers(phones);

    return contact;
}
