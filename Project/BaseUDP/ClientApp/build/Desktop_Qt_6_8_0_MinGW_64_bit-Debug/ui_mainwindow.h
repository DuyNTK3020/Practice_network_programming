/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLineEdit *ipInput;
    QLineEdit *portInput;
    QLabel *label_2;
    QTextEdit *logTextEdit;
    QPushButton *connectButton;
    QLineEdit *messageInput;
    QLabel *label_3;
    QPushButton *sendButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(569, 352);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(70, 40, 21, 16));
        ipInput = new QLineEdit(centralwidget);
        ipInput->setObjectName("ipInput");
        ipInput->setGeometry(QRect(110, 40, 113, 21));
        portInput = new QLineEdit(centralwidget);
        portInput->setObjectName("portInput");
        portInput->setGeometry(QRect(110, 80, 113, 21));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(40, 80, 49, 16));
        logTextEdit = new QTextEdit(centralwidget);
        logTextEdit->setObjectName("logTextEdit");
        logTextEdit->setGeometry(QRect(353, 20, 201, 141));
        connectButton = new QPushButton(centralwidget);
        connectButton->setObjectName("connectButton");
        connectButton->setGeometry(QRect(90, 130, 75, 24));
        messageInput = new QLineEdit(centralwidget);
        messageInput->setObjectName("messageInput");
        messageInput->setGeometry(QRect(90, 220, 113, 21));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 220, 49, 16));
        sendButton = new QPushButton(centralwidget);
        sendButton->setObjectName("sendButton");
        sendButton->setGeometry(QRect(250, 220, 75, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 569, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "IP", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "PORT", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "connect", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "input", nullptr));
        sendButton->setText(QCoreApplication::translate("MainWindow", "send", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
