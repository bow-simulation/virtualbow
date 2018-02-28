cmake_minimum_required(VERSION 3.5.1)
include(ExternalProject)
project(bow-simulator)
set(CMAKE_CXX_STANDARD 14)

# Parameters used for configuring files

set(APPLICATION_NAME "bow-simulator")
set(APPLICATION_TITLE "Bow Simulator")
set(APPLICATION_VERSION "0.5")
set(APPLICATION_WEBSITE "bow-simulator.org")
set(APPLICATION_MAINTAINER "Stefan Pfeifer")
set(APPLICATION_COPYRIGHT "Copyright (C) 2016-2018 Stefan Pfeifer")
set(APPLICATION_LICENSE "GNU General Public License v3.0")
set(APPLICATION_DESCRIPTION_SHORT "Bow and arrow physics simulation")
set(APPLICATION_DESCRIPTION_LONG "Software tool for simulating the static and dynamic performance of user-defined bow designs")

# External libraries

# Qt (must be provided by the system/developer)

find_package(Qt5 COMPONENTS X11Extras Widgets PrintSupport)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# VTK (download from url and build)

ExternalProject_Add(
    vtk
    URL "https://gitlab.com/bow-simulator/vtk-custom/repository/archive.tar.gz?ref=vtkcamera-horizontal-parallel-scale"
    URL_MD5 "7636bca3cada9c6039b0bce44c41a469"
    PREFIX ${CMAKE_BINARY_DIR}/vtk_download
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/vtk_install
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_TESTING=OFF
        -DVTK_Group_Qt=ON
        -DVTK_QT_VERSION=5
        -DQt5_DIR=${Qt5_DIR}
)

# https://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage
find_package(VTK PATHS ${CMAKE_BINARY_DIR}/vtk_install)
if(${VTK_FOUND})
    add_custom_target(rescan)
    include(${VTK_USE_FILE})
else()
    add_custom_target(rescan ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} DEPENDS vtk)
endif()

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
    bow-simulator

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


# Declare dependencies, link libraries

add_dependencies(bow-simulator rescan eigen catch json boost)
target_link_libraries(bow-simulator ${VTK_LIBRARIES})
target_link_libraries(bow-simulator Qt5::Widgets Qt5::PrintSupport)


# Link VTK libraries
#target_link_libraries(
#    bow-simulator
#    vtksys-9.0 vtkCommonCore-9.0 vtkCommonMath-9.0 vtkCommonMisc-9.0 vtkCommonSystem-9.0 vtkCommonTransforms-9.0 vtkCommonDataModel-9.0 vtkCommonColor-9.0 vtkCommonExecutionModel-9.0 vtkCommonComputationalGeometry-9.0 vtkFiltersCore-9.0 vtkFiltersGeneral-9.0 vtkImagingCore-9.0 vtkImagingFourier-9.0 vtkFiltersStatistics-9.0 vtkFiltersExtraction-9.0 vtkInfovisCore-9.0 vtkFiltersGeometry-9.0 vtkFiltersSources-9.0 vtkRenderingCore-9.0 vtkzlib-9.0 vtkfreetype-9.0 vtkRenderingFreeType-9.0 vtkRenderingContext2D-9.0 vtkChartsCore-9.0 vtkDICOMParser-9.0 vtklz4-9.0 vtklzma-9.0 vtkIOCore-9.0 vtkIOLegacy-9.0 vtkexpat-9.0 vtkIOXMLParser-9.0 vtkDomainsChemistry-9.0 vtkglew-9.0 vtkRenderingOpenGL2-9.0 vtkDomainsChemistryOpenGL2-9.0 vtkIOXML-9.0 vtkParallelCore-9.0 vtkFiltersAMR-9.0 vtkFiltersFlowPaths-9.0 vtkFiltersGeneric-9.0 vtkImagingSources-9.0 vtkFiltersHybrid-9.0 vtkFiltersHyperTree-9.0 vtkImagingGeneral-9.0 vtkFiltersImaging-9.0 vtkFiltersModeling-9.0 vtkFiltersParallel-9.0 vtkFiltersParallelImaging-9.0 vtkFiltersPoints-9.0 vtkFiltersProgrammable-9.0 vtkFiltersSMP-9.0 vtkFiltersSelection-9.0 vtkFiltersTexture-9.0 vtkFiltersTopology-9.0 vtkFiltersVerdict-9.0 vtkInteractionStyle-9.0 vtkGUISupportQt-9.0 vtksqlite-9.0 vtkIOSQL-9.0 vtkGUISupportQtSQL-9.0 vtkmetaio-9.0 vtkjpeg-9.0 vtkpng-9.0 vtktiff-9.0 vtkIOImage-9.0 vtkImagingHybrid-9.0 vtkInfovisLayout-9.0 vtkImagingColor-9.0 vtkRenderingAnnotation-9.0 vtkRenderingVolume-9.0 vtkInteractionWidgets-9.0 vtkViewsCore-9.0 vtkproj4-9.0 vtkGeovisCore-9.0 vtkhdf5_src-9.0 vtkhdf5_hl_src-9.0 vtkIOAMR-9.0 vtkIOEnSight-9.0 vtknetcdf-9.0 vtkexodusII-9.0 vtkIOExodus-9.0 vtkgl2ps-9.0 vtkRenderingGL2PSOpenGL2-9.0 vtklibharu-9.0 vtkIOExport-9.0 vtkIOExportOpenGL2-9.0 vtkIOGeometry-9.0 vtkIOImport-9.0 vtklibxml2-9.0 vtkIOInfovis-9.0 vtkIOLSDyna-9.0 vtkIOMINC-9.0 vtkogg-9.0 vtktheora-9.0 vtkIOMovie-9.0 vtknetcdfcpp-9.0 vtkIONetCDF-9.0 vtkIOPLY-9.0 vtkjsoncpp-9.0 vtkIOParallel-9.0 vtkIOParallelXML-9.0 vtkIOTecplotTable-9.0 vtkIOVideo-9.0 vtkImagingMath-9.0 vtkImagingMorphological-9.0 vtkImagingStatistics-9.0 vtkImagingStencil-9.0 vtkInteractionImage-9.0 vtkRenderingContextOpenGL2-9.0 vtkRenderingImage-9.0 vtkRenderingLOD-9.0 vtkRenderingLabel-9.0 vtkRenderingQt-9.0 vtkRenderingVolumeOpenGL2-9.0 vtkViewsContext2D-9.0 vtkViewsInfovis-9.0 vtkViewsQt-9.0
#)

# vtksys vtkCommonCore vtkCommonMath vtkCommonMisc vtkCommonSystem vtkCommonTransforms vtkCommonDataModel vtkCommonColor vtkCommonExecutionModel vtkCommonComputationalGeometry vtkFiltersCore vtkFiltersGeneral vtkImagingCore vtkImagingFourier vtkFiltersStatistics vtkFiltersExtraction vtkInfovisCore vtkFiltersGeometry vtkFiltersSources vtkRenderingCore vtkzlib vtkfreetype vtkRenderingFreeType vtkRenderingContext2D vtkChartsCore vtkDICOMParser vtklz4 vtklzma vtkIOCore vtkIOLegacy vtkexpat vtkIOXMLParser vtkDomainsChemistry vtkglew vtkRenderingOpenGL2 vtkDomainsChemistryOpenGL2 vtkIOXML vtkParallelCore vtkFiltersAMR vtkFiltersFlowPaths vtkFiltersGeneric vtkImagingSources vtkFiltersHybrid vtkFiltersHyperTree vtkImagingGeneral vtkFiltersImaging vtkFiltersModeling vtkFiltersParallel vtkFiltersParallelImaging vtkFiltersPoints vtkFiltersProgrammable vtkFiltersSMP vtkFiltersSelection vtkFiltersTexture vtkFiltersTopology vtkFiltersVerdict vtkInteractionStyle vtkGUISupportQt vtksqlite vtkIOSQL vtkGUISupportQtSQL vtkmetaio vtkjpeg vtkpng vtktiff vtkIOImage vtkImagingHybrid vtkInfovisLayout vtkImagingColor vtkRenderingAnnotation vtkRenderingVolume vtkInteractionWidgets vtkViewsCore vtkproj4 vtkGeovisCore vtkhdf5_src vtkhdf5_hl_src vtkIOAMR vtkIOEnSight vtknetcdf vtkexodusII vtkIOExodus vtkgl2ps vtkRenderingGL2PSOpenGL2 vtklibharu vtkIOExport vtkIOExportOpenGL2 vtkIOGeometry vtkIOImport vtklibxml2 vtkIOInfovis vtkIOLSDyna vtkIOMINC vtkogg vtktheora vtkIOMovie vtknetcdfcpp vtkIONetCDF vtkIOPLY vtkjsoncpp vtkIOParallel vtkIOParallelXML vtkIOTecplotTable vtkIOVideo vtkImagingMath vtkImagingMorphological vtkImagingStatistics vtkImagingStencil vtkInteractionImage vtkRenderingContextOpenGL2 vtkRenderingImage vtkRenderingLOD vtkRenderingLabel vtkRenderingQt vtkRenderingVolumeOpenGL2 vtkViewsContext2D vtkViewsInfovis vtkViewsQt

# Test executable

#add_executable(
#    bow-simulator-tests
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
