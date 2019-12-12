TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        astnode.cpp \
        interpreter.cpp \
        main.cpp

HEADERS += \
    astnode.h \
    interpreter.h \
    peglib.h \
    visitor.h
