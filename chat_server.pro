TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -pthread
QMAKE_CXXFLAGS += -pthread
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp

