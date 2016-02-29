#-------------------------------------------------
#
# Project created by QtCreator 2015-10-04T12:20:17
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OPAEditor
TEMPLATE = app

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    operatorwidget.cpp \
    rs232/rs232-linux.c \
    rs232/rs232-win.c \
    globals.cpp \
    midi/midiin-linux.cpp \
    midi/midiin-win.cpp \
    midi/midiin-mac.cpp \
    opa.cpp \
    programfile.cpp \
    programwidget.cpp \
    led.cpp \
    envelope.cpp

HEADERS += \
    mainwindow.h \
    operatorwidget.h \
    rs232/rs232.h \
    globals.h \
    midi/midiin.h \
    opa.h \
    programfile.h \
    programwidget.h \
    led.h \
    envelope.h

FORMS += \
    mainwindow.ui \
    operatorwidget.ui \
    programwidget.ui

win32:LIBS += \
    -luser32 \
    -lwinmm

linux:LIBS += \
    -lasound

macx:LIBS += \
    -framework CoreAudio \
    -framework CoreFoundation \
    -framework CoreMidi

QMAKE_CFLAGS += \
    -std=c99

RESOURCES += \
    resources.qrc

RC_FILE = winrc.rc

DISTFILES += \
    winrc.rc
