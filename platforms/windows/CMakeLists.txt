target_compile_definitions(virtualbow-tests PRIVATE _USE_MATH_DEFINES)    # Define math constants on MSVC
target_sources(virtualbow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/icon.rc)     # Add resource file to set executable icon
set_target_properties(virtualbow PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/windows)
set_target_properties(virtualbow PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/windows)

# Configure files needed for package creation

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ISS_PROGRAM_DIR "{pf64}")    # Program Files
else()
    set(ISS_PROGRAM_DIR "{pf32}")    # Program Files (x86)
endif()

configure_file(setup.iss ${CMAKE_BINARY_DIR}/setup.iss)

# Target: Windows directory layout

#add_custom_target(windows-files DEPENDS virtualbow)
#find_program(WINDEPLOYQT NAMES windeployqt PATHS "${CMAKE_PREFIX_PATH}/tools/qt5")
#add_custom_command(
#    TARGET windows-files
#    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/Release/virtualbow.exe ${CMAKE_BINARY_DIR}/windows/virtualbow.exe
#    COMMAND ${WINDEPLOYQT} ${CMAKE_BINARY_DIR}/windows/virtualbow.exe
#)

# Target: Inno Setup installer

add_custom_target(iss-installer DEPENDS virtualbow)
add_custom_command(
    TARGET iss-installer
    COMMAND ISCC ${CMAKE_BINARY_DIR}/setup.iss /O${CMAKE_BINARY_DIR} /Fvirtualbow-setup
)
