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
    ../source/fem/System2.cpp \
    ../source/fem/Node2.cpp \
    ../source/fem/Element2.cpp

HEADERS += \
    ../source/fem/View.hpp \
    ../source/fem/System2.hpp \
    ../source/fem/Node2.hpp \
    ../source/fem/elements/BarElement2.hpp \
    ../source/fem/Solver.hpp \
    ../source/numerics/Linspace.hpp \
    ../source/fem/elements/MassElement2.hpp \
    ../source/fem/Element2.hpp \
    ../source/fem/elements/BeamElement2.hpp


