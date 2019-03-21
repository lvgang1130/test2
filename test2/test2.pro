#-------------------------------------------------
#
# Project created by QtCreator 2019-01-31T11:43:07
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#开启多线程，不用每次设置多线程编译,让程序执行加快
#MAKEFLAGS=-j8

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mythread.cpp \
    mythread2.cpp \
    curvechart.cpp \
    thread_readsql.cpp

HEADERS += \
        mainwindow.h \
    mythread.h \
    mythread2.h \
    thread_readsql.h \
    curvechart.h \
    stable.h

FORMS += \
        mainwindow.ui

PRECOMPILED_HEADER=stable.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    picture1.qrc
