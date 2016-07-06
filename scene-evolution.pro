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
        src/EventArgs.cpp \
        src/OrientedBoundingBox.cpp \
        src/BoundingBoxManager.cpp \
        src/SELManager.cpp \
        src/SELDriver.cpp

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
        include/scene-evolution/interop.h \
        include/scene-evolution/OrientedBoundingBox.h \
        include/scene-evolution/BoundingBoxManager.h \
        include/SEL/SELManager.h \
        include/SEL/SELDriver.h

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

# Flex/Bison
LIBS += -lfl -ly

FLEXSOURCES = config/sel-flex.l
BISONSOURCES = config/sel-bison.y

OTHER_FILES += \
    $$FLEXSOURCES \
    $$BISONSOURCES

flexsource.input = FLEXSOURCES
flexsource.output = $$PWD/src/${QMAKE_FILE_BASE}.cpp
flexsource.commands = flex++ --header-file=$$PWD/include/SEL/${QMAKE_FILE_BASE}.h -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
flexsource.variable_out = SOURCES
flexsource.name = Flex Sources ${QMAKE_FILE_IN}
flexsource.CONFIG += target_predeps

QMAKE_EXTRA_COMPILERS += flexsource

flexheader.input = FLEXSOURCES
flexheader.output = $$PWD/include/SEL/${QMAKE_FILE_BASE}.h
flexheader.commands = @true
flexheader.variable_out = HEADERS
flexheader.name = Flex Headers ${QMAKE_FILE_IN}
flexheader.CONFIG += target_predeps no_link

QMAKE_EXTRA_COMPILERS += flexheader

bisonsource.input = BISONSOURCES
bisonsource.output = $$PWD/src/${QMAKE_FILE_BASE}.cpp
bisonsource.commands = bison -d --defines=$$PWD/include/SEL/${QMAKE_FILE_BASE}.h -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
bisonsource.variable_out = SOURCES
bisonsource.name = Bison Sources ${QMAKE_FILE_IN}
bisonsource.CONFIG += target_predeps

QMAKE_EXTRA_COMPILERS += bisonsource

bisonheader.input = BISONSOURCES
bisonheader.output = $$PWD/include/SEL/${QMAKE_FILE_BASE}.h
bisonheader.commands = @true
bisonheader.variable_out = HEADERS
bisonheader.name = Bison Headers ${QMAKE_FILE_IN}
bisonheader.CONFIG += target_predeps no_link

QMAKE_EXTRA_COMPILERS += bisonheader
