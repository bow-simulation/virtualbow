# Build and Deployment of Releases

## Windows

The Windows version is built with [MinGW-w64](http://mingw-w64.org/doku.php).
Comparisons have shown that it generates faster code than Microsoft Visual C++ for our use case.

The Windows installer can be built with

    cmake --build . --target iss-installer

This requires [Inno Setup](http://www.jrsoftware.org/isinfo.php) to be installed and added to `PATH`.

## Linux

The Linux version is built with GCC on the oldest feasible Ubuntu LTS release, currently 18.04.
(The stil supported LTS release 16.04 is not feasible, because Qt >= 5.6 is needed.)
This ensures reasonable compatibility with other/older systems.
Other than Windows and MacOS, the Qt libraries used are the ones packaged with the system (e.g. `qt5-default` on Ubuntu).
This means that VirtualBow has to compile with that Qt version, which currently is Qt 5.9.5.
Other platforms are built with newer Qt versions to take advantage of any bug fixes and improvements in Qt, but may not use newer features.

The release packages are built with
    
    cmake --build . --target deb-package
    cmake --build . --target rpm-package

This requires `dpkg` and `rpmbuild` respectively.

## MacOS

The oldest MacOS version still supported by Apple is currently 10.13, which is also what the latest Qt version supports.
The CMake option `DCMAKE_OSX_DEPLOYMENT_TARGET` is used to target  versions.

The release package is built with

    cmake --build . --target dmg-installer

and requires [node-appdmg](https://github.com/LinusU/node-appdmg) for creating the installer image.
