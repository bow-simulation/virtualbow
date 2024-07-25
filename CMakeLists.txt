cmake_minimum_required(VERSION 3.10.2)
project(virtualbow)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCXXCompilerFlag)

# Parameters

set(APPLICATION_NAME "VirtualBow")
set(APPLICATION_VERSION "0.9.1")
set(APPLICATION_WEBSITE "https://www.virtualbow.org")
set(APPLICATION_MAINTAINER "Stefan Pfeifer")
set(APPLICATION_COPYRIGHT "Copyright (C) 2016-2022 Stefan Pfeifer")
set(APPLICATION_LICENSE "GNU General Public License v3.0")
set(APPLICATION_DESCRIPTION_SHORT "Bow design tool")
set(APPLICATION_DESCRIPTION_LONG "Software tool for designing and simulating bows")

# External libraries

find_package(Qt5 5.9.5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Boost 1.79.0 REQUIRED)
find_package(Catch2 2.13.9 REQUIRED)
find_package(Eigen3 3.4.0 REQUIRED)
find_package(nlohmann_json 3.10.5 REQUIRED)
find_package(Threads REQUIRED)                # System thread library (https://stackoverflow.com/a/39547577)
find_package(OpenGL REQUIRED)

get_target_property(QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
get_filename_component(QT_BINARY_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

configure_file(source/config.hpp.in ${CMAKE_BINARY_DIR}/config.hpp)
include_directories(source ${PROJECT_BINARY_DIR})
include_directories(${Boost_INCLUDE_DIRS})

# Compiler flags

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno")    # Don't make standard math functions set error flags

# Target: Solver library

add_library(
    virtualbow-lib STATIC
    source/solver/model/ContinuousLimb.cpp
    source/solver/model/LimbProperties.cpp
    source/solver/model/input/InputData.cpp
    source/solver/model/input/Conversion.cpp
    source/solver/model/output/OutputData.cpp
    source/solver/model/profile/ProfileCurve.cpp
    source/solver/model/profile/ProfileSegment.cpp
    source/solver/model/profile/ProfileInput.cpp
    source/solver/model/profile/segments/ClothoidSegment.cpp
    source/solver/model/profile/segments/SplineSegment.cpp
    source/solver/numerics/CubicSpline.cpp
    source/solver/numerics/Fresnel.cpp
    source/solver/numerics/Sorting.cpp

)

target_link_libraries(
    virtualbow-lib
    ${Boost_LIBRARIES}
    Eigen3::Eigen
    nlohmann_json::nlohmann_json
)

# Target: Gui executable

add_executable(
    virtualbow-gui
    resources/resources.qrc
    source/gui/Main.cpp
    source/gui/HelpMenu.cpp
    source/gui/HelpMenu.hpp
    source/gui/KeyEventFilter.cpp
    source/gui/KeyEventFilter.hpp
    source/gui/MainWindow.cpp
    source/gui/MainWindow.hpp
    source/gui/RecentFilesMenu.cpp
    source/gui/RecentFilesMenu.hpp
    source/gui/SimulationDialog.cpp
    source/gui/SimulationDialog.hpp
    source/gui/UnitDialog.cpp
    source/gui/UnitDialog.hpp
    source/gui/limbview/LayerColors.cpp
    source/gui/limbview/LayerColors.hpp
    source/gui/limbview/MaterialLegend.cpp
    source/gui/limbview/MaterialLegend.hpp
    source/gui/limbview/LimbMesh.cpp
    source/gui/limbview/LimbMesh.hpp
    source/gui/limbview/LimbView.cpp
    source/gui/limbview/LimbView.hpp
    source/gui/limbview/OpenGLUtils.cpp
    source/gui/limbview/OpenGLUtils.hpp
    source/gui/viewmodel/ViewModel.cpp
    source/gui/viewmodel/ViewModel.hpp
    source/gui/viewmodel/units/Unit.cpp
    source/gui/viewmodel/units/Unit.hpp
    source/gui/viewmodel/units/Quantity.cpp
    source/gui/viewmodel/units/Quantity.hpp
    source/gui/viewmodel/units/UnitSystem.cpp
    source/gui/viewmodel/units/UnitSystem.hpp
    source/gui/treedock/TreeDock.cpp
    source/gui/treedock/TreeDock.hpp
    source/gui/treedock/TreeItem.cpp
    source/gui/treedock/TreeItem.hpp
    source/gui/treedock/items/CommentTreeItem.cpp
    source/gui/treedock/items/CommentTreeItem.hpp
    source/gui/treedock/items/SettingsTreeItem.cpp
    source/gui/treedock/items/SettingsTreeItem.hpp
    source/gui/treedock/items/DimensionsTreeItem.cpp
    source/gui/treedock/items/DimensionsTreeItem.hpp
    source/gui/treedock/items/MaterialsTreeItem.cpp
    source/gui/treedock/items/MaterialsTreeItem.hpp
    source/gui/treedock/items/StringTreeItem.cpp
    source/gui/treedock/items/StringTreeItem.hpp
    source/gui/treedock/items/MassesTreeItem.cpp
    source/gui/treedock/items/MassesTreeItem.hpp
    source/gui/treedock/items/DampingTreeItem.cpp
    source/gui/treedock/items/DampingTreeItem.hpp
    source/gui/treedock/items/WidthTreeItem.cpp
    source/gui/treedock/items/WidthTreeItem.hpp
    source/gui/treedock/items/LayersTreeItem.cpp
    source/gui/treedock/items/LayersTreeItem.hpp
    source/gui/treedock/items/ProfileTreeItem.cpp
    source/gui/treedock/items/ProfileTreeItem.hpp
    source/gui/editdock/EditDock.cpp
    source/gui/editdock/EditDock.hpp
    source/gui/editdock/editors/SegmentEditor.hpp
    source/gui/editdock/editors/LineSegmentEditor.cpp
    source/gui/editdock/editors/LineSegmentEditor.hpp
    source/gui/editdock/editors/ArcSegmentEditor.cpp
    source/gui/editdock/editors/ArcSegmentEditor.hpp
    source/gui/editdock/editors/SpiralSegmentEditor.cpp
    source/gui/editdock/editors/SpiralSegmentEditor.hpp
    source/gui/editdock/editors/SplineSegmentEditor.cpp
    source/gui/editdock/editors/SplineSegmentEditor.hpp
    source/gui/editdock/editors/PropertyValueEditor.cpp
    source/gui/editdock/editors/PropertyValueEditor.hpp
    source/gui/plotdock/PlotDock.cpp
    source/gui/plotdock/PlotDock.hpp
    source/gui/plotdock/plots/ProfileView.cpp
    source/gui/plotdock/plots/ProfileView.hpp
    source/gui/plotdock/plots/SplineView.cpp
    source/gui/plotdock/plots/SplineView.hpp
    source/gui/utils/DoubleRange.cpp
    source/gui/utils/DoubleRange.hpp
    source/gui/utils/IntegerRange.cpp
    source/gui/utils/IntegerRange.hpp
    source/gui/utils/UserSettings.cpp
    source/gui/utils/UserSettings.hpp
    source/gui/utils/ScrollArea.hpp
    source/gui/widgets/DialogBase.cpp
    source/gui/widgets/DialogBase.hpp
    source/gui/widgets/DoubleSpinBox.cpp
    source/gui/widgets/DoubleSpinBox.hpp
    source/gui/widgets/IntegerSpinBox.cpp
    source/gui/widgets/IntegerSpinBox.hpp
    source/gui/widgets/EditableTabBar.cpp
    source/gui/widgets/EditableTabBar.hpp
    source/gui/widgets/PersistentDialog.cpp
    source/gui/widgets/PersistentDialog.hpp
    source/gui/widgets/PlotOverlayDialog.cpp
    source/gui/widgets/PlotOverlayDialog.hpp
    source/gui/widgets/PlotWidget.cpp
    source/gui/widgets/PlotWidget.hpp
    source/gui/widgets/TableDelegate.cpp
    source/gui/widgets/TableDelegate.hpp
    source/gui/widgets/TableEditor.hpp
    source/gui/widgets/TableEditor.cpp
    source/gui/widgets/TableEditor.hpp
    source/gui/widgets/TableModel.cpp
    source/gui/widgets/TableModel.hpp
    source/gui/widgets/TableView.cpp
    source/gui/widgets/TableView.hpp
    source/gui/widgets/propertytree/PropertyTreeWidget.cpp
    source/gui/widgets/propertytree/PropertyTreeWidget.hpp
    source/gui/widgets/propertytree/PropertyTreeItem.cpp
    source/gui/widgets/propertytree/PropertyTreeItem.hpp
    source/gui/widgets/propertytree/PropertyDelegate.cpp
    source/gui/widgets/propertytree/PropertyDelegate.hpp
    source/gui/widgets/propertytree/items/ColorPropertyItem.cpp
    source/gui/widgets/propertytree/items/ColorPropertyItem.hpp
    source/gui/widgets/propertytree/items/DoublePropertyItem.cpp
    source/gui/widgets/propertytree/items/DoublePropertyItem.hpp
    source/gui/widgets/propertytree/items/GroupPropertyItem.cpp
    source/gui/widgets/propertytree/items/GroupPropertyItem.hpp
    source/gui/widgets/propertytree/items/IntegerPropertyItem.cpp
    source/gui/widgets/propertytree/items/IntegerPropertyItem.hpp
    source/gui/widgets/propertytree/items/StringPropertyItem.cpp
    source/gui/widgets/propertytree/items/StringPropertyItem.hpp
    source/gui/widgets/qcustomplot/qcustomplot.cpp
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
    source/post/Main.cpp
    source/gui/HelpMenu.cpp
    source/gui/HelpMenu.hpp
    source/gui/KeyEventFilter.cpp
    source/gui/KeyEventFilter.hpp
    source/gui/RecentFilesMenu.cpp
    source/gui/RecentFilesMenu.hpp
    source/gui/UnitDialog.cpp
    source/gui/UnitDialog.hpp
    source/gui/utils/UserSettings.cpp
    source/gui/utils/UserSettings.hpp
    source/gui/limbview/LayerColors.cpp
    source/gui/limbview/LayerColors.hpp
    source/gui/viewmodel/units/Unit.cpp
    source/gui/viewmodel/units/Unit.hpp
    source/gui/viewmodel/units/Quantity.cpp
    source/gui/viewmodel/units/Quantity.hpp
    source/gui/viewmodel/units/UnitSystem.cpp
    source/gui/viewmodel/units/UnitSystem.hpp
    source/gui/utils/DoubleRange.cpp
    source/gui/utils/DoubleRange.hpp
    source/gui/utils/IntegerRange.cpp
    source/gui/utils/IntegerRange.hpp
    source/gui/utils/UserSettings.cpp
    source/gui/utils/UserSettings.hpp
    source/gui/utils/ScrollArea.hpp
    source/gui/widgets/DialogBase.cpp
    source/gui/widgets/DialogBase.hpp
    source/gui/widgets/DoubleSpinBox.cpp
    source/gui/widgets/DoubleSpinBox.hpp
    source/gui/widgets/IntegerSpinBox.cpp
    source/gui/widgets/IntegerSpinBox.hpp
    source/gui/widgets/PlotOverlayDialog.cpp
    source/gui/widgets/PlotOverlayDialog.hpp
    source/gui/widgets/PlotWidget.cpp
    source/gui/widgets/PlotWidget.hpp
    source/gui/widgets/qcustomplot/qcustomplot.cpp
    source/post/ComboPlot.cpp
    source/post/ComboPlot.hpp
    source/post/CurvaturePlot.cpp
    source/post/CurvaturePlot.hpp
    source/post/EnergyPlot.cpp
    source/post/EnergyPlot.hpp
    source/post/MainWindow.cpp
    source/post/MainWindow.hpp
    source/post/NumberGrid.cpp
    source/post/NumberGrid.hpp
    source/post/OutputWidget.cpp
    source/post/OutputWidget.hpp
    source/post/ShapePlot.cpp
    source/post/ShapePlot.hpp
    source/post/Slider.cpp
    source/post/Slider.hpp
    source/post/StressPlot.cpp
    source/post/StressPlot.hpp
)

target_link_libraries(
    virtualbow-post
    virtualbow-lib
    Qt5::Widgets
    Qt5::PrintSupport
)

# Change output directories

set_target_properties(
    virtualbow-gui
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
