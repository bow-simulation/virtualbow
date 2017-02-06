INCLUDEPATH += /mnt/Daten/Libraries/Eigen-3.3.1 \          # http://eigen.tuxfamily.org
               /mnt/Daten/Libraries/Catch-1.5.9/include \  # https://github.com/philsquared/Catch
               /mnt/Daten/Libraries/Jsoncons-0.99.5/src \  # https://github.com/danielaparker/jsoncons
               /mnt/Daten/Libraries/Boost-1.62.0 \         # http://www.boost.org

TARGET = bow-simulator
TEMPLATE = app

# install
target.path = /bin
INSTALLS += target

# Todo: Remove opengl?
QT += core gui widgets opengl printsupport

# Todo: Remove what's not needed
QT += 3dcore 3drender 3dinput 3dextras

RESOURCES += resources/resources.qrc

HEADERS += \
    source/external/qcustomplot/qcustomplot.h \
    source/fem/elements/BarElement.hpp \
    source/fem/elements/BeamElement.hpp \
    source/fem/elements/MassElement.hpp \
    source/fem/Element.hpp \
    source/fem/Node.hpp \
    source/fem/Solver.hpp \
    source/fem/System.hpp \
    source/fem/View.hpp \
    source/gui/input/BowEditor.hpp \
    source/gui/input/CommentsDialog.hpp \
    source/gui/input/DoubleView.hpp \
    source/gui/input/IntegerView.hpp \
    source/gui/input/NumberGroup.hpp \
    source/gui/input/SeriesView.hpp \
    source/gui/input/SettingsDialog.hpp \
    source/gui/output/ComboPlot.hpp \
    source/gui/output/EnergyPlot.hpp \
    source/gui/output/OutputDialog.hpp \
    source/gui/output/OutputGrid.hpp \
    source/gui/output/ShapePlot.hpp \
    source/gui/output/Slider.hpp \
    source/gui/output/StressPlot.hpp \
    source/gui/MainWindow.hpp \
    source/gui/Plot.hpp \
    source/gui/ProgressDialog.hpp \
    source/model/BowModel.hpp \
    source/model/DiscreteLimb.hpp \
    source/model/Document.hpp \
    source/model/InputData.hpp \
    source/model/OutputData.hpp \
    source/numerics/ArcCurve.hpp \
    source/numerics/CubicSpline.hpp \
    source/numerics/Curve.hpp \
    source/numerics/Domain.hpp \
    source/numerics/Linspace.hpp \
    source/numerics/SecantMethod.hpp \
    source/numerics/Series.hpp \
    source/numerics/TDMatrix.hpp \
    source/numerics/Units.hpp \
    source/utils/DynamicCastIterator.hpp \
    source/utils/Optional.hpp \
    source/gui/input/SplineView.hpp \
    source/gui/input/ProfileView.hpp \
    source/gui/input/GeometryView.hpp

SOURCES += \
    source/external/qcustomplot/qcustomplot.cpp \
    source/fem/elements/BarElement.cpp \
    source/fem/elements/BeamElement.cpp \
    source/fem/elements/MassElement.cpp \
    source/fem/Element.cpp \
    source/fem/Node.cpp \
    source/fem/Solver.cpp \
    source/fem/System.cpp \
    source/gui/input/BowEditor.cpp \
    source/gui/input/CommentsDialog.cpp \
    source/gui/input/DoubleView.cpp \
    source/gui/input/IntegerView.cpp \
    source/gui/input/NumberGroup.cpp \
    source/gui/input/SeriesView.cpp \
    source/gui/input/SettingsDialog.cpp \
    source/gui/output/ComboPlot.cpp \
    source/gui/output/EnergyPlot.cpp \
    source/gui/output/OutputDialog.cpp \
    source/gui/output/OutputGrid.cpp \
    source/gui/output/ShapePlot.cpp \
    source/gui/output/Slider.cpp \
    source/gui/output/StressPlot.cpp \
    source/gui/MainWindow.cpp \
    source/gui/Plot.cpp \
    source/gui/ProgressDialog.cpp \
    source/model/BowModel.cpp \
    source/model/DiscreteLimb.cpp \
    source/model/InputData.cpp \
    source/model/OutputData.cpp \
    source/numerics/ArcCurve.cpp \
    source/numerics/CubicSpline.cpp \
    source/numerics/Series.cpp \
    source/main.cpp \
    source/gui/input/SplineView.cpp \
    source/gui/input/ProfileView.cpp \
    source/gui/input/LimbView.cpp

# Compiler flags
CONFIG += c++14

# Platform specific
win32-g++{
    # http://eigen.tuxfamily.org/dox-devel/group__TopicWrongStackAlignment.html
    QMAKE_CXXFLAGS += -mincoming-stack-boundary=2
}

