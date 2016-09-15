include(dependencies.pri)

TARGET = bow-design-simulator
TEMPLATE = app

CONFIG += c++14

QT += core \
      gui \
      widgets \
      charts \
      printsupport \

SOURCES += ../source/main.cpp \
    ../source/fem/elements/Element.cpp \
    ../source/gui/MainWindow.cpp \
    ../source/numerics/StepFunction.cpp \
    ../source/fem/System.cpp \
    ../source/numerics/CubicSpline.cpp \
    ../source/numerics/LinearSpline.cpp \
    ../source/gui/NumberEditor.cpp \
    ../source/numerics/Domain.cpp

HEADERS += \
    ../source/fem/elements/BarElement.hpp \
    ../source/fem/elements/MassElement.hpp \
    ../source/fem/elements/BeamElement.hpp \
    ../source/fem/elements/Element.hpp \
    ../source/gui/MainWindow.hpp \
    ../source/numerics/TDMatrix.hpp \
    ../source/numerics/StepFunction.hpp \
    ../source/gui/BowEditor.hpp \
    ../source/fem/Node.hpp \
    ../source/fem/System.hpp \
    ../source/fem/View.hpp \
    ../source/numerics/NumParam.hpp \
    ../source/numerics/Domain.hpp \
    ../source/numerics/Units.hpp \
    ../source/numerics/DataSeries.hpp \
    ../source/gui/SeriesEditor.hpp \
    ../source/model/InputData.hpp \
    ../source/model/DiscreteLimb.hpp \
    ../source/model/BowModel.hpp \
    ../source/model/OutputData.hpp \
    ../source/numerics/SecantMethod.hpp \
    ../source/fem/elements/ContactElement1D.hpp \
    ../source/numerics/CubicSpline.hpp \
    ../source/numerics/LinearSpline.hpp \
    ../source/gui/Document.hpp \
    ../source/gui/ScalarView.hpp \
    ../source/gui/SeriesView.hpp \
    ../source/gui/DoubleEditor.hpp \
    ../source/gui/Plot.hpp

RESOURCES += ../resources/resources.qrc
