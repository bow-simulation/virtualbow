cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 14)
project(bow-simulator)

# Dependencies

include_directories("build/eigen")
include_directories("build/catch/include")
include_directories("build/jsoncons/src")
include_directories("build/boost")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)   # Add resource files to sources, cmake does the rest
set(CMAKE_AUTOUIC ON)

# Add files, compile and link

include_directories(source)
add_executable(${PROJECT_NAME}
    source/main.cpp
    resources/resources.qrc
    source/external/qcustomplot/qcustomplot.cpp

    source/fem/elements/BarElement.cpp
    source/fem/elements/BeamElement.cpp
    source/fem/elements/MassElement.cpp
    source/fem/Element.cpp
    source/fem/Node.cpp
    source/fem/Solver.cpp
    source/fem/System.cpp

    source/gui/MainWindow.cpp
    source/gui/PlotWidget.cpp
    source/gui/ProgressDialog.cpp

    source/gui/input/BowEditor.cpp
    source/gui/input/ModelTree.cpp

    source/gui/input/CommentsDialog.cpp
    source/gui/input/ProfileDialog.cpp
    source/gui/input/DoubleView.cpp
    source/gui/input/IntegerView.cpp
    source/gui/input/NumberGroup.cpp
    source/gui/input/SeriesView.cpp
    source/gui/input/LimbView.cpp
    #source/gui/input/LimbSource.cpp
    source/gui/input/SplineView.cpp
    source/gui/input/ProfileView.cpp
    source/gui/input/LimbView.cpp

    source/gui/output/ComboPlot.cpp
    source/gui/output/EnergyPlot.cpp
    source/gui/output/OutputDialog.cpp
    source/gui/output/OutputGrid.cpp
    source/gui/output/ShapePlot.cpp
    source/gui/output/Slider.cpp
    source/gui/output/StressPlot.cpp

    source/model/BowModel.cpp
    source/model/LimbProperties.cpp
    source/model/InputData.cpp
    source/model/OutputData.cpp
    source/model/Document.hpp   # moc

    source/numerics/ArcCurve.cpp
    source/numerics/CubicSpline.cpp
    source/numerics/Series.cpp
)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::PrintSupport)

# Install

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
