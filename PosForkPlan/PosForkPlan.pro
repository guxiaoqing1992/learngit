#-------------------------------------------------
#
# Project created by QtCreator 2017-10-25T11:30:37
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PosForkPlan
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    database.cpp \
    forkplanwidget.cpp \
    usingplanwidget.cpp \
    checkboxdelegate.cpp \
    changedatabasedialog.cpp \
    combobutton.cpp

HEADERS  += mainwindow.h \
    database.h \
    forkplanwidget.h \
    usingplanwidget.h \
    checkboxdelegate.h \
    changedatabasedialog.h \
    combobutton.h

FORMS    += mainwindow.ui \
    forkplanwidget.ui \
    usingplanwidget.ui \
    changedatabasedialog.ui

DISTFILES +=

RESOURCES += \
    style.qrc
