#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include <QDialog>
#include "contact.h"
#include <QLineEdit>

namespace Ui {
class EditContactDialog;
}

class EditContactDialog : public QDialog {
    Q_OBJECT

public:
    explicit EditContactDialog(QWidget* parent = nullptr);
    explicit EditContactDialog(const Contact& contact, QWidget* parent = nullptr);
    ~EditContactDialog();

    Contact getContact() const;
    bool isValid() const;

private slots:
    void on_addPhoneButton_clicked();
    void on_removePhoneButton_clicked();
    void validateForm();
    void accept() override;

private:
    Ui::EditContactDialog* ui;
    Contact currentContact;
    bool validForm;

    void setupConnections();
    void populateForm(const Contact& contact);
    Contact createContactFromForm() const;
    bool validateField(QLineEdit* field, bool (*validator)(const QString&),
                       const QString& errorMessage);
};

#endif // EDITCONTACTDIALOG_H
