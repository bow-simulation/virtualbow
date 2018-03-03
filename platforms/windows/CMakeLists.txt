# Create windows directory layout

get_filename_component(QT_BINARY_DIR ${QT_QMAKE_EXECUTABLE} PATH)
add_custom_target(windows-files DEPENDS ${CMAKE_BINARY_DIR}/windows-files/bow-simulator.exe)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/windows-files/bow-simulator.exe
    DEPENDS bow-simulator
    # Program executable
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bow-simulator.exe ${CMAKE_BINARY_DIR}/windows-files/bow-simulator.exe
    # Qt libraries
    COMMAND ${QT_BINARY_DIR}/windeployqt ${CMAKE_BINARY_DIR}/windows-files/bow-simulator.exe --no-translations --no-compiler-runtime
    # MSVC runtime libraries
    COMMAND ${CMAKE_COMMAND} -E copy $ENV{VCToolsRedistDir}/$ENV{VSCMD_ARG_TGT_ARCH}/Microsoft.VC141.CRT/msvcp140.dll ${CMAKE_BINARY_DIR}/windows-files/msvcp140.dll
    COMMAND ${CMAKE_COMMAND} -E copy $ENV{VCToolsRedistDir}/$ENV{VSCMD_ARG_TGT_ARCH}/Microsoft.VC141.CRT/vcruntime140.dll ${CMAKE_BINARY_DIR}/windows-files/vcruntime140.dll
)

# Build installer using Inno Setup

configure_file(setup.iss.in ${CMAKE_BINARY_DIR}/iss/setup.iss)
configure_file(license.rtf ${CMAKE_BINARY_DIR}/iss/license.rtf COPYONLY)
configure_file(logo.ico ${CMAKE_BINARY_DIR}/iss/logo.ico COPYONLY)

add_custom_target(iss-installer DEPENDS installer.exe)
add_custom_command(
    OUTPUT installer.exe
    DEPENDS windows-files
    COMMAND ISCC
        ${CMAKE_BINARY_DIR}/iss/setup.iss
        /O${CMAKE_BINARY_DIR}/iss
        /F"installer"
        /D"BINARY_DIR="${CMAKE_BINARY_DIR}"/windows-files"
)
