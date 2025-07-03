cmake_minimum_required(VERSION 3.13)
project(virtualbow)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets PrintSupport Test)
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

# Target: Solver library

add_library(
    virtualbow-lib STATIC
    source/solver/API.cpp
    source/solver/BowModel.cpp
    source/solver/Defaults.cpp
)

target_link_libraries(virtualbow-lib virtualbow_ffi nlohmann_json::nlohmann_json)
target_link_libraries(virtualbow-lib $<$<PLATFORM_ID:Windows>:ws2_32.lib;Userenv.lib;ntdll.lib;>)  # Add missing Windows libraries

# Target: Gui executable

add_executable(
    virtualbow-gui
    resources/resources.qrc
    source/extern/qcustomplot/qcustomplot.cpp
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
    source/pre/utils/Expressions.cpp
    source/pre/utils/Expressions.hpp
    source/pre/utils/DoubleRange.cpp
    source/pre/utils/DoubleRange.hpp
    source/pre/utils/IntegerRange.cpp
    source/pre/utils/IntegerRange.hpp
    source/pre/utils/UserSettings.cpp
    source/pre/utils/UserSettings.hpp
    source/pre/widgets/ScrollArea.hpp
    source/pre/widgets/DialogBase.cpp
    source/pre/widgets/DialogBase.hpp
    source/pre/widgets/DoubleSpinBox.cpp
    source/pre/widgets/DoubleSpinBox.hpp
    source/pre/widgets/IntegerSpinBox.cpp
    source/pre/widgets/IntegerSpinBox.hpp
    source/pre/widgets/PersistentDialog.cpp
    source/pre/widgets/PersistentDialog.hpp
    source/pre/widgets/PlotOverlayDialog.cpp
    source/pre/widgets/PlotOverlayDialog.hpp
    source/pre/widgets/PlotWidget.cpp
    source/pre/widgets/PlotWidget.hpp
    source/pre/widgets/PropertyList.cpp
    source/pre/widgets/PropertyList.hpp
    source/pre/models/MainModel.cpp
    source/pre/models/MainModel.hpp
    source/pre/models/MainTreeModel.hpp
    source/pre/models/MainTreeModel.cpp
    source/pre/models/PropertyListModel.hpp
    source/pre/models/PropertyListModel.cpp
    source/pre/models/CommentsModel.hpp
    source/pre/models/CommentsModel.cpp
    source/pre/models/SettingsModel.hpp
    source/pre/models/SettingsModel.cpp
    source/pre/models/DimensionsModel.hpp
    source/pre/models/DimensionsModel.cpp
    source/pre/models/MaterialModel.hpp
    source/pre/models/MaterialModel.cpp
    source/pre/models/LayersModel.hpp
    source/pre/models/LayersModel.cpp
    source/pre/models/LayerModel.hpp
    source/pre/models/LayerModel.cpp
    source/pre/models/StringModel.hpp
    source/pre/models/StringModel.cpp
    source/pre/models/MassesModel.hpp
    source/pre/models/MassesModel.cpp
    source/pre/models/DampingModel.hpp
    source/pre/models/DampingModel.cpp
    source/pre/models/LineModel.hpp
    source/pre/models/LineModel.cpp
    source/pre/models/ArcModel.hpp
    source/pre/models/ArcModel.cpp
    source/pre/models/SpiralModel.hpp
    source/pre/models/SpiralModel.cpp
    source/pre/models/TableModel.cpp
    source/pre/models/TableModel.hpp
    source/pre/models/units/Unit.cpp
    source/pre/models/units/Unit.hpp
    source/pre/models/units/Quantity.cpp
    source/pre/models/units/Quantity.hpp
    source/pre/models/units/UnitSystem.cpp
    source/pre/models/units/UnitSystem.hpp
    source/pre/views/docks/TreeDock.cpp
    source/pre/views/docks/TreeDock.hpp
    source/pre/views/docks/EditDock.cpp
    source/pre/views/docks/EditDock.hpp
    source/pre/views/docks/PlotDock.cpp
    source/pre/views/docks/PlotDock.hpp
    source/pre/views/limb2d/ProfileView.cpp
    source/pre/views/limb2d/ProfileView.hpp
    source/pre/views/limb2d/SplineView.cpp
    source/pre/views/limb2d/SplineView.hpp
    source/pre/views/primitive/DoubleView.hpp
    source/pre/views/primitive/DoubleView.cpp
    source/pre/views/primitive/ColorView.hpp
    source/pre/views/primitive/ColorView.cpp
    source/pre/views/primitive/EnumSelectionView.hpp
    source/pre/views/primitive/EnumSelectionView.cpp
    source/pre/views/primitive/StringSelectionView.hpp
    source/pre/views/primitive/StringSelectionView.cpp
    source/pre/views/primitive/IntegerView.hpp
    source/pre/views/primitive/IntegerView.cpp
    source/pre/views/primitive/TextView.hpp
    source/pre/views/primitive/TextView.cpp
    source/pre/views/primitive/TableView.hpp
    source/pre/views/primitive/TableView.cpp
    source/pre/views/CommentsView.hpp
    source/pre/views/CommentsView.cpp
    source/pre/views/SettingsView.hpp
    source/pre/views/SettingsView.cpp
    source/pre/views/DimensionsView.hpp
    source/pre/views/DimensionsView.cpp
    source/pre/views/MaterialView.hpp
    source/pre/views/MaterialView.cpp
    source/pre/views/LayerView.hpp
    source/pre/views/LayerView.cpp
    source/pre/views/WidthView.hpp
    source/pre/views/WidthView.cpp
    source/pre/views/LineView.hpp
    source/pre/views/LineView.cpp
    source/pre/views/ArcView.hpp
    source/pre/views/ArcView.cpp
    source/pre/views/SpiralView.hpp
    source/pre/views/SpiralView.cpp
    source/pre/views/SplineView.hpp
    source/pre/views/SplineView.cpp
    source/pre/views/StringView.hpp
    source/pre/views/StringView.cpp
    source/pre/views/MassesView.hpp
    source/pre/views/MassesView.cpp
    source/pre/views/DampingView.hpp
    source/pre/views/DampingView.cpp
    source/pre/views/limb3d/LayerColors.cpp
    source/pre/views/limb3d/LayerColors.hpp
    source/pre/views/limb3d/MaterialLegend.cpp
    source/pre/views/limb3d/MaterialLegend.hpp
    source/pre/views/limb3d/LimbMesh.cpp
    source/pre/views/limb3d/LimbMesh.hpp
    source/pre/views/limb3d/LimbView.cpp
    source/pre/views/limb3d/LimbView.hpp
    source/pre/views/limb3d/OpenGLUtils.cpp
    source/pre/views/limb3d/OpenGLUtils.hpp
)

target_compile_definitions(virtualbow-gui PRIVATE JSON_HAS_CPP_17)

target_link_libraries(virtualbow-gui virtualbow-lib
    Threads::Threads
    ${OPENGL_gl_LIBRARY}
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::PrintSupport
    Qt6::Test
)

# Target: Post executable

add_executable(
    virtualbow-post
    resources/resources.qrc
    source/extern/qcustomplot/qcustomplot.cpp
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
    source/pre/views/limb3d/LayerColors.cpp
    source/pre/views/limb3d/LayerColors.hpp
    source/pre/models/units/Unit.cpp
    source/pre/models/units/Unit.hpp
    source/pre/models/units/Quantity.cpp
    source/pre/models/units/Quantity.hpp
    source/pre/models/units/UnitSystem.cpp
    source/pre/models/units/UnitSystem.hpp
    source/pre/utils/Expressions.cpp
    source/pre/utils/Expressions.hpp
    source/pre/utils/DoubleRange.cpp
    source/pre/utils/DoubleRange.hpp
    source/pre/utils/IntegerRange.cpp
    source/pre/utils/IntegerRange.hpp
    source/pre/utils/UserSettings.cpp
    source/pre/utils/UserSettings.hpp
    source/pre/widgets/ScrollArea.hpp
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

# Platform-specific build targets

if(WIN32)
    add_subdirectory(platforms/windows)
elseif(APPLE)
    add_subdirectory(platforms/macos)
elseif(UNIX)
    add_subdirectory(platforms/linux)
endif()
