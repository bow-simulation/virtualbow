target_compile_definitions(virtualbow-tests PRIVATE _USE_MATH_DEFINES)    # Define math constants on MSVC
target_sources(virtualbow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/icon.rc)     # Add resource file to set executable icon

# Configure files needed for package creation

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ISS_PROGRAM_DIR "{pf64}")    # Program Files
else()
    set(ISS_PROGRAM_DIR "{pf32}")    # Program Files (x86)
endif()

configure_file(setup.iss ${CMAKE_BINARY_DIR}/setup.iss)

# Target: Inno Setup installer

add_custom_target(iss-installer DEPENDS virtualbow)
add_custom_command(
    TARGET iss-installer
	COMMAND ISCC ${CMAKE_BINARY_DIR}/setup.iss /O${CMAKE_BINARY_DIR} /Fvirtualbow-setup
)

