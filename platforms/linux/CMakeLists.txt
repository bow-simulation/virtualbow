# Configure files needed for package creation

configure_file(virtualbow.desktop ${CMAKE_BINARY_DIR}/virtualbow.desktop)
configure_file(control ${CMAKE_BINARY_DIR}/control)
configure_file(virtualbow.spec ${CMAKE_BINARY_DIR}/virtualbow.spec)

# Target: Linux directory layout

add_custom_target(linux-files DEPENDS virtualbow)
add_custom_command(
    TARGET linux-files
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/icons/logo.svg ${CMAKE_BINARY_DIR}/linux/usr/share/icons/hicolor/scalable/apps/virtualbow.svg
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/virtualbow.desktop ${CMAKE_BINARY_DIR}/linux/usr/share/applications/virtualbow.desktop
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/virtualbow ${CMAKE_BINARY_DIR}/linux/usr/bin/virtualbow
)

# Target: Deb package

add_custom_target(deb-package DEPENDS linux-files)
add_custom_command(
    TARGET deb-package
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/control ${CMAKE_BINARY_DIR}/deb_build/DEBIAN/control
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/linux ${CMAKE_BINARY_DIR}/deb_build
    COMMAND dpkg-deb --build ${CMAKE_BINARY_DIR}/deb_build ${CMAKE_BINARY_DIR}/virtualbow.deb
)

# Target: Rpm package

add_custom_target(rpm-package DEPENDS linux-files)
set(RPM_TEMP_NAME ${APPLICATION_NAME}-${APPLICATION_VERSION})
add_custom_command(
    TARGET rpm-package
    COMMAND mkdir -p ~/rpmbuild/RPMS
                     ~/rpmbuild/SRPMS
                     ~/rpmbuild/BUILD
                     ~/rpmbuild/SOURCES
                     ~/rpmbuild/SPECS
                     ~/rpmbuild/tmp
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/virtualbow.spec ~/rpmbuild/SPECS/virtualbow.spec
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/linux ~/rpmbuild/SOURCES/${RPM_TEMP_NAME}
    COMMAND cd ~/rpmbuild/SOURCES && tar -zcvf ${RPM_TEMP_NAME}.tar.gz ${RPM_TEMP_NAME} && cd -
    COMMAND rpmbuild -bb ~/rpmbuild/SPECS/virtualbow.spec
    COMMAND cp ~/rpmbuild/RPMS/*/*.rpm ${CMAKE_BINARY_DIR}/virtualbow.rpm
)

# Target: AppImage

add_custom_target(appimage DEPENDS linux-files)
add_custom_command(
    TARGET appimage
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/linux ${CMAKE_BINARY_DIR}/appimage
    COMMAND linuxdeployqt ${CMAKE_BINARY_DIR}/appimage/usr/share/applications/virtualbow.desktop -appimage -bundle-non-qt-libs -no-translations
    COMMAND mv ${CMAKE_BINARY_DIR}/*.AppImage ${CMAKE_BINARY_DIR}/virtualbow.AppImage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
