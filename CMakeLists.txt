cmake_minimum_required(VERSION 3.2.2)
include(ExternalProject)
project(bow-simulator)
set(CMAKE_CXX_STANDARD 14)

# Parameters used for configuring files
set(APPLICATION_NAME "bow-simulator")
set(APPLICATION_DISPLAY_NAME "Bow Simulator")
set(APPLICATION_VERSION "0.6")
set(APPLICATION_WEBSITE "http://bow-simulator.org")
set(APPLICATION_MAINTAINER "Stefan Pfeifer")
set(APPLICATION_COPYRIGHT "Copyright (C) 2016-2019 Stefan Pfeifer")
set(APPLICATION_LICENSE "GNU General Public License v3.0")
set(APPLICATION_DESCRIPTION_SHORT "Bow and arrow physics simulation")
set(APPLICATION_DESCRIPTION_LONG "Software tool for simulating the static and dynamic performance of bow designs")

# External libraries

# Qt (must be provided by the system/developer)
find_package(Qt5 COMPONENTS Widgets PrintSupport)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Eigen (download from url)
ExternalProject_Add(
    eigen
    URL "http://bitbucket.org/eigen/eigen/get/3.3.4.tar.gz"
    URL_MD5 "1a47e78efe365a97de0c022d127607c3"
    PREFIX ${CMAKE_BINARY_DIR}/eigen
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(eigen SOURCE_DIR)
include_directories(${SOURCE_DIR})

# Catch (download from url)
ExternalProject_Add(
    catch
    URL "https://github.com/philsquared/Catch/archive/v1.9.3.tar.gz"
    URL_MD5 "ca8817b67031d8e5c134a3cb36ed8397"
    PREFIX ${CMAKE_BINARY_DIR}/catch
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(catch SOURCE_DIR)
include_directories(${SOURCE_DIR}/include)

# Json (download from url)
ExternalProject_Add(
    json
    URL "https://github.com/nlohmann/json/archive/v3.0.1.tar.gz"
    URL_MD5 "b03467284d9f608bd1b432315dc08d7d"
    PREFIX ${CMAKE_BINARY_DIR}/json
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

externalproject_Get_Property(json SOURCE_DIR)
include_directories(${SOURCE_DIR}/src)

# Boost (download from url)
ExternalProject_Add(
    boost
    URL "https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz"
    URL_MD5 "d275cd85b00022313c171f602db59fc5"
    PREFIX ${CMAKE_BINARY_DIR}/boost
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

externalproject_get_property(boost SOURCE_DIR)
include_directories(${SOURCE_DIR})

# System thread library (https://stackoverflow.com/a/39547577)
find_package(Threads REQUIRED)

# Build application library

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
    source/fem/System.cpp
    source/bow/LimbProperties.cpp
    source/bow/LayerProperties.cpp
    source/bow/input/InputData.cpp
    source/bow/BowModel.cpp
    source/numerics/ArcCurve.cpp
    source/numerics/CubicSpline.cpp
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
    source/gui/output/OutputDialog.cpp
    source/gui/output/OutputDialog.hpp
    source/gui/output/OutputGrid.cpp
    source/gui/output/OutputGrid.hpp
    source/gui/output/ShapePlot.cpp
    source/gui/output/ShapePlot.hpp
    source/gui/output/Slider.cpp
    source/gui/output/Slider.hpp
    source/gui/output/StressPlot.cpp
    source/gui/output/StressPlot.hpp
    source/external/qcustomplot/qcustomplot.cpp
)

add_dependencies(main-library eigen catch json boost)
target_link_libraries(main-library Qt5::Widgets Qt5::PrintSupport Threads::Threads)

# Build application executable

add_executable(bow-simulator source/main.cpp resources/resources.qrc)
target_link_libraries(bow-simulator main-library)

# Build test executable

add_executable(
    bow-simulator-test
    tests/main.cpp
    tests/fem/BarTrusses.cpp
    tests/fem/HarmonicOscillator.cpp
    tests/fem/LargeDeformationBeams.cpp
    tests/fem/TangentStiffness.cpp
)

target_compile_definitions(bow-simulator-test PRIVATE _USE_MATH_DEFINES)    # Defines math constants on MSVC
target_link_libraries(bow-simulator-test main-library)

# Build release packages for different platforms

if(UNIX)
    add_subdirectory(platforms/linux)
endif()

if(WIN32)
    add_subdirectory(platforms/windows)
endif()

if(APPLE)
    add_subdirectory(platforms/mac-os)
endif()
