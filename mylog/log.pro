
TEMPLATE = app
QT -= gui

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/Log.h

SOURCES += \
    $$PWD/Log.cpp\
    $$PWD/main.cpp

include($$PWD/log4qt/log4qt.pri)
