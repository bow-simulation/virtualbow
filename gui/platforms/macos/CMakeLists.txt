# Build an .app bundle as the main target, specify metadata and resource files
set_target_properties(
    virtualbow-gui
    virtualbow-cli
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/MacOS
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/MacOS
)

configure_file(Info.plist ${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/Info.plist)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/logo.icns DESTINATION ${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/Resources)
file(COPY ${CMAKE_SOURCE_DIR}/resources/docs/user-manual DESTINATION ${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/Resources)

# Target: Copy shared libraries to application directory
add_custom_target(copy-dependencies DEPENDS virtualbow-gui virtualbow-cli)
add_custom_command(
    TARGET copy-dependencies
    COMMAND ${QT_BINARY_DIR}/macdeployqt ${CMAKE_BINARY_DIR}/application/VirtualBow.app
        -executable=${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/MacOS/virtualbow-gui
        -executable=${CMAKE_BINARY_DIR}/application/VirtualBow.app/Contents/MacOS/virtualbow-cli
    COMMAND codesign --force --deep --sign - ${CMAKE_BINARY_DIR}/application/VirtualBow.app
)


# Target: Dmg Installer
configure_file(installer.json ${CMAKE_BINARY_DIR}/installer.json)
add_custom_target(dmg-installer DEPENDS copy-dependencies)
add_custom_command(
    TARGET dmg-installer
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/packages
    COMMAND appdmg ${CMAKE_BINARY_DIR}/installer.json ${CMAKE_BINARY_DIR}/packages/virtualbow.dmg
)
