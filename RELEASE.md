# Build and Deployment of Releases

## Windows

The Windows release uses custom `x86-windows-static-v140` and `x64-windows-static-v140` target triplets for the 32 bit and 64 bit versions respectively. They build static libraries and also link the msvc runtime statically, i.e. no Visual C++ Redistributable has to be installed on the target system. The `v140` platform toolset (Visual Studio 2015) is used because newer toolsets assume the UCRT to be present which is (out of the box) only the case for Windows 8 and later.

    .\vcpkg install boost catch2 eigen3 nlohmann-json qt5-base --triplet x64-windows-static-v140

When compiling, we need to specify the target triplet for selecting the correct library versions. In addition to that we have to make sure we are using matching compiler options: Overriding "/MD" with "/MT" in CMake's default compiler options makes sure we also use the static runtime. The custom preprocessor definition `QT_STATIC_WINDOWS` switches on some additional code that is required for static Qt builds.

    cmake ..\virtualbow
          -DCMAKE_TOOLCHAIN_FILE=C:\Users\Stefan\Desktop\Repos\vcpkg\scripts\buildsystems\vcpkg.cmake
          -DVCPKG_TARGET_TRIPLET=x64-windows-static-v140
          -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG /DQT_STATIC_WINDOWS"
          -A x64
		  -T v140
		  
	cmake --build . --config Release
	
For the 32 bit build, x64 has to be replaced with x86 in the commands above. (If you run into a bug "This project doesn't contain the Configuration and Platform combination of Debug|Win32", remove the -A flag from cmake. It will then use x86 by default.)

Building the installer requires Inno Setup and can be done by running

    ISCC setup.iss /O. /Finstaller
	
in the output directory.

## Linux

## MacOS