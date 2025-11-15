#include <iostream>
#include <iomanip>
#include <limits>
#include "contactmanager.h"

void printMenu() {
    std::cout << "\n=== Система управления контактами ===" << std::endl;
    std::cout << "1. Добавить контакт" << std::endl;
    std::cout << "2. Показать все контакты" << std::endl;
    std::cout << "3. Редактировать контакт" << std::endl;
    std::cout << "4. Удалить контакт" << std::endl;
    std::cout << "5. Поиск контактов" << std::endl;
    std::cout << "6. Сортировка контактов" << std::endl;
    std::cout << "7. Выход" << std::endl;
    std::cout << "Выберите опцию: ";
}

void printContact(const Contact& contact, int index) {
    std::cout << "\n--- Контакт #" << index + 1 << " ---" << std::endl;
    std::cout << "ФИО: " << contact.getLastName() << " " << contact.getFirstName();
    if (!contact.getPatronymic().empty()) {
        std::cout << " " << contact.getPatronymic();
    }
    std::cout << std::endl;
    std::cout << "Email: " << contact.getEmail() << std::endl;
    std::cout << "Телефоны: ";
    for (const auto& phone : contact.getPhoneNumbers()) {
        std::cout << phone << " ";
    }
    std::cout << std::endl;
    if (!contact.getBirthDate().empty()) {
        std::string displayDate = Contact::convertDateToDisplayFormat(contact.getBirthDate());
        std::cout << "Дата рождения: " << displayDate << std::endl;
    }
    if (!contact.getAddress().empty()) {
        std::cout << "Адрес: " << contact.getAddress() << std::endl;
    }
}

Contact inputContact() {
    std::string firstName, lastName, email, phone;

    std::cout << "Введите имя: ";
    std::getline(std::cin, firstName);
    while (!Contact::isValidName(firstName)) {
        std::cout << "Неверное имя. Должно содержать только буквы, цифры, дефис и пробел, начинаться с буквы. Попробуйте снова: ";
        std::getline(std::cin, firstName);
    }

    std::cout << "Введите фамилию: ";
    std::getline(std::cin, lastName);
    while (!Contact::isValidName(lastName)) {
        std::cout << "Неверная фамилия. Должна содержать только буквы, цифры, дефис и пробел, начинаться с буквы. Попробуйте снова: ";
        std::getline(std::cin, lastName);
    }

    std::cout << "Введите email: ";
    std::getline(std::cin, email);
    while (!Contact::isValidEmail(email)) {
        std::cout << "Неверный email. Попробуйте снова: ";
        std::getline(std::cin, email);
    }

    std::cout << "Введите номер телефона: ";
    std::getline(std::cin, phone);
    while (!Contact::isValidPhone(phone)) {
        std::cout << "Неверный номер телефона. Поддерживаемые форматы: +78121234567, 88121234567, +7(812)1234567, 8(812)1234567, +7(812)123-45-67, 8(812)123-45-67. Попробуйте снова: ";
        std::getline(std::cin, phone);
    }

    Contact contact(firstName, lastName, email, phone);

    std::string patronymic, birthDate, address;

    std::cout << "Введите отчество (опционально): ";
    std::getline(std::cin, patronymic);
    if (!patronymic.empty()) {
        contact.setPatronymic(patronymic);
    }

    std::cout << "Введите дату рождения (ДД.ММ.ГГГГ, опционально): ";
    std::getline(std::cin, birthDate);
    if (!birthDate.empty()) {
        // Проверяем валидность даты перед установкой
        if (Contact::isValidDate(birthDate)) {
            contact.setBirthDate(birthDate);
        } else {
            std::cout << "Предупреждение: Дата не была установлена из-за неверного формата или значения." << std::endl;
        }
    }

    std::cout << "Введите адрес (опционально): ";
    std::getline(std::cin, address);
    if (!address.empty()) {
        contact.setAddress(address);
    }

    // Добавление дополнительных номеров телефона
    char addMore;
    std::cout << "Добавить еще один номер телефона? (y/n): ";
    std::cin >> addMore;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (addMore == 'y' || addMore == 'Y') {
        std::string additionalPhone;
        std::cout << "Введите дополнительный номер телефона: ";
        std::getline(std::cin, additionalPhone);
        if (Contact::isValidPhone(additionalPhone)) {
            contact.addPhoneNumber(additionalPhone);
            std::cout << "Номер добавлен успешно!" << std::endl;
        } else {
            std::cout << "Неверный номер телефона. Номер не был добавлен." << std::endl;
        }

        std::cout << "Добавить еще один номер телефона? (y/n): ";
        std::cin >> addMore;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return contact;
}

int main() {
    ContactManager manager;

    int choice;
    do {
        printMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                Contact newContact = inputContact();
                if (manager.addContact(newContact)) {
                    std::cout << "Контакт успешно добавлен!" << std::endl;
                } else {
                    std::cout << "Ошибка добавления контакта. Проверьте введенные данные." << std::endl;
                }
                break;
            }

            case 2: {
                std::cout << "\n=== Все контакты ===" << std::endl;
                const auto& contacts = manager.getContacts();
                if (contacts.empty()) {
                    std::cout << "Контакты не найдены." << std::endl;
                } else {
                    for (size_t i = 0; i < contacts.size(); ++i) {
                        printContact(contacts[i], i);
                    }
                }
                break;
            }

            case 3: {
                std::cout << "Введите индекс контакта для редактирования: ";
                int index;
                std::cin >> index;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (index >= 1 && index <= manager.getContactCount()) {
                    std::cout << "Редактирование контакта #" << index << std::endl;
                    Contact updatedContact = inputContact();
                    if (manager.editContact(index - 1, updatedContact)) {
                        std::cout << "Контакт успешно обновлен!" << std::endl;
                    } else {
                        std::cout << "Ошибка обновления контакта." << std::endl;
                    }
                } else {
                    std::cout << "Неверный индекс контакта." << std::endl;
                }
                break;
            }

            case 4: {
                std::cout << "Введите индекс контакта для удаления: ";
                int index;
                std::cin >> index;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (index >= 1 && index <= manager.getContactCount()) {
                    if (manager.removeContact(index - 1)) {
                        std::cout << "Контакт успешно удален!" << std::endl;
                    } else {
                        std::cout << "Ошибка удаления контакта." << std::endl;
                    }
                } else {
                    std::cout << "Неверный индекс контакта." << std::endl;
                }
                break;
            }

            case 5: {
                std::cout << "Поиск по: " << std::endl;
                std::cout << "1. Любому полю" << std::endl;
                std::cout << "2. Имени" << std::endl;
                std::cout << "3. Фамилии" << std::endl;
                std::cout << "4. Email" << std::endl;
                std::cout << "Выберите опцию: ";

                int searchChoice;
                std::cin >> searchChoice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::string query;
                std::cout << "Введите запрос для поиска: ";
                std::getline(std::cin, query);

                std::vector<Contact> results;
                switch (searchChoice) {
                    case 1: results = manager.search(query); break;
                    case 2: results = manager.searchByField("firstName", query); break;
                    case 3: results = manager.searchByField("lastName", query); break;
                    case 4: results = manager.searchByField("email", query); break;
                    default:
                        std::cout << "Неверный выбор." << std::endl;
                        break;
                }

                if (results.empty()) {
                    std::cout << "Контакты не найдены." << std::endl;
                } else {
                    std::cout << "\n=== Результаты поиска ===" << std::endl;
                    for (size_t i = 0; i < results.size(); ++i) {
                        printContact(results[i], i);
                    }
                }
                break;
            }

            case 6: {
                std::cout << "Сортировка по: " << std::endl;
                std::cout << "1. Имени" << std::endl;
                std::cout << "2. Фамилии" << std::endl;
                std::cout << "3. Email" << std::endl;
                std::cout << "4. Дате рождения" << std::endl;
                std::cout << "Выберите опцию: ";

                int sortChoice;
                std::cin >> sortChoice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                switch (sortChoice) {
                    case 1:
                        manager.sortByField("firstName");
                        std::cout << "Контакты отсортированы по имени." << std::endl;
                        break;
                    case 2:
                        manager.sortByField("lastName");
                        std::cout << "Контакты отсортированы по фамилии." << std::endl;
                        break;
                    case 3:
                        manager.sortByField("email");
                        std::cout << "Контакты отсортированы по email." << std::endl;
                        break;
                    case 4:
                        manager.sortByField("birthDate");
                        std::cout << "Контакты отсортированы по дате рождения." << std::endl;
                        break;
                    default:
                        std::cout << "Неверный выбор." << std::endl;
                        break;
                }
                break;
            }

            case 7:
                std::cout << "До свидания!" << std::endl;
                break;

            default:
                std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
                break;
        }
    } while (choice != 7);

    return 0;
}