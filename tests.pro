include(shared.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += tests/main.cpp \
           tests/harmonic_oscillator.cpp \
    tests/tangent_stiffness.cpp \
    tests/bar_trusses.cpp \
    tests/large_deformation_beams.cpp

