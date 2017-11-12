cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 14)
project(bow-simulator)

# Dependencies

include_directories("build/eigen")
include_directories("build/catch/include")
include_directories("build/json/src")
include_directories("build/boost")

find_package(VTK REQUIRED PATHS "build/vtk")
include(${VTK_USE_FILE})

find_package(Qt5X11Extras)    # Why is this platform specific stuff needed on Linux? It's not even linked.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)   # Add resource files to sources, cmake does the rest
set(CMAKE_AUTOUIC ON)

# Build main executable

include_directories(source)
add_executable(${PROJECT_NAME}
   source/main.cpp
   resources/resources.qrc

   source/external/qcustomplot/qcustomplot.cpp

   source/fem/elements/BarElement.cpp
   source/fem/elements/BeamElement.cpp
   source/fem/elements/MassElement.cpp
   source/fem/elements/ContactElement.cpp
   source/fem/elements/ContactHandler.cpp
   source/fem/elements/ConstraintElement.cpp
   source/fem/Node.cpp
   source/fem/Element.cpp
   source/fem/StaticSolver.cpp
   source/fem/DynamicSolver.cpp
   source/fem/System.cpp

   source/gui/Application.cpp
   source/gui/MainWindow.cpp
   source/gui/PersistentDialog.cpp
   source/gui/PlotWidget.cpp
   source/gui/ProgressDialog.cpp
   source/gui/ProgressDialog.hpp    # moc

   source/gui/input/BowEditor.cpp
   source/gui/input/ModelTree.cpp
   source/gui/input/CommentDialog.cpp
   source/gui/input/ProfileDialog.cpp
   source/gui/input/NumberGroup.cpp

   source/gui/views/DoubleView.cpp
   source/gui/views/IntegerView.cpp
   source/gui/views/SeriesView.cpp
   source/gui/views/SplineView.cpp
   source/gui/views/ProfileView.cpp
   source/gui/views/LimbView.cpp
   source/gui/views/LimbSource.cpp

   source/gui/output/ComboPlot.cpp
   source/gui/output/EnergyPlot.cpp
   source/gui/output/OutputDialog.cpp
   source/gui/output/OutputGrid.cpp
   source/gui/output/ShapePlot.cpp
   source/gui/output/Slider.cpp
   source/gui/output/StressPlot.cpp

   source/model/BowModel.cpp
   source/model/input/InputData.cpp
   source/model/document/Document.hpp    # moc

   source/numerics/ArcCurve.cpp
   source/numerics/CubicSpline.cpp
   source/numerics/Series.cpp
)

target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::PrintSupport)

# Build tests

#add_executable(tests
#    source/tests/main.cpp
#    source/tests/tangent_stiffness.cpp
#    source/tests/large_deformation_beams.cpp
#    source/tests/harmonic_oscillator.cpp
#    source/tests/bar_trusses.cpp

#    source/fem/elements/BarElement.cpp
#    source/fem/elements/BeamElement.cpp
#    source/fem/elements/MassElement.cpp
#    source/fem/elements/ContactElement.cpp
#    source/fem/elements/ContactSurface.cpp
#    source/fem/elements/ConstraintElement.cpp
#    source/fem/Node.cpp
#    source/fem/DynamicSolver.cpp
#    source/fem/System.cpp
#)

# Install

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
