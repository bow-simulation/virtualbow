cmake_minimum_required(VERSION 3.13.4)
project(virtualbow)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Parameters

set(APPLICATION_NAME "virtualbow")
set(APPLICATION_DISPLAY_NAME "VirtualBow")
set(APPLICATION_VERSION "0.7")
set(APPLICATION_WEBSITE "https://www.virtualbow.org")
set(APPLICATION_MAINTAINER "Stefan Pfeifer")
set(APPLICATION_COPYRIGHT "Copyright (C) 2016-2020 Stefan Pfeifer")
set(APPLICATION_LICENSE "GNU General Public License v3.0")
set(APPLICATION_DESCRIPTION_SHORT "Bow and arrow physics simulation")
set(APPLICATION_DESCRIPTION_LONG "Software tool for designing and simulating bows")

# External libraries

find_package(Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)                # System thread library (https://stackoverflow.com/a/39547577)
find_package(OpenGL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Target: Main library

configure_file(source/config.hpp.in ${CMAKE_BINARY_DIR}/config.hpp)
include_directories(source ${PROJECT_BINARY_DIR})

add_library(
    main-library
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
    source/fem/EigenvalueSolver.cpp
    source/fem/System.cpp
    source/bow/BeamUtils.cpp
    source/bow/ContinuousLimb.cpp
    source/bow/LimbProperties.cpp
    source/bow/ProfileCurve.cpp
    source/bow/input/InputData.cpp
    source/bow/output/OutputData.cpp
    source/bow/BowModel.cpp
    source/numerics/CubicSpline.cpp
    source/numerics/Fresnel.cpp
    source/numerics/Series.cpp
    source/gui/Application.cpp
    source/gui/Application.hpp
    source/gui/MainWindow.cpp
    source/gui/MainWindow.hpp
    source/gui/BaseDialog.cpp
    source/gui/BaseDialog.hpp
    source/gui/PersistentDialog.cpp
    source/gui/PersistentDialog.hpp
    source/gui/PlotWidget.cpp
    source/gui/PlotWidget.hpp
    source/gui/ProgressDialog.cpp
    source/gui/ProgressDialog.hpp
    source/gui/EditableTabBar.cpp
    source/gui/EditableTabBar.hpp
    source/gui/input/dialogs/CommentDialog.cpp
    source/gui/input/dialogs/CommentDialog.hpp
    source/gui/input/dialogs/GroupDialog.cpp
    source/gui/input/dialogs/GroupDialog.hpp
    source/gui/input/dialogs/LayerDialog.cpp
    source/gui/input/dialogs/LayerDialog.hpp
    source/gui/input/dialogs/MassesDialog.cpp
    source/gui/input/dialogs/MassesDialog.hpp
    source/gui/input/dialogs/DampingDialog.cpp
    source/gui/input/dialogs/DampingDialog.hpp
    source/gui/input/dialogs/DimensionsDialog.cpp
    source/gui/input/dialogs/DimensionsDialog.hpp
    source/gui/input/dialogs/ProfileDialog.cpp
    source/gui/input/dialogs/ProfileDialog.hpp
    source/gui/input/dialogs/SettingsDialog.cpp
    source/gui/input/dialogs/SettingsDialog.hpp
    source/gui/input/dialogs/StringDialog.cpp
    source/gui/input/dialogs/StringDialog.hpp
    source/gui/input/dialogs/WidthDialog.cpp
    source/gui/input/dialogs/WidthDialog.hpp
    source/gui/input/editors/BowEditor.cpp
    source/gui/input/editors/BowEditor.hpp
    source/gui/input/editors/DoubleEditor.cpp
    source/gui/input/editors/DoubleEditor.hpp
    source/gui/input/editors/IntegerEditor.cpp
    source/gui/input/editors/IntegerEditor.hpp
    source/gui/input/editors/LayerEditor.cpp
    source/gui/input/editors/LayerEditor.hpp
    source/gui/input/editors/SeriesEditor.cpp
    source/gui/input/editors/SeriesEditor.hpp
    source/gui/input/editors/TreeEditor.cpp
    source/gui/input/editors/TreeEditor.hpp
    source/gui/input/editors/TreeItem.hpp
    source/gui/input/views/AABB.cpp
    source/gui/input/views/AABB.hpp
    source/gui/input/views/LayerLegend.cpp
    source/gui/input/views/LayerLegend.hpp
    source/gui/input/views/LimbView.cpp
    source/gui/input/views/LimbView.hpp
    source/gui/input/views/LimbMesh.cpp
    source/gui/input/views/LimbMesh.hpp
    source/gui/input/views/LayerColors.cpp
    source/gui/input/views/LayerColors.hpp
    source/gui/input/views/ProfileView.cpp
    source/gui/input/views/ProfileView.hpp
    source/gui/input/views/SplineView.cpp
    source/gui/input/views/SplineView.hpp
    source/gui/output/ComboPlot.cpp
    source/gui/output/ComboPlot.hpp
    source/gui/output/EnergyPlot.cpp
    source/gui/output/EnergyPlot.hpp
    source/gui/output/CurvaturePlot.cpp
    source/gui/output/CurvaturePlot.hpp
    source/gui/output/NumberGrid.cpp
    source/gui/output/NumberGrid.hpp
    source/gui/output/OutputWindow.cpp
    source/gui/output/OutputWindow.hpp
    source/gui/output/ShapePlot.cpp
    source/gui/output/ShapePlot.hpp
    source/gui/output/Slider.cpp
    source/gui/output/Slider.hpp
    source/gui/output/StressPlot.cpp
    source/gui/output/StressPlot.hpp
    source/external/qcustomplot/qcustomplot.cpp
)

target_link_libraries(
    main-library
    Threads::Threads
    ${OPENGL_gl_LIBRARY}
    Qt5::Widgets
    Qt5::PrintSupport
    ${Boost_LIBRARIES}
    Eigen3::Eigen
    nlohmann_json::nlohmann_json
)

# Target: Main executable

add_executable(virtualbow MACOSX_BUNDLE source/main.cpp resources/resources.qrc)
target_link_libraries(virtualbow main-library)

# Target: Test executable

add_executable(
    virtualbow-tests
    tests/main.cpp
    tests/bow/BeamStiffnessMatrix.cpp
    tests/fem/BarTrusses.cpp
    tests/fem/HarmonicOscillator.cpp
    tests/fem/LargeDeformationBeams.cpp
    tests/fem/TangentStiffness.cpp
    tests/numerics/Geometry.cpp
)
target_link_libraries(virtualbow-tests main-library Catch2::Catch2)

# Platform specifics

if(WIN32)
    add_subdirectory(platforms/windows)
elseif(APPLE)
    add_subdirectory(platforms/macos)
elseif(UNIX)
    add_subdirectory(platforms/linux)
endif()
