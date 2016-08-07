include(dependencies.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../tests/bar_trusses.cpp \
    ../tests/tangent_stiffness.cpp \
    ../tests/harmonic_oscillator.cpp \
    ../tests/main.cpp \
    ../tests/large_deformation_beams.cpp \
    ../src/fem/System.cpp \
    ../src/fem/elements/Element.cpp

HEADERS += \
    ../src/fem/Node.hpp \
    ../src/fem/View.hpp \
    ../src/fem/System.hpp \
    ../src/fem/elements/BarElement.hpp \
    ../src/fem/elements/MassElement.hpp \
    ../src/fem/elements/Element.hpp \
    ../src/fem/elements/BeamElement.hpp


