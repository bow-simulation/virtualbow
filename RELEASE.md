# Build and Deployment of Releases

## Windows

The Windows release uses the `x86-windows-static` and `x64-windows-static` target triplets for the 32 bit and 64 bit versions respectively. This builds static libraries and also links the msvc runtime statically, i.e. no Visual C++ Redistributable has to be installed on the target machine.

    .\vcpkg install boost catch2 eigen3 nlohmann-json qt5-base --triplet x86-windows-static

When compiling, we need to specify the target triplet for selecting the correct library versions. In addition to that we have to make sure we are using matching compiler options: Overriding "/MD" with "/MT" in CMake's default compiler options makes sure we also use the static runtime. The custom preprocessor definition `QT_STATIC_WINDOWS` switches on some additional code that is required for static Qt builds.

    cmake ..\virtualbow -DCMAKE_TOOLCHAIN_FILE=C:\Users\Stefan\Desktop\Repos\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG /DQT_STATIC_WINDOWS"
	cmake --build . --config Release
	
## Linux

## MacOS