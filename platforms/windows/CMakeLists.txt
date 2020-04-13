# Define math constants on MSVC
target_compile_definitions(virtualbow-gui PRIVATE _USE_MATH_DEFINES)
target_compile_definitions(virtualbow-slv PRIVATE _USE_MATH_DEFINES)
target_compile_definitions(virtualbow-post PRIVATE _USE_MATH_DEFINES)
target_compile_definitions(virtualbow-test PRIVATE _USE_MATH_DEFINES)

# Set executable icons
target_sources(virtualbow-gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/icons-gui.rc)
target_sources(virtualbow-slv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/icons-slv.rc)
target_sources(virtualbow-post PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/icons-post.rc)

# Target: Copy shared libraries to application directory

get_target_property(QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
find_program(WINDEPLOYQT NAMES windeployqt HINTS "${QT_BIN_DIR}")

add_custom_target(copy-dependencies DEPENDS virtualbow-gui virtualbow-slv virtualbow-post)
add_custom_command(
    TARGET copy-dependencies
    COMMAND ${WINDEPLOYQT}
	${CMAKE_BINARY_DIR}/application/virtualbow-gui.exe
	${CMAKE_BINARY_DIR}/application/virtualbow-slv.exe
	${CMAKE_BINARY_DIR}/application/virtualbow-post.exe
	--release
)

# Target: Inno Setup installer

configure_file(setup.iss ${CMAKE_BINARY_DIR}/setup.iss)
add_custom_target(iss-installer DEPENDS copy-dependencies)
add_custom_command(
    TARGET iss-installer
    COMMAND ISCC ${CMAKE_BINARY_DIR}/setup.iss /O${CMAKE_BINARY_DIR}/packages /Fvirtualbow-setup
)
