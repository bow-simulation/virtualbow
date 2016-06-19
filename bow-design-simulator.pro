#-------------------------------------------------
#
# Project created by QtCreator 2016-06-06T06:15:39
#
#-------------------------------------------------

QT += core gui
QT += widgets

TARGET = bow-design-simulator
TEMPLATE = app

SOURCES += src/main.cpp \
    src/gui/MainWindow.cpp \
    src/fem/System.cpp

HEADERS += \
    src/fem/elements/element.hpp \
    src/gui/MainWindow.hpp \
    src/fem/ViewBase.hpp \
    src/fem/View.hpp \
    src/fem/System.hpp

INCLUDEPATH += /home/s/Libraries/Eigen-3.2.8
