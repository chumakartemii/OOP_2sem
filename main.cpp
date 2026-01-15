#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Установка стиля приложения
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // Настройка стиля приложения
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::AlternateBase, QColor(248, 248, 248));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    palette.setColor(QPalette::Text, QColor(0, 0, 0));
    palette.setColor(QPalette::Button, QColor(74, 144, 226));
    palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(74, 144, 226));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    QApplication::setPalette(palette);

    MainWindow w;
    w.show();

    return a.exec();
}
