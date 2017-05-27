cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 14)
project(bow-simulator)

# Eigen
set(EIGEN_DIR "build/eigen" CACHE FILEPATH "Eigen directory")
include_directories(${EIGEN_DIR})

# Catch
set(CATCH_DIR "build/catch" CACHE FILEPATH "Catch directory")
include_directories(${CATCH_DIR}/include)

# Jsonsons
set(JSONCONS_DIR "build/jsoncons" CACHE FILEPATH "Jsoncons directory")
include_directories(${JSONCONS_DIR}/src)

# Boost
set(BOOST_DIR "build/boost" CACHE FILEPATH "Boost directory")
include_directories(${BOOST_DIR})

# VTK
find_package(VTK 7.1.0 REQUIRED PATHS "build/vtk")
include(${VTK_USE_FILE})

# Qt
find_package(Qt5Core REQUIRED PATHS "build/qt/lib/cmake/Qt5Core")


find_package(Qt5Widgets REQUIRED PATHS "build/qt/lib/cmake/Qt5Widgets")
find_package(Qt5PrintSupport REQUIRED PATHS "build/qt/lib/cmake/Qt5PrintSupport")
find_package(Qt5X11Extras PATHS "build/qt/lib/cmake/Qt5X11Extras")    # On Linux

find_package(Qt5Core REQUIRED PATHS "build/qt/lib/cmake/Qt5Core")
find_package(Qt5Gui REQUIRED PATHS "build/qt/lib/cmake/Qt5Gui")
find_package(Qt5Sql REQUIRED PATHS "build/qt/lib/cmake/Qt5Sql")


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
    source/gui/input/LimbSource.cpp
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

target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::PrintSupport Qt5::Core Qt5::Gui Qt5::Sql Qt5::X11Extras)

#link_directories("build/qt/lib")
#target_link_libraries(${PROJECT_NAME} libpng)
