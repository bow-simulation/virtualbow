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
    ../src/numerics/StepFunction.cpp \
    ../src/fem/System.cpp \
    ../src/numerics/Domain.cpp \
    ../src/numerics/CubicSpline.cpp \
    ../src/numerics/LinearSpline.cpp \
    ../src/gui/DocView.cpp

HEADERS += \
    ../src/fem/elements/BarElement.hpp \
    ../src/fem/elements/MassElement.hpp \
    ../src/fem/elements/BeamElement.hpp \
    ../src/fem/elements/Element.hpp \
    ../src/gui/MainWindow.hpp \
    ../src/numerics/TDMatrix.hpp \
    ../src/numerics/StepFunction.hpp \
    ../src/gui/BowEditor.hpp \
    ../src/fem/Node.hpp \
    ../src/fem/System.hpp \
    ../src/fem/View.hpp \
    ../src/numerics/NumParam.hpp \
    ../src/numerics/Domain.hpp \
    ../src/numerics/Units.hpp \
    ../src/gui/NumberView.hpp \
    ../src/numerics/DataSeries.hpp \
    ../src/gui/SeriesEditor.hpp \
    ../src/model/InputData.hpp \
    ../src/model/DiscreteLimb.hpp \
    ../src/model/BowModel.hpp \
    ../src/model/OutputData.hpp \
    ../src/numerics/SecantMethod.hpp \
    ../src/fem/elements/ContactElement1D.hpp \
    ../src/numerics/CubicSpline.hpp \
    ../src/numerics/LinearSpline.hpp \
    ../src/gui/DocView.hpp
