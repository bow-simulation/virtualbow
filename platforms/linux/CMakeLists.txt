# https://stackoverflow.com/questions/33769077/executing-multiple-post-build-commands-in-cmake-depending-on-condition

# Create linux directory layout

configure_file(
    ${CMAKE_SOURCE_DIR}/resources/icons/logo.svg
    ${CMAKE_BINARY_DIR}/linux-files/usr/share/icons/hicolor/scalable/apps/bow-simulator.svg
    COPYONLY
)

configure_file(
    desktop.in
    ${CMAKE_BINARY_DIR}/linux-files/usr/share/applications/bow-simulator.desktop
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/linux-files/usr/bin/bow-simulator
    DEPENDS bow-simulator
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bow-simulator ${CMAKE_BINARY_DIR}/linux-files/usr/bin/bow-simulator
)
add_custom_target(linux-files ALL DEPENDS ${CMAKE_BINARY_DIR}/linux-files/usr/bin/bow-simulator)

# Create deb package

configure_file(control.in ${CMAKE_BINARY_DIR}/deb/DEBIAN/control)

add_custom_command(
    OUTPUT bow_simulator.deb
    DEPENDS linux-files
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/linux-files ${CMAKE_BINARY_DIR}/deb
    COMMAND dpkg-deb --build ${CMAKE_BINARY_DIR}/deb ${CMAKE_BINARY_DIR}/bow_simulator.deb
)
add_custom_target(deb-package DEPENDS bow_simulator.deb)

# Create AppImage

add_custom_command(
    OUTPUT Bow_Simulator-x86_64.AppImage
    DEPENDS linux-files
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/linux-files ${CMAKE_BINARY_DIR}/appimage
    COMMAND linuxdeployqt ${CMAKE_BINARY_DIR}/appimage/usr/share/applications/bow-simulator.desktop -appimage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(appimage DEPENDS Bow_Simulator-x86_64.AppImage)