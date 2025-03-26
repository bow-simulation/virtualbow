cmake_minimum_required(VERSION 3.10.2)
project(virtualbow)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCXXCompilerFlag)

# Parameters

set(APPLICATION_NAME "VirtualBow")
set(APPLICATION_VERSION "0.10.0")
set(APPLICATION_WEBSITE "https://www.virtualbow.org")
set(APPLICATION_MAINTAINER "Stefan Pfeifer")
set(APPLICATION_COPYRIGHT "Copyright (C) 2016-2025 Stefan Pfeifer")
set(APPLICATION_LICENSE "PolyForm Noncommercial License 1.0.0")
set(APPLICATION_DESCRIPTION_SHORT "Bow design tool")
set(APPLICATION_DESCRIPTION_LONG "Software tool for designing and simulating bows")

# External libraries

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets PrintSupport)
find_package(Catch2 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)                # System thread library (https://stackoverflow.com/a/39547577)
find_package(OpenGL REQUIRED)

get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
get_filename_component(QT_BINARY_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

configure_file(source/config.hpp.in ${CMAKE_BINARY_DIR}/config.hpp)
include_directories(source ${PROJECT_BINARY_DIR})

include_directories("../rust/virtualbow_ffi")
link_directories("../rust/target/release")

# Compiler flags

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno")    # Don't make standard math functions set error flags

# Target: Solver library

add_library(
    virtualbow-lib STATIC
    source/solver/API.cpp
    source/solver/BowModel.cpp
)

target_link_libraries(
    virtualbow-lib
    virtualbow_ffi
    nlohmann_json::nlohmann_json
)

# Target: Gui executable

add_executable(
    virtualbow-gui
    resources/resources.qrc
    source/pre/Main.cpp
    source/pre/HelpMenu.cpp
    source/pre/HelpMenu.hpp
    source/pre/KeyEventFilter.cpp
    source/pre/KeyEventFilter.hpp
    source/pre/MainWindow.cpp
    source/pre/MainWindow.hpp
    source/pre/RecentFilesMenu.cpp
    source/pre/RecentFilesMenu.hpp
    source/pre/SimulationDialog.cpp
    source/pre/SimulationDialog.hpp
    source/pre/UnitDialog.cpp
    source/pre/UnitDialog.hpp
    source/pre/limbview/LayerColors.cpp
    source/pre/limbview/LayerColors.hpp
    source/pre/limbview/MaterialLegend.cpp
    source/pre/limbview/MaterialLegend.hpp
    source/pre/limbview/LimbMesh.cpp
    source/pre/limbview/LimbMesh.hpp
    source/pre/limbview/LimbView.cpp
    source/pre/limbview/LimbView.hpp
    source/pre/limbview/OpenGLUtils.cpp
    source/pre/limbview/OpenGLUtils.hpp
    source/pre/viewmodel/ViewModel.cpp
    source/pre/viewmodel/ViewModel.hpp
    source/pre/viewmodel/units/Unit.cpp
    source/pre/viewmodel/units/Unit.hpp
    source/pre/viewmodel/units/Quantity.cpp
    source/pre/viewmodel/units/Quantity.hpp
    source/pre/viewmodel/units/UnitSystem.cpp
    source/pre/viewmodel/units/UnitSystem.hpp
    source/pre/treedock/TreeDock.cpp
    source/pre/treedock/TreeDock.hpp
    source/pre/treedock/TreeItem.cpp
    source/pre/treedock/TreeItem.hpp
    source/pre/treedock/items/CommentTreeItem.cpp
    source/pre/treedock/items/CommentTreeItem.hpp
    source/pre/treedock/items/SettingsTreeItem.cpp
    source/pre/treedock/items/SettingsTreeItem.hpp
    source/pre/treedock/items/DimensionsTreeItem.cpp
    source/pre/treedock/items/DimensionsTreeItem.hpp
    source/pre/treedock/items/MaterialsTreeItem.cpp
    source/pre/treedock/items/MaterialsTreeItem.hpp
    source/pre/treedock/items/StringTreeItem.cpp
    source/pre/treedock/items/StringTreeItem.hpp
    source/pre/treedock/items/MassesTreeItem.cpp
    source/pre/treedock/items/MassesTreeItem.hpp
    source/pre/treedock/items/DampingTreeItem.cpp
    source/pre/treedock/items/DampingTreeItem.hpp
    source/pre/treedock/items/WidthTreeItem.cpp
    source/pre/treedock/items/WidthTreeItem.hpp
    source/pre/treedock/items/LayersTreeItem.cpp
    source/pre/treedock/items/LayersTreeItem.hpp
    source/pre/treedock/items/ProfileTreeItem.cpp
    source/pre/treedock/items/ProfileTreeItem.hpp
    source/pre/editdock/EditDock.cpp
    source/pre/editdock/EditDock.hpp
    source/pre/editdock/editors/SegmentEditor.hpp
    source/pre/editdock/editors/LineSegmentEditor.cpp
    source/pre/editdock/editors/LineSegmentEditor.hpp
    source/pre/editdock/editors/ArcSegmentEditor.cpp
    source/pre/editdock/editors/ArcSegmentEditor.hpp
    source/pre/editdock/editors/SpiralSegmentEditor.cpp
    source/pre/editdock/editors/SpiralSegmentEditor.hpp
    source/pre/editdock/editors/SplineSegmentEditor.cpp
    source/pre/editdock/editors/SplineSegmentEditor.hpp
    source/pre/editdock/editors/PropertyValueEditor.cpp
    source/pre/editdock/editors/PropertyValueEditor.hpp
    source/pre/plotdock/PlotDock.cpp
    source/pre/plotdock/PlotDock.hpp
    source/pre/plotdock/plots/ProfileView.cpp
    source/pre/plotdock/plots/ProfileView.hpp
    source/pre/plotdock/plots/SplineView.cpp
    source/pre/plotdock/plots/SplineView.hpp
    source/pre/utils/DoubleRange.cpp
    source/pre/utils/DoubleRange.hpp
    source/pre/utils/IntegerRange.cpp
    source/pre/utils/IntegerRange.hpp
    source/pre/utils/UserSettings.cpp
    source/pre/utils/UserSettings.hpp
    source/pre/utils/ScrollArea.hpp
    source/pre/widgets/DialogBase.cpp
    source/pre/widgets/DialogBase.hpp
    source/pre/widgets/DoubleSpinBox.cpp
    source/pre/widgets/DoubleSpinBox.hpp
    source/pre/widgets/IntegerSpinBox.cpp
    source/pre/widgets/IntegerSpinBox.hpp
    source/pre/widgets/EditableTabBar.cpp
    source/pre/widgets/EditableTabBar.hpp
    source/pre/widgets/PersistentDialog.cpp
    source/pre/widgets/PersistentDialog.hpp
    source/pre/widgets/PlotOverlayDialog.cpp
    source/pre/widgets/PlotOverlayDialog.hpp
    source/pre/widgets/PlotWidget.cpp
    source/pre/widgets/PlotWidget.hpp
    source/pre/widgets/TableDelegate.cpp
    source/pre/widgets/TableDelegate.hpp
    source/pre/widgets/TableEditor.hpp
    source/pre/widgets/TableEditor.cpp
    source/pre/widgets/TableEditor.hpp
    source/pre/widgets/TableModel.cpp
    source/pre/widgets/TableModel.hpp
    source/pre/widgets/TableView.cpp
    source/pre/widgets/TableView.hpp
    source/pre/widgets/propertytree/PropertyTreeWidget.cpp
    source/pre/widgets/propertytree/PropertyTreeWidget.hpp
    source/pre/widgets/propertytree/PropertyTreeItem.cpp
    source/pre/widgets/propertytree/PropertyTreeItem.hpp
    source/pre/widgets/propertytree/PropertyDelegate.cpp
    source/pre/widgets/propertytree/PropertyDelegate.hpp
    source/pre/widgets/propertytree/items/ColorPropertyItem.cpp
    source/pre/widgets/propertytree/items/ColorPropertyItem.hpp
    source/pre/widgets/propertytree/items/DoublePropertyItem.cpp
    source/pre/widgets/propertytree/items/DoublePropertyItem.hpp
    source/pre/widgets/propertytree/items/GroupPropertyItem.cpp
    source/pre/widgets/propertytree/items/GroupPropertyItem.hpp
    source/pre/widgets/propertytree/items/IntegerPropertyItem.cpp
    source/pre/widgets/propertytree/items/IntegerPropertyItem.hpp
    source/pre/widgets/propertytree/items/StringPropertyItem.cpp
    source/pre/widgets/propertytree/items/StringPropertyItem.hpp
    source/pre/widgets/qcustomplot/qcustomplot.cpp
    source/pre/viewmodels/MainVM.cpp
    source/pre/viewmodels/MainVM.hpp
    source/pre/viewmodels/ModelTreeVM.hpp
    source/pre/viewmodels/ModelTreeVM.cpp
)

target_compile_definitions(virtualbow-gui PRIVATE JSON_HAS_CPP_17)

target_link_libraries(
    virtualbow-gui
    virtualbow-lib
    Threads::Threads
    ${OPENGL_gl_LIBRARY}
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::PrintSupport
)

# Target: Post executable

add_executable(
    virtualbow-post
    resources/resources.qrc
    source/post/Main.cpp
    source/pre/HelpMenu.cpp
    source/pre/HelpMenu.hpp
    source/pre/KeyEventFilter.cpp
    source/pre/KeyEventFilter.hpp
    source/pre/RecentFilesMenu.cpp
    source/pre/RecentFilesMenu.hpp
    source/pre/UnitDialog.cpp
    source/pre/UnitDialog.hpp
    source/pre/utils/UserSettings.cpp
    source/pre/utils/UserSettings.hpp
    source/pre/limbview/LayerColors.cpp
    source/pre/limbview/LayerColors.hpp
    source/pre/viewmodel/units/Unit.cpp
    source/pre/viewmodel/units/Unit.hpp
    source/pre/viewmodel/units/Quantity.cpp
    source/pre/viewmodel/units/Quantity.hpp
    source/pre/viewmodel/units/UnitSystem.cpp
    source/pre/viewmodel/units/UnitSystem.hpp
    source/pre/utils/DoubleRange.cpp
    source/pre/utils/DoubleRange.hpp
    source/pre/utils/IntegerRange.cpp
    source/pre/utils/IntegerRange.hpp
    source/pre/utils/UserSettings.cpp
    source/pre/utils/UserSettings.hpp
    source/pre/utils/ScrollArea.hpp
    source/pre/widgets/DialogBase.cpp
    source/pre/widgets/DialogBase.hpp
    source/pre/widgets/DoubleSpinBox.cpp
    source/pre/widgets/DoubleSpinBox.hpp
    source/pre/widgets/IntegerSpinBox.cpp
    source/pre/widgets/IntegerSpinBox.hpp
    source/pre/widgets/PlotOverlayDialog.cpp
    source/pre/widgets/PlotOverlayDialog.hpp
    source/pre/widgets/PlotWidget.cpp
    source/pre/widgets/PlotWidget.hpp
    source/pre/widgets/qcustomplot/qcustomplot.cpp
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

target_compile_definitions(virtualbow-post PRIVATE JSON_HAS_CPP_17)

target_link_libraries(
    virtualbow-post
    virtualbow-lib
    Qt6::Widgets
    Qt6::PrintSupport
)

# Target: Test executable

add_executable(
    virtualbow-test
    source/tests/main.cpp
)

target_link_libraries(
    virtualbow-test
    virtualbow-lib
    Catch2::Catch2
)

# Change output directories

set_target_properties(
    virtualbow-gui
    virtualbow-post
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/application
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/application
)

# Target: Solver executable (Rust)
# Invokes Cargo to build the release version of the solver and copies the executable into the application directory
# https://stackoverflow.com/a/50027621/10489834

add_custom_target(virtualbow-cli)
add_custom_command(
    TARGET virtualbow-cli
    COMMAND cargo build --release --manifest-path=${CMAKE_CURRENT_SOURCE_DIR}/../rust/Cargo.toml
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../rust/target/release/virtualbow_cli ${CMAKE_BINARY_DIR}/application/virtualbow-cli ||
            ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../rust/target/release/virtualbow_cli.exe ${CMAKE_BINARY_DIR}/application/virtualbow-cli.exe
)

# Platform specifics

if(WIN32)
    add_subdirectory(platforms/windows)
elseif(APPLE)
    add_subdirectory(platforms/macos)
elseif(UNIX)
    add_subdirectory(platforms/linux)
endif()
