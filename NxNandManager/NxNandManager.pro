#-------------------------------------------------
#
# Project created by QtCreator 2019-02-22T12:38:11
#
#-------------------------------------------------

#QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NxNandManager
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

CONFIG += c++11
CONFIG += console

SOURCES += \
    DiskSector.cpp \
    hex_string.cpp \
    keyset.cpp \
    mainwindow.cpp \
    main.cpp \
    NxStorage.cpp \
    partitionmanager.cpp \
    properties.cpp \
    utils.cpp \
    worker.cpp \
    opendrive.cpp \
    xts_crypto.cpp

HEADERS += \
    DiskSector.h \
    hex_string.h \
    keyset.h \
    mainwindow.h \
    NxStorage.h \
    partitionmanager.h \
    properties.h \
    utils.h \
    types.h \
    NxNandManager.h \
    worker.h \
    opendrive.h \
    xts_crypto.h

FORMS += \
    mainwindow.ui \
    opendrive.ui \
    keyset.ui \
    properties.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    application.qrc

DISTFILES +=

QT += winextras

RC_FILE = NxNandManager.rc

LIBS += -L$$PWD/../../../../../Users/elibo/OneDrive/Documents/Switch/openssl-1.1.1c-win32-mingw/lib/ -lcrypto
INCLUDEPATH += $$PWD/../../../../../Users/elibo/OneDrive/Documents/Switch/openssl-1.1.1c-win32-mingw/include
DEPENDPATH += $$PWD/../../../../../Users/elibo/OneDrive/Documents/Switch/openssl-1.1.1c-win32-mingw/include

#LIBS += -L$$PWD/../../../../../Users/elibo/OneDrive/Documents/Switch/openssl-1.1.1c-win64-mingw/lib/ -lcrypto
#INCLUDEPATH += $$PWD/../../../../../Users/elibo/OneDrive/Documents/Switch/openssl-1.1.1c-win64-mingw/include
#DEPENDPATH += $$PWD/../../../../../Users/elibo/OneDrive/Documents/Switch/openssl-1.1.1c-win64-mingw/include
