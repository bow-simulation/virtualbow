include(dependencies.pri)

TARGET = bow-design-simulator
TEMPLATE = app

QT += core \
      gui \
      widgets \
      charts \
      printsupport \

SOURCES += ../src/main.cpp \
    ../src/fem/elements/Element.cpp \
    ../src/gui/MainWindow.cpp \
    ../src/numerics/SplineFunction.cpp \
    ../src/numerics/StepFunction.cpp \
    ../src/fem/System.cpp

HEADERS += \
    ../src/fem/elements/BarElement.hpp \
    ../src/fem/elements/MassElement.hpp \
    ../src/fem/elements/BeamElement.hpp \
    ../src/fem/elements/Element.hpp \
    ../src/gui/MainWindow.hpp \
    ../src/model/InputData.hpp \
    ../src/numerics/TDMatrix.hpp \
    ../src/numerics/SplineFunction.hpp \
    ../src/numerics/StepFunction.hpp \
    ../src/model/Limb.hpp \
    ../src/gui/CurveEditor.hpp \
    ../src/gui/BowEditor.hpp \
    ../src/fem/Node.hpp \
    ../src/fem/System.hpp \
    ../src/fem/View.hpp \
    ../src/numerics/NumParam.hpp
