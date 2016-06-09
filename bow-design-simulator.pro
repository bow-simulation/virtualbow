#-------------------------------------------------
#
# Project created by QtCreator 2016-06-06T06:15:39
#
#-------------------------------------------------

QT += core gui
QT += widgets

TARGET = bow-design-simulator
TEMPLATE = app

SOURCES += src/main.cpp
SOURCES += src/fem/system.cpp

HEADERS += \
    src/fem/system.hpp \
    src/fem/view_base.hpp \
    src/fem/view.hpp

INCLUDEPATH += /home/s/Libraries/Eigen-3.2.8
