target_compile_definitions(virtualbow-tests PRIVATE _USE_MATH_DEFINES)    # Defines math constants on MSVC
target_sources(virtualbow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/icon.rc)     # Add resource file to set executable icon

# Create .iss installer source file

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ISS_PROGRAM_DIR "{pf64}")    # Program Files
else()
    set(ISS_PROGRAM_DIR "{pf32}")    # Program Files (x86)
endif()

configure_file(setup.iss ${CMAKE_BINARY_DIR}/setup.iss)