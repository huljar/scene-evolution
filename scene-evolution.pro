#-------------------------------------------------
#
# Project created by QtCreator 2016-06-06T15:19:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scene-evolution
TEMPLATE = app


CONFIG += c++11

INCLUDEPATH += include

SOURCES += src/main.cpp\
        src/MainWindow.cpp \
        src/QtOgreWindow.cpp \
        src/CustomOgreWindow.cpp

HEADERS  += include/scene-evolution/MainWindow.h \
        include/scene-evolution/QtOgreWindow.h \
        include/scene-evolution/CustomOgreWindow.h

FORMS    += forms/MainWindow.ui

# OGRE
LIBS += -lOgreMain
contains(QT_ARCH, x86_64) {
    LIBS += -L/usr/lib/x86_64-linux-gnu/OGRE-1.9.0
}
else {
    LIBS += -L/usr/lib/i386-linux-gnu/OGRE-1.9.0
}

# Boost
LIBS += -lboost_system

# OpenCV
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui
