cmake_minimum_required(VERSION 3.10.2)
project(virtualbow)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Parameters

set(APPLICATION_NAME "VirtualBow")
set(APPLICATION_VERSION "0.8")
set(APPLICATION_WEBSITE "https://www.virtualbow.org")
set(APPLICATION_MAINTAINER "Stefan Pfeifer")
set(APPLICATION_COPYRIGHT "Copyright (C) 2016-2021 Stefan Pfeifer")
set(APPLICATION_LICENSE "GNU General Public License v3.0")
set(APPLICATION_DESCRIPTION_SHORT "Bow and arrow physics simulation")
set(APPLICATION_DESCRIPTION_LONG "Software tool for designing and simulating bows")

# External libraries

find_package(Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(NLopt REQUIRED)
find_package(Threads REQUIRED)                # System thread library (https://stackoverflow.com/a/39547577)
find_package(OpenGL REQUIRED)

get_target_property(QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
get_filename_component(QT_BINARY_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

configure_file(source/config.hpp.in ${CMAKE_BINARY_DIR}/config.hpp)
include_directories(source ${PROJECT_BINARY_DIR})
include_directories(${Boost_INCLUDE_DIRS})

# Target: Solver library

add_library(
    virtualbow-lib STATIC
    source/solver/fem/elements/BarElement.cpp
    source/solver/fem/elements/BeamElement.cpp
    source/solver/fem/elements/MassElement.cpp
    source/solver/fem/elements/ContactElement.cpp
    source/solver/fem/elements/ContactHandler.cpp
    source/solver/fem/elements/ConstraintElement.cpp
    source/solver/fem/Node.cpp
    source/solver/fem/Element.cpp
    source/solver/fem/StaticSolver.cpp
    source/solver/fem/DynamicSolver.cpp
    source/solver/fem/EigenvalueSolver.cpp
    source/solver/fem/System.cpp
    source/solver/model/BeamUtils.cpp
    source/solver/model/ContinuousLimb.cpp
    source/solver/model/LimbProperties.cpp
    source/solver/model/ProfileCurve.cpp
    source/solver/model/input/InputData.cpp
    source/solver/model/output/OutputData.cpp
    source/solver/model/BowModel.cpp
    source/solver/numerics/CubicSpline.cpp
    source/solver/numerics/Fresnel.cpp
    source/solver/numerics/Series.cpp
)

target_link_libraries(
    virtualbow-lib
    ${Boost_LIBRARIES}
    Eigen3::Eigen
    nlohmann_json::nlohmann_json
)

# Target: Solver executable

add_executable(
    virtualbow-slv
    source/solver/main.cpp
)

target_link_libraries(
    virtualbow-slv
    virtualbow-lib
    Qt5::Core
)

# Target: Gui executable

add_executable(
    virtualbow-gui
    resources/resources.qrc
    source/gui/main.cpp
    source/gui/MainWindow.cpp
    source/gui/MainWindow.hpp
    source/gui/BaseDialog.cpp
    source/gui/BaseDialog.hpp
    source/gui/HelpMenu.cpp
    source/gui/HelpMenu.hpp
    source/gui/PersistentDialog.cpp
    source/gui/PersistentDialog.hpp
    source/gui/PlotWidget.cpp
    source/gui/PlotWidget.hpp
    source/gui/BackgroundDialog.cpp
    source/gui/BackgroundDialog.hpp
    source/gui/SimulationDialog.cpp
    source/gui/SimulationDialog.hpp
    source/gui/EditableTabBar.cpp
    source/gui/EditableTabBar.hpp
    source/gui/RecentFilesMenu.cpp
    source/gui/RecentFilesMenu.hpp
    source/gui/dialogs/CommentDialog.cpp
    source/gui/dialogs/CommentDialog.hpp
    source/gui/dialogs/GroupDialog.cpp
    source/gui/dialogs/GroupDialog.hpp
    source/gui/dialogs/LayerDialog.cpp
    source/gui/dialogs/LayerDialog.hpp
    source/gui/dialogs/MassesDialog.cpp
    source/gui/dialogs/MassesDialog.hpp
    source/gui/dialogs/DampingDialog.cpp
    source/gui/dialogs/DampingDialog.hpp
    source/gui/dialogs/DimensionsDialog.cpp
    source/gui/dialogs/DimensionsDialog.hpp
    source/gui/dialogs/ProfileDialog.cpp
    source/gui/dialogs/ProfileDialog.hpp
    source/gui/dialogs/SettingsDialog.cpp
    source/gui/dialogs/SettingsDialog.hpp
    source/gui/dialogs/StringDialog.cpp
    source/gui/dialogs/StringDialog.hpp
    source/gui/dialogs/WidthDialog.cpp
    source/gui/dialogs/WidthDialog.hpp
    source/gui/editors/ModelEditor.cpp
    source/gui/editors/ModelEditor.hpp
    source/gui/editors/DoubleEditor.cpp
    source/gui/editors/DoubleEditor.hpp
    source/gui/editors/IntegerEditor.cpp
    source/gui/editors/IntegerEditor.hpp
    source/gui/editors/LayerEditor.cpp
    source/gui/editors/LayerEditor.hpp
    source/gui/editors/TableEditor.cpp
    source/gui/editors/TableEditor.hpp
    source/gui/editors/TableModel.cpp
    source/gui/editors/TableModel.hpp
    source/gui/editors/TableView.cpp
    source/gui/editors/TableView.hpp
    source/gui/editors/TreeEditor.cpp
    source/gui/editors/TreeEditor.hpp
    source/gui/units/Unit.cpp
    source/gui/units/Unit.hpp
    source/gui/units/UnitGroup.cpp
    source/gui/units/UnitGroup.hpp
    source/gui/units/UnitDialog.cpp
    source/gui/units/UnitDialog.hpp
    source/gui/units/UnitSystem.cpp
    source/gui/units/UnitSystem.hpp
    source/gui/views/LayerLegend.cpp
    source/gui/views/LayerLegend.hpp
    source/gui/views/LimbView.cpp
    source/gui/views/LimbView.hpp
    source/gui/views/LimbMesh.cpp
    source/gui/views/LimbMesh.hpp
    source/gui/views/LayerColors.cpp
    source/gui/views/LayerColors.hpp
    source/gui/views/ProfileView.cpp
    source/gui/views/ProfileView.hpp
    source/gui/views/SplineView.cpp
    source/gui/views/SplineView.hpp
    source/gui/qcustomplot/qcustomplot.cpp
)

target_link_libraries(
    virtualbow-gui
    virtualbow-lib
    Threads::Threads
    ${OPENGL_gl_LIBRARY}
    Qt5::Widgets
    Qt5::PrintSupport
)

# Target: Post executable

add_executable(
    virtualbow-post
    resources/resources.qrc
    source/post/main.cpp
    source/post/MainWindow.cpp
    source/post/MainWindow.hpp
    source/post/OutputWidget.cpp
    source/post/OutputWidget.hpp
    source/post/ComboPlot.cpp
    source/post/ComboPlot.hpp
    source/post/EnergyPlot.cpp
    source/post/EnergyPlot.hpp
    source/post/CurvaturePlot.cpp
    source/post/CurvaturePlot.hpp
    source/post/NumberGrid.cpp
    source/post/NumberGrid.hpp
    source/post/ShapePlot.cpp
    source/post/ShapePlot.hpp
    source/post/Slider.cpp
    source/post/Slider.hpp
    source/post/StressPlot.cpp
    source/post/StressPlot.hpp
    source/gui/qcustomplot/qcustomplot.cpp
    source/gui/RecentFilesMenu.cpp
    source/gui/RecentFilesMenu.hpp
    source/gui/HelpMenu.cpp
    source/gui/HelpMenu.hpp
    source/gui/BaseDialog.cpp
    source/gui/BaseDialog.hpp
    source/gui/PlotWidget.cpp
    source/gui/PlotWidget.hpp
    source/gui/BackgroundDialog.cpp
    source/gui/BackgroundDialog.hpp
    source/gui/views/LayerColors.cpp
    source/gui/views/LayerColors.hpp
    source/gui/units/Unit.cpp
    source/gui/units/Unit.hpp
    source/gui/units/UnitGroup.cpp
    source/gui/units/UnitGroup.hpp
    source/gui/units/UnitDialog.cpp
    source/gui/units/UnitDialog.hpp
    source/gui/units/UnitSystem.cpp
    source/gui/units/UnitSystem.hpp
)

target_link_libraries(
    virtualbow-post
    virtualbow-lib
    Qt5::Widgets
    Qt5::PrintSupport
)

# Target: Test executable

add_executable(
    virtualbow-test
    source/tests/main.cpp
    source/tests/model/BeamStiffnessMatrix.cpp
    source/tests/fem/BarTrusses.cpp
    source/tests/fem/HarmonicOscillator.cpp
    source/tests/fem/LargeDeformationBeams.cpp
    source/tests/fem/TangentStiffness.cpp
    source/tests/numerics/Geometry.cpp
    source/tests/numerics/FindInterval.cpp
)

target_link_libraries(
    virtualbow-test
    virtualbow-lib
    Catch2::Catch2
)

# Change output directories

set_target_properties(
    virtualbow-gui
    virtualbow-slv
    virtualbow-post
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/application
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/application
)

# Platform specifics

if(WIN32)
    add_subdirectory(platforms/windows)
elseif(APPLE)
    add_subdirectory(platforms/macos)
elseif(UNIX)
    add_subdirectory(platforms/linux)
endif()
