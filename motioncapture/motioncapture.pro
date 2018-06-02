TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

SOURCES += main.cpp \
    functions.cpp

LIBS += `pkg-config opencv --libs`

HEADERS += \
    functions.h
