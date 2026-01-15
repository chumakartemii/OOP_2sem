#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "contactmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_addButton_clicked();
    void on_editButton_clicked();
    void on_deleteButton_clicked();
    void on_searchButton_clicked();
    void on_clearSearchButton_clicked();
    void on_sortComboBox_currentIndexChanged(int index);
    void refreshTable();
    void onSaveButtonClicked(); // вызывается при нажатии кнопки "Сохранить"

private:
    Ui::MainWindow* ui;
    ContactManager* contactManager;
    DatabaseStorage db;
    void setupTable();
    void updateStatistics();
    void loadContactsToTable();
    void saveContact(const Contact& contact); // метод сохранения

};

#endif // MAINWINDOW_H
