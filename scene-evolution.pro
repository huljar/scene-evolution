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
        src/CustomOgreWindow.cpp \
        src/RGBDScene.cpp \
        src/CameraManager.cpp \
        src/DatasetManager.cpp \
        src/Scene.cpp \
        src/EventArgs.cpp

HEADERS  += include/scene-evolution/MainWindow.h \
        include/scene-evolution/QtOgreWindow.h \
        include/scene-evolution/CustomOgreWindow.h \
        include/scene-evolution/RGBDScene.h \
        include/scene-evolution/CameraManager.h \
        include/scene-evolution/DatasetManager.h \
        include/scene-evolution/Scene.h \
        include/scene-evolution/EventArgs.h \
        include/scene-evolution/Cache.h \
        include/scene-evolution/util.h \
        include/scene-evolution/interop.h

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
