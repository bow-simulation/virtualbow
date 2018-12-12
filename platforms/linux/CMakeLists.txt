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

add_custom_target(linux-files DEPENDS ${CMAKE_BINARY_DIR}/linux-files/usr/bin/bow-simulator)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/linux-files/usr/bin/bow-simulator
    DEPENDS bow-simulator
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bow-simulator ${CMAKE_BINARY_DIR}/linux-files/usr/bin/bow-simulator
)

# Create deb package

configure_file(control.in ${CMAKE_BINARY_DIR}/deb/DEBIAN/control)

add_custom_target(deb-package DEPENDS bow-simulator.deb)
add_custom_command(
    OUTPUT bow-simulator.deb
    DEPENDS linux-files
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/linux-files ${CMAKE_BINARY_DIR}/deb
    COMMAND dpkg-deb --build ${CMAKE_BINARY_DIR}/deb ${CMAKE_BINARY_DIR}/bow-simulator.deb
)

# Create rpm package

configure_file(spec.in ~/rpmbuild/SPECS/application.spec)

# Set up rpm build environment. Todo: Find a way to do this in the local build folder instead of in the users home?
file(MAKE_DIRECTORY
    ~/rpmbuild/RPMS
    ~/rpmbuild/SRPMS
    ~/rpmbuild/BUILD
    ~/rpmbuild/SOURCES
    ~/rpmbuild/SPECS
    ~/rpmbuild/tmp
)

add_custom_target(rpm-package DEPENDS bow-simulator.rpm)

set(RPM_TEMP_NAME ${APPLICATION_NAME}-${APPLICATION_VERSION})

add_custom_command(
    OUTPUT bow-simulator.rpm
    DEPENDS linux-files
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/linux-files ~/rpmbuild/SOURCES/${RPM_TEMP_NAME}
    COMMAND cd ~/rpmbuild/SOURCES && tar -zcvf ${RPM_TEMP_NAME}.tar.gz ${RPM_TEMP_NAME} && cd -
    COMMAND rpmbuild -bb ~/rpmbuild/SPECS/application.spec
    COMMAND cp ~/rpmbuild/RPMS/*/*.rpm ${CMAKE_BINARY_DIR}/bow-simulator.rpm
)

# Create AppImage

add_custom_target(appimage DEPENDS bow-simulator.AppImage)
add_custom_command(
    OUTPUT bow-simulator.AppImage
    DEPENDS linux-files
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/linux-files ${CMAKE_BINARY_DIR}/appimage
    COMMAND linuxdeployqt ${CMAKE_BINARY_DIR}/appimage/usr/share/applications/bow-simulator.desktop -appimage -bundle-non-qt-libs -no-translations
    COMMAND mv ${CMAKE_BINARY_DIR}/*.AppImage ${CMAKE_BINARY_DIR}/bow-simulator.AppImage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
