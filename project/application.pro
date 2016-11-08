include(dependencies.pri)

TARGET = bow-design-simulator
TEMPLATE = app

CONFIG += c++14

QT += core \
      gui \
      widgets \
      printsupport \

SOURCES += ../source/main.cpp \
    ../source/fem/elements/Element.cpp \
    ../source/gui/MainWindow.cpp \
    ../source/numerics/StepFunction.cpp \
    ../source/fem/System.cpp \
    ../source/numerics/CubicSpline.cpp \
    ../source/numerics/LinearSpline.cpp \
    ../source/external/qcustomplot/qcustomplot.cpp \
    ../source/gui/BowEditor.cpp \
    ../source/gui/CommentsDialog.cpp \
    ../source/gui/DoubleView.cpp \
    ../source/gui/IntegerView.cpp \
    ../source/gui/NumberGroup.cpp \
    ../source/gui/Plot.cpp \
    ../source/gui/PlotViews.cpp \
    ../source/gui/SeriesView.cpp \
    ../source/gui/SettingsDialog.cpp \
    ../source/gui/ProgressDialog.cpp

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
    ../source/model/DiscreteLimb.hpp \
    ../source/model/BowModel.hpp \
    ../source/model/OutputData.hpp \
    ../source/numerics/SecantMethod.hpp \
    ../source/fem/elements/ContactElement1D.hpp \
    ../source/numerics/CubicSpline.hpp \
    ../source/numerics/LinearSpline.hpp \
    ../source/gui/Document.hpp \
    ../source/gui/SettingsDialog.hpp \
    ../source/gui/IntegerView.hpp \
    ../source/model/Document.hpp \
    ../source/model/InputData.hpp \
    ../source/model/Document.hpp \
    ../source/gui/DoubleView.hpp \
    ../source/gui/NumberGroup.hpp \
    ../source/numerics/Series.hpp \
    ../source/gui/SeriesView.hpp \
    ../source/gui/Plot.hpp \
    ../source/gui/PlotViews.hpp \
    ../source/numerics/ArcCurve.hpp \
    ../source/gui/CommentsDialog.hpp \
    ../source/external/qcustomplot/qcustomplot.h \
    ../source/gui/ProgressDialog.hpp

RESOURCES += ../resources/resources.qrc
