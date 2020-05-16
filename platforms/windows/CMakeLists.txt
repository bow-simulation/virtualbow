# Define math constants for MSVC
target_compile_definitions(virtualbow-gui PRIVATE _USE_MATH_DEFINES)
target_compile_definitions(virtualbow-slv PRIVATE _USE_MATH_DEFINES)
target_compile_definitions(virtualbow-post PRIVATE _USE_MATH_DEFINES)
target_compile_definitions(virtualbow-test PRIVATE _USE_MATH_DEFINES)

# Set executable icons
target_sources(virtualbow-gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/icons-gui.rc)
target_sources(virtualbow-slv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/icons-slv.rc)
target_sources(virtualbow-post PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/icons-post.rc)

# Make virtualbow-gui and virtualbow-post GUI applications
set_target_properties(virtualbow-gui PROPERTIES WIN32_EXECUTABLE TRUE)
set_target_properties(virtualbow-post PROPERTIES WIN32_EXECUTABLE TRUE)

# Copy necessary resources into application directory
configure_file(${CMAKE_SOURCE_DIR}/resources/docs/manual.pdf ${CMAKE_BINARY_DIR}/application/manual.pdf COPYONLY)

# Target: Copy shared libraries to application directory

add_custom_target(copy-dependencies DEPENDS virtualbow-gui virtualbow-slv virtualbow-post)
add_custom_command(
    TARGET copy-dependencies
    COMMAND ${QT_BINARY_DIR}/windeployqt
    ${CMAKE_BINARY_DIR}/application/virtualbow-gui.exe
    ${CMAKE_BINARY_DIR}/application/virtualbow-slv.exe
    ${CMAKE_BINARY_DIR}/application/virtualbow-post.exe
    --release
    --compiler-runtime
)

# Target: Inno Setup installer

configure_file(setup.iss ${CMAKE_BINARY_DIR}/setup.iss)
add_custom_target(iss-installer DEPENDS copy-dependencies)
add_custom_command(
    TARGET iss-installer
    COMMAND ISCC ${CMAKE_BINARY_DIR}/setup.iss /O${CMAKE_BINARY_DIR}/packages /Fvirtualbow-setup
)
