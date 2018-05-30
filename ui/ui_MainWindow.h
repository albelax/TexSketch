/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *s_mainWindowGridLayout;
    QGroupBox *s_transformGB;
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *viewport;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tabWidget;
    QWidget *diffuseTab;
    QWidget *normalTab;
    QMenuBar *menubar;
    QMenu *menuFiles;
    QMenu *menuedit;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        s_mainWindowGridLayout = new QGridLayout(centralwidget);
        s_mainWindowGridLayout->setObjectName(QStringLiteral("s_mainWindowGridLayout"));
        s_transformGB = new QGroupBox(centralwidget);
        s_transformGB->setObjectName(QStringLiteral("s_transformGB"));
        gridLayout = new QGridLayout(s_transformGB);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(s_transformGB);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        viewport = new QComboBox(s_transformGB);
        viewport->setObjectName(QStringLiteral("viewport"));

        gridLayout->addWidget(viewport, 3, 0, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 2, 3, 1, 1);


        s_mainWindowGridLayout->addWidget(s_transformGB, 0, 6, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        s_mainWindowGridLayout->addItem(horizontalSpacer_5, 0, 3, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        s_mainWindowGridLayout->addItem(horizontalSpacer_6, 0, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        s_mainWindowGridLayout->addItem(horizontalSpacer_2, 0, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        s_mainWindowGridLayout->addItem(horizontalSpacer, 0, 4, 1, 1);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        diffuseTab = new QWidget();
        diffuseTab->setObjectName(QStringLiteral("diffuseTab"));
        tabWidget->addTab(diffuseTab, QString());
        normalTab = new QWidget();
        normalTab->setObjectName(QStringLiteral("normalTab"));
        tabWidget->addTab(normalTab, QString());

        s_mainWindowGridLayout->addWidget(tabWidget, 1, 6, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 26));
        menuFiles = new QMenu(menubar);
        menuFiles->setObjectName(QStringLiteral("menuFiles"));
        menuedit = new QMenu(menubar);
        menuedit->setObjectName(QStringLiteral("menuedit"));
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menuFiles->menuAction());
        menubar->addAction(menuedit->menuAction());

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "TexSketch", Q_NULLPTR));
        s_transformGB->setTitle(QString());
        label->setText(QApplication::translate("MainWindow", "Viewport Mode", Q_NULLPTR));
        viewport->clear();
        viewport->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "2D", Q_NULLPTR)
         << QApplication::translate("MainWindow", "3D", Q_NULLPTR)
        );
        tabWidget->setTabText(tabWidget->indexOf(diffuseTab), QApplication::translate("MainWindow", "Tab 1", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(normalTab), QApplication::translate("MainWindow", "Tab 2", Q_NULLPTR));
        menuFiles->setTitle(QApplication::translate("MainWindow", "File", Q_NULLPTR));
        menuedit->setTitle(QApplication::translate("MainWindow", "Edit", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
