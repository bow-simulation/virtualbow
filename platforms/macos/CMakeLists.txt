# Build an .app bundle as the main target, specify metadata

set_target_properties(virtualbow PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist
)

# Configure files needed for package creation

set(MACOSX_BUNDLE_ICON_FILE icon)
set(MACOSX_BUNDLE_BUNDLE_NAME ${APPLICATION_DISPLAY_NAME})
set(MACOSX_BUNDLE_GUI_IDENTIFIER virtualbow)
set(MACOSX_BUNDLE_LONG_VERSION_STRING ${APPLICATION_VERSION})
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${APPLICATION_VERSION})
set(MACOSX_BUNDLE_BUNDLE_VERSION ${APPLICATION_VERSION})
set(MACOSX_BUNDLE_INFO_STRING ${APPLICATION_DESCRIPTION_SHORT})
set(MACOSX_BUNDLE_COPYRIGHT ${APPLICATION_LICENSE})

configure_file(installer.json ${CMAKE_BINARY_DIR}/installer.json)
configure_file(Info.plist ${CMAKE_BINARY_DIR}/Info.plist)

# Target: Dmg Installer

add_custom_target(dmg-installer DEPENDS virtualbow)
get_target_property(QT_QMAKE_PATH Qt5::qmake IMPORTED_LOCATION)
get_filename_component(QT_BINARY_DIR ${QT_QMAKE_PATH} DIRECTORY)
add_custom_command(
    TARGET dmg-installer
    # Deploy Qt libraries (macdeployqt) and icon
    COMMAND ${QT_BINARY_DIR}/macdeployqt ${CMAKE_BINARY_DIR}/virtualbow.app
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/icon.icns ${CMAKE_BINARY_DIR}/virtualbow.app/Contents/Resources
    # Rename app bundle, build dmg installer
    COMMAND mv ${CMAKE_BINARY_DIR}/virtualbow.app ${CMAKE_BINARY_DIR}/VirtualBow.app
    COMMAND appdmg ${CMAKE_BINARY_DIR}/installer.json ${CMAKE_BINARY_DIR}/virtualbow.dmg
)
