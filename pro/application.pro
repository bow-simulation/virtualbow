include(dependencies.pri)

QT += core gui
QT += widgets
QT += charts

TARGET = bow-design-simulator
TEMPLATE = app

SOURCES += ../src/main.cpp \
    ../src/fem/elements/Element.cpp \
    ../src/gui/MainWindow.cpp \
    ../src/numerics/SplineFunction.cpp \
    ../src/numerics/StepFunction.cpp

HEADERS += \
    ../src/fem/elements/BarElement.hpp \
    ../src/fem/elements/MassElement.hpp \
    ../src/fem/elements/BeamElement.hpp \
    ../src/fem/elements/Element.hpp \
    ../src/gui/MainWindow.hpp \
    ../src/model/Limb.hpp \
    ../src/model/InputData.hpp \
    ../src/numerics/TDMatrix.hpp \
    ../src/numerics/SplineFunction.hpp \
    ../src/numerics/StepFunction.hpp
