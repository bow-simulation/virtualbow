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
    src/fem/System.cpp \
    src/fem/elements/Element.cpp

HEADERS += \
    src/gui/MainWindow.hpp \
    src/fem/System.hpp \
    src/fem/elements/Element.hpp \
    src/fem/View.hpp \
    src/fem/elements/BarElement.hpp \
    src/fem/Node.hpp \
    src/fem/elements/MassElement.hpp

INCLUDEPATH += /home/s/Libraries/Eigen-3.2.8
