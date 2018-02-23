cmake_minimum_required(VERSION 3.5.1)
include(ExternalProject)

project(bow-simulator)
set(CMAKE_CXX_STANDARD 14)

# Project properties (used for configuring files)

set(PROJECT_EXE_NAME "bow-simulator")
set(PROJECT_APP_NAME "Bow Simulator")
set(PROJECT_VERSION "0.5")
set(PROJECT_WEBSITE "bow-simulator.org")
set(PROJECT_COPYRIGHT "Copyright (C) 2016-2018 Stefan Pfeifer")
set(PROJECT_LICENSE "GNU General Public License v3.0")

# External libraries

# Qt (must be provided by the system/developer)

find_package(Qt5X11Extras)    # Why is this platform specific stuff needed on Linux? It's not even linked.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# VTK (download from url and build)

ExternalProject_Add(
    vtk
    URL "https://gitlab.com/bow-simulator/vtk-custom/repository/archive.tar.gz?ref=vtkcamera-horizontal-parallel-scale"
    URL_MD5 "7636bca3cada9c6039b0bce44c41a469"
    PREFIX ${CMAKE_BINARY_DIR}/vtk_build
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/vtk_install
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_TESTING=OFF
        -DVTK_Group_Qt=ON
        -DVTK_QT_VERSION=5
)

find_package(VTK REQUIRED PATHS ${CMAKE_BINARY_DIR}/vtk_install)
include(${VTK_USE_FILE})

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

# Main executable

configure_file(source/config.hpp.in ${CMAKE_BINARY_DIR}/config.hpp)
include_directories(${PROJECT_BINARY_DIR})

include_directories(source)
add_executable(
    ${PROJECT_EXE_NAME}

    source/main.cpp
    ${CMAKE_BINARY_DIR}/config.hpp

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

    source/bow/BowModel.cpp
    source/bow/LimbProperties.cpp
    source/bow/LayerProperties.cpp
    source/bow/input/InputData.cpp

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
    source/gui/input/dialogs/OperationDialog.cpp
    source/gui/input/dialogs/OperationDialog.hpp
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

    source/gui/input/views/LimbSource.cpp
    source/gui/input/views/LimbSource.hpp
    source/gui/input/views/LimbView.cpp
    source/gui/input/views/LimbView.hpp
    source/gui/input/views/LayerLegend.cpp
    source/gui/input/views/LayerLegend.hpp
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

    resources/resources.qrc
    source/external/qcustomplot/qcustomplot.cpp
)

# Link libraries, declare dependencies on others
# https://stackoverflow.com/questions/15175318/cmake-how-to-build-external-projects-and-include-their-targets

target_link_libraries(${PROJECT_EXE_NAME} ${VTK_LIBRARIES})
target_link_libraries(${PROJECT_EXE_NAME} Qt5::Widgets Qt5::PrintSupport)
add_dependencies(${PROJECT_EXE_NAME} eigen catch json boost)

# Test executable

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

# Build release packages for the respective platform

if(UNIX)
    add_subdirectory(platforms/linux)
endif()

if(WIN32)
    add_subdirectory(platforms/windows)
endif()

if(APPLE)
    add_subdirectory(platforms/mac-os)
endif()
