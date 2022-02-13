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
    source/solver/model/input/InputData.cpp
    source/solver/model/input/Conversion.cpp
    source/solver/model/output/OutputData.cpp
    source/solver/model/BowModel.cpp
    source/solver/model/profile/ProfileCurve.cpp
    source/solver/model/profile/segments/ClothoidSegment.cpp
    source/solver/model/profile/segments/SplineSegment.cpp
    source/solver/numerics/CubicSpline.cpp
    source/solver/numerics/Fresnel.cpp
    source/solver/numerics/Sorting.cpp
    source/solver/numerics/Geometry.cpp
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
    source/solver/Main.cpp
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

    source/gui/viewmodel/MainViewModel.cpp
    source/gui/viewmodel/MainViewModel.hpp
    source/gui/viewmodel/DataViewModel.cpp
    source/gui/viewmodel/DataViewModel.hpp

    source/gui/treeview/TreeView.cpp
    source/gui/treeview/TreeView.hpp
    source/gui/treeview/TreeItem.cpp
    source/gui/treeview/TreeItem.hpp
    source/gui/treeview/items/CommentTreeItem.cpp
    source/gui/treeview/items/CommentTreeItem.hpp
    source/gui/treeview/items/SettingsTreeItem.cpp
    source/gui/treeview/items/SettingsTreeItem.hpp

    source/gui/propertyview/PropertyView.cpp
    source/gui/propertyview/PropertyView.hpp

    source/gui/plotview/PlotView.cpp
    source/gui/plotview/PlotView.hpp

    source/gui/limbview/LayerColors.cpp
    source/gui/limbview/LayerColors.hpp
    source/gui/limbview/LayerLegend.cpp
    source/gui/limbview/LayerLegend.hpp
    source/gui/limbview/LimbMesh.cpp
    source/gui/limbview/LimbMesh.hpp
    source/gui/limbview/LimbView.cpp
    source/gui/limbview/LimbView.hpp
    source/gui/limbview/OpenGLUtils.cpp
    source/gui/limbview/OpenGLUtils.hpp
    source/gui/modeltree/GroupDialog.cpp
    source/gui/modeltree/GroupDialog.hpp
    source/gui/modeltree/ModelTreeEditor.cpp
    source/gui/modeltree/ModelTreeEditor.hpp
    source/gui/modeltree/ModelTreeItem.hpp
    source/gui/modeltree/comment/CommentDialog.cpp
    source/gui/modeltree/comment/CommentDialog.hpp
    source/gui/modeltree/damping/DampingDialog.cpp
    source/gui/modeltree/damping/DampingDialog.hpp
    source/gui/modeltree/dimensions/DimensionsDialog.cpp
    source/gui/modeltree/dimensions/DimensionsDialog.hpp
    source/gui/modeltree/layers/LayerDialog.cpp
    source/gui/modeltree/layers/LayerDialog.hpp
    source/gui/modeltree/layers/LayerEditor.cpp
    source/gui/modeltree/layers/LayerEditor.hpp
    source/gui/modeltree/masses/MassesDialog.cpp
    source/gui/modeltree/masses/MassesDialog.hpp
    source/gui/modeltree/profile/ProfileDialog.cpp
    source/gui/modeltree/profile/ProfileDialog.hpp
    source/gui/modeltree/profile/ProfileEditor.cpp
    source/gui/modeltree/profile/ProfileEditor.hpp
    source/gui/modeltree/profile/ProfileTreeView.cpp
    source/gui/modeltree/profile/ProfileTreeView.hpp
    source/gui/modeltree/profile/ProfileSegmentView.cpp
    source/gui/modeltree/profile/ProfileSegmentView.hpp
    source/gui/modeltree/profile/ProfileTreeModel.cpp
    source/gui/modeltree/profile/ProfileTreeModel.hpp
    source/gui/modeltree/profile/ProfileView.cpp
    source/gui/modeltree/profile/ProfileView.hpp
    source/gui/modeltree/profile/SegmentEditor.hpp
    source/gui/modeltree/profile/segments/PropertyValueEditor.cpp
    source/gui/modeltree/profile/segments/PropertyValueEditor.hpp
    source/gui/modeltree/profile/segments/LineSegmentEditor.cpp
    source/gui/modeltree/profile/segments/LineSegmentEditor.hpp
    source/gui/modeltree/profile/segments/ArcSegmentEditor.cpp
    source/gui/modeltree/profile/segments/ArcSegmentEditor.hpp
    source/gui/modeltree/profile/segments/SpiralSegmentEditor.cpp
    source/gui/modeltree/profile/segments/SpiralSegmentEditor.hpp
    source/gui/modeltree/profile/segments/SplineSegmentEditor.cpp
    source/gui/modeltree/profile/segments/SplineSegmentEditor.hpp
    source/gui/modeltree/settings/SettingsDialog.cpp
    source/gui/modeltree/settings/SettingsDialog.hpp
    source/gui/modeltree/string/StringDialog.cpp
    source/gui/modeltree/string/StringDialog.hpp
    source/gui/modeltree/width/WidthDialog.cpp
    source/gui/modeltree/width/WidthDialog.hpp
    source/gui/units/Unit.cpp
    source/gui/units/Unit.hpp
    source/gui/units/UnitDialog.cpp
    source/gui/units/UnitDialog.hpp
    source/gui/units/UnitGroup.cpp
    source/gui/units/UnitGroup.hpp
    source/gui/units/UnitSystem.cpp
    source/gui/units/UnitSystem.hpp
    source/gui/utils/DoubleRange.cpp
    source/gui/utils/DoubleRange.hpp
    source/gui/widgets/DialogBase.cpp
    source/gui/widgets/DialogBase.hpp
    source/gui/widgets/DoubleEditor.cpp
    source/gui/widgets/DoubleEditor.hpp
    source/gui/widgets/DoubleSpinBox.cpp
    source/gui/widgets/DoubleSpinBox.hpp
    source/gui/widgets/EditableTabBar.cpp
    source/gui/widgets/EditableTabBar.hpp
    source/gui/widgets/IntegerEditor.cpp
    source/gui/widgets/IntegerEditor.hpp
    source/gui/widgets/PersistentDialog.cpp
    source/gui/widgets/PersistentDialog.hpp
    source/gui/widgets/PlotOverlayDialog.cpp
    source/gui/widgets/PlotOverlayDialog.hpp
    source/gui/widgets/PlotWidget.cpp
    source/gui/widgets/PlotWidget.hpp
    source/gui/widgets/SplineView.cpp
    source/gui/widgets/SplineView.hpp
    source/gui/widgets/TableDelegate.cpp
    source/gui/widgets/TableDelegate.hpp
    source/gui/widgets/TableEditor.hpp
    source/gui/widgets/TableEditor.cpp
    source/gui/widgets/TableEditor.hpp
    source/gui/widgets/TableModel.cpp
    source/gui/widgets/TableModel.hpp
    source/gui/widgets/TableSpinner.cpp
    source/gui/widgets/TableSpinner.hpp
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
    source/gui/limbview/LayerColors.cpp
    source/gui/limbview/LayerColors.hpp
    source/gui/units/Unit.cpp
    source/gui/units/Unit.hpp
    source/gui/units/UnitDialog.cpp
    source/gui/units/UnitDialog.hpp
    source/gui/units/UnitGroup.cpp
    source/gui/units/UnitGroup.hpp
    source/gui/units/UnitSystem.cpp
    source/gui/units/UnitSystem.hpp
    source/gui/widgets/DialogBase.cpp
    source/gui/widgets/DialogBase.hpp
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

# Target: Test executable

add_executable(
    virtualbow-test
    source/tests/Main.cpp
    source/tests/fem/BarTrusses.cpp
    source/tests/fem/HarmonicOscillator.cpp
    source/tests/fem/LargeDeformationBeams.cpp
    source/tests/fem/TangentStiffness.cpp
    source/tests/model/BeamStiffnessMatrix.cpp
    source/tests/numerics/FindInterval.cpp
    source/tests/numerics/Geometry.cpp
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
