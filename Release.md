# Build and Deployment of Releases

## Windows

(This section is not up to date, the Windows build was temporarily reverted to dynamic linking. See issue #140 for details.)

The Windows release uses custom `x86-windows-static-v140` and `x64-windows-static-v140` target triplets for the 32 bit and 64 bit versions respectively. They build static libraries and also link the msvc runtime statically, i.e. no Visual C++ Redistributable has to be installed on the target system. The `v140` platform toolset (Visual Studio 2015) is used because newer toolsets assume the UCRT to be present which is (out of the box) only the case for Windows 8 and later.

    .\vcpkg install boost catch2 eigen3 nlohmann-json qt5-base --triplet x64-windows-static-v140

When compiling, we need to specify the target triplet for selecting the correct library versions. In addition to that we have to make sure we are using matching compiler options: Overriding "/MD" with "/MT" in CMake's default compiler options makes sure we also use the static runtime. Passiing the custom preprocessor definition `QT_STATIC_PLUGINS` switches on some additional code that is required for static Qt builds.

    cmake ..\virtualbow
          -DCMAKE_TOOLCHAIN_FILE=C:\Users\Stefan\Desktop\Repos\vcpkg\scripts\buildsystems\vcpkg.cmake
          -DVCPKG_TARGET_TRIPLET=x64-windows-static-v140
          -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG /DQT_STATIC_PLUGINS"
          -A x64
          -T v140
        
    cmake --build . --config Release

For the 32 bit build, `x64` has to be replaced with `x86` in the commands above. (If you run into the bug "This project doesn't contain the Configuration and Platform combination of Debug|Win32", remove the `-A` flag from cmake. It will then use `x86` by default.)

Building the installer requires [Inno Setup](http://www.jrsoftware.org/isinfo.php) and is done by running

    cmake --build . --config Release --target iss-installer

in the output directory.

## Linux

The linux build is done with the static `x64-linux` default triplet, but Qt is linked dynamically by using the system package.

    `sudo apt install qt5-default`
    
The release packages are built with
    
    cmake --build . --target deb-package
    cmake --build . --target rpm-package
    cmake --build . --target appimage

This requires `dpkg`, `rpmbuild` and [Linuxdeployqt](https://github.com/probonopd/linuxdeployqt) respectively.

## MacOS

This build has to work around the fact that the latest Qt version in vcpkg doesn't support MacOS 10.11 (El Capitan) anymore, which is the only/latest system that is currently available to me for testing.
Therefore the slightly older Qt 5.11 release with the official pre-built (dynamic) binaries is used, together with `-DCMAKE_OSX_DEPLOYMENT_TARGET=10.11`.

The release package is built with

    cmake --build . --target dmg-installer

and requires [node-appdmg](https://github.com/LinusU/node-appdmg) for creating the installer image.
