include(dependencies.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../source/tests/bar_trusses.cpp \
    ../source/tests/tangent_stiffness.cpp \
    ../source/tests/harmonic_oscillator.cpp \
    ../source/tests/main.cpp \
    ../source/tests/large_deformation_beams.cpp \
    ../source/fem/System.cpp \
    ../source/fem/elements/Element.cpp

HEADERS += \
    ../source/fem/Node.hpp \
    ../source/fem/View.hpp \
    ../source/fem/System.hpp \
    ../source/fem/elements/BarElement.hpp \
    ../source/fem/elements/MassElement.hpp \
    ../source/fem/elements/Element.hpp \
    ../source/fem/elements/BeamElement.hpp


