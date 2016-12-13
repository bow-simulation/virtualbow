include(dependencies.pri)

TARGET = bow-simulator
TEMPLATE = app

QT += core \
      gui \
      widgets \
      printsupport \

RESOURCES += ../resources/resources.qrc

HEADERS += \
    ../source/external/qcustomplot/qcustomplot.h \
    ../source/fem/elements/BarElement.hpp \
    ../source/fem/elements/BeamElement.hpp \
    ../source/fem/elements/MassElement.hpp \
    ../source/fem/Element.hpp \
    ../source/fem/Node.hpp \
    ../source/fem/Solver.hpp \
    ../source/fem/System.hpp \
    ../source/fem/View.hpp \
    ../source/gui/input/BowEditor.hpp \
    ../source/gui/input/CommentsDialog.hpp \
    ../source/gui/input/DoubleView.hpp \
    ../source/gui/input/IntegerView.hpp \
    ../source/gui/input/LimbViews.hpp \
    ../source/gui/input/NumberGroup.hpp \
    ../source/gui/input/SeriesView.hpp \
    ../source/gui/input/SettingsDialog.hpp \
    ../source/gui/output/ComboPlot.hpp \
    ../source/gui/output/EnergyPlot.hpp \
    ../source/gui/output/OutputDialog.hpp \
    ../source/gui/output/OutputGrid.hpp \
    ../source/gui/output/ShapePlot.hpp \
    ../source/gui/output/Slider.hpp \
    ../source/gui/output/StressPlot.hpp \
    ../source/gui/MainWindow.hpp \
    ../source/gui/Plot.hpp \
    ../source/gui/ProgressDialog.hpp \
    ../source/model/BowModel.hpp \
    ../source/model/DiscreteLimb.hpp \
    ../source/model/Document.hpp \
    ../source/model/InputData.hpp \
    ../source/model/OutputData.hpp \
    ../source/numerics/ArcCurve.hpp \
    ../source/numerics/CubicSpline.hpp \
    ../source/numerics/Curve.hpp \
    ../source/numerics/Domain.hpp \
    ../source/numerics/Linspace.hpp \
    ../source/numerics/SecantMethod.hpp \
    ../source/numerics/Series.hpp \
    ../source/numerics/TDMatrix.hpp \
    ../source/numerics/Units.hpp \
    ../source/utils/DynamicCastIterator.hpp \
    ../source/utils/Optional.hpp

SOURCES += \
    ../source/external/qcustomplot/qcustomplot.cpp \
    ../source/fem/elements/BarElement.cpp \
    ../source/fem/elements/BeamElement.cpp \
    ../source/fem/elements/MassElement.cpp \
    ../source/fem/Element.cpp \
    ../source/fem/Node.cpp \
    ../source/fem/Solver.cpp \
    ../source/fem/System.cpp \
    ../source/gui/input/BowEditor.cpp \
    ../source/gui/input/CommentsDialog.cpp \
    ../source/gui/input/DoubleView.cpp \
    ../source/gui/input/IntegerView.cpp \
    ../source/gui/input/LimbViews.cpp \
    ../source/gui/input/NumberGroup.cpp \
    ../source/gui/input/SeriesView.cpp \
    ../source/gui/input/SettingsDialog.cpp \
    ../source/gui/output/ComboPlot.cpp \
    ../source/gui/output/EnergyPlot.cpp \
    ../source/gui/output/OutputDialog.cpp \
    ../source/gui/output/OutputGrid.cpp \
    ../source/gui/output/ShapePlot.cpp \
    ../source/gui/output/Slider.cpp \
    ../source/gui/output/StressPlot.cpp \
    ../source/gui/MainWindow.cpp \
    ../source/gui/Plot.cpp \
    ../source/gui/ProgressDialog.cpp \
    ../source/model/BowModel.cpp \
    ../source/model/DiscreteLimb.cpp \
    ../source/model/InputData.cpp \
    ../source/model/OutputData.cpp \
    ../source/numerics/ArcCurve.cpp \
    ../source/numerics/CubicSpline.cpp \
    ../source/numerics/Series.cpp \
    ../source/main.cpp

# Compiler flags
CONFIG += c++14

# Platform specific
win32-g++{
    # http://eigen.tuxfamily.org/dox-devel/group__TopicWrongStackAlignment.html
    QMAKE_CXXFLAGS += -mincoming-stack-boundary=2
}
