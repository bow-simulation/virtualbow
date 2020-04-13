# Build an .app bundle as the main target, specify metadata and resource files

set_target_properties(
    virtualbow-gui
    virtualbow-slv
    virtualbow-post
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/MacOS
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/MacOS
)

configure_file(Info.plist ${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/Info.plist)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/icon.icns ${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/Resources/icon.icns COPYONLY)

# Target: Dmg Installer

configure_file(installer.json ${CMAKE_BINARY_DIR}/installer.json)
add_custom_target(dmg-installer DEPENDS virtualbow-gui virtualbow-slv virtualbow-post)
get_target_property(QT_QMAKE_PATH Qt5::qmake IMPORTED_LOCATION)
get_filename_component(QT_BINARY_DIR ${QT_QMAKE_PATH} DIRECTORY)
add_custom_command(
    TARGET dmg-installer
    COMMAND ${QT_BINARY_DIR}/macdeployqt ${CMAKE_BINARY_DIR}/application/VirtualBow.app
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/packages
    COMMAND appdmg ${CMAKE_BINARY_DIR}/installer.json ${CMAKE_BINARY_DIR}/packages/virtualbow.dmg
)
