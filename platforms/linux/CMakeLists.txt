# https://stackoverflow.com/questions/33769077/executing-multiple-post-build-commands-in-cmake-depending-on-condition

# Create linux directory layout
configure_file(
    ${CMAKE_SOURCE_DIR}/resources/icons/logo.png
    ${CMAKE_BINARY_DIR}/deb/usr/share/pixmaps/bow-simulator.png
    COPYONLY
)

configure_file(
    desktop.in
    ${CMAKE_BINARY_DIR}/deb/usr/share/applications/bow-simulator.desktop
)

configure_file(control.in ${CMAKE_BINARY_DIR}/deb/DEBIAN/control)

# Create deb package
add_custom_command(
    OUTPUT bow_simulator.deb
    DEPENDS bow-simulator
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bow-simulator ${CMAKE_BINARY_DIR}/deb/usr/local/bin/bow-simulator
    COMMAND dpkg-deb --build ${CMAKE_BINARY_DIR}/deb ${CMAKE_BINARY_DIR}/bow_simulator.deb
)

add_custom_target(deb-package ALL DEPENDS bow_simulator.deb)
