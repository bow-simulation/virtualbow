# Target: Linux directory layout

add_custom_target(linux-files DEPENDS virtualbow-gui virtualbow-post virtualbow-slv)
add_custom_command(
    TARGET linux-files
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/virtualbow-gui.desktop ${CMAKE_BINARY_DIR}/linux/usr/share/applications/virtualbow-gui.desktop
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/virtualbow-post.desktop ${CMAKE_BINARY_DIR}/linux/usr/share/applications/virtualbow-post.desktop
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/icons/logo.svg ${CMAKE_BINARY_DIR}/linux/usr/share/icons/hicolor/scalable/apps/virtualbow.svg
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/application/virtualbow-gui ${CMAKE_BINARY_DIR}/linux/usr/bin/virtualbow-gui
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/application/virtualbow-slv ${CMAKE_BINARY_DIR}/linux/usr/bin/virtualbow-slv
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/application/virtualbow-post ${CMAKE_BINARY_DIR}/linux/usr/bin/virtualbow-post
)

# Target: Deb package

configure_file(control ${CMAKE_BINARY_DIR}/control)
add_custom_target(deb-package DEPENDS linux-files)
add_custom_command(
    TARGET deb-package
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/control ${CMAKE_BINARY_DIR}/deb_build/DEBIAN/control
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/linux ${CMAKE_BINARY_DIR}/deb_build
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/packages
    COMMAND dpkg-deb --build ${CMAKE_BINARY_DIR}/deb_build ${CMAKE_BINARY_DIR}/packages/virtualbow.deb
)

# Target: Rpm package

configure_file(virtualbow.spec ${CMAKE_BINARY_DIR}/virtualbow.spec)
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
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/packages
    COMMAND cp ~/rpmbuild/RPMS/*/*.rpm ${CMAKE_BINARY_DIR}/packages/virtualbow.rpm
)
