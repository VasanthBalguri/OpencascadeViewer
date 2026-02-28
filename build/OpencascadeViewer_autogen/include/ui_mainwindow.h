/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionBottle;
    QAction *actionSample;
    QAction *actionRead_Step_File;
    QAction *actionOCAF_Example;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QMenuBar *menubar;
    QMenu *menuExamples;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        actionBottle = new QAction(MainWindow);
        actionBottle->setObjectName(QString::fromUtf8("actionBottle"));
        actionSample = new QAction(MainWindow);
        actionSample->setObjectName(QString::fromUtf8("actionSample"));
        actionRead_Step_File = new QAction(MainWindow);
        actionRead_Step_File->setObjectName(QString::fromUtf8("actionRead_Step_File"));
        actionOCAF_Example = new QAction(MainWindow);
        actionOCAF_Example->setObjectName(QString::fromUtf8("actionOCAF_Example"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 30));
        menuExamples = new QMenu(menubar);
        menuExamples->setObjectName(QString::fromUtf8("menuExamples"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuExamples->menuAction());
        menuExamples->addSeparator();
        menuExamples->addAction(actionBottle);
        menuExamples->addAction(actionSample);
        menuExamples->addAction(actionRead_Step_File);
        menuExamples->addAction(actionOCAF_Example);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionBottle->setText(QCoreApplication::translate("MainWindow", "Bottle", nullptr));
        actionSample->setText(QCoreApplication::translate("MainWindow", " Sample", nullptr));
        actionRead_Step_File->setText(QCoreApplication::translate("MainWindow", "Read Step File", nullptr));
        actionOCAF_Example->setText(QCoreApplication::translate("MainWindow", "OCAF Example", nullptr));
        menuExamples->setTitle(QCoreApplication::translate("MainWindow", "Examples", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
