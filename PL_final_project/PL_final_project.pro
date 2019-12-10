TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ast_node.cpp \
        interpreter.cpp \
        main.cpp

HEADERS += \
    ast_node.h \
    interpreter.h \
    peglib.h \
    visitor.h
