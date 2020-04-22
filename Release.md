# Build and Deployment of Releases

## Windows

The Windows version is built with [MinGW-w64](http://mingw-w64.org/doku.php). Comparisons have shown that it generates faster code than Microsoft Visual C++ for our use case.
The Qt version used is 5.12.8, because it is the first one to ship pre-built binaries for 64 bit MinGW and is also closest to version 5.11.3 to which we are currently tied to on MacOS.
For that reason no newer features than 5.11.3 can be used.

The Windows installer can be built with

    cmake --build . --target iss-installer

This requires [Inno Setup](http://www.jrsoftware.org/isinfo.php) to be installed and added to `PATH`.

## Linux

The Linux version is built with GCC and linked against Qt 5.11.3 to keep the usage of Qt features in sync with MacOS.
Other than Windows and MacOS though, the Qt libraries are not bundled with the application.
Instead the Qt libraries packaged by the system are used (e.g. `qt5-default` on Ubuntu). Those may be newer than the ones we built against.
The built is performed on the oldest still supported Ubuntu LTS version (currently 16.04) to ensure reasonable compatibility with other/older systems.

The release packages are built with
    
    cmake --build . --target deb-package
    cmake --build . --target rpm-package

This requires `dpkg` and `rpmbuild` respectively.

## MacOS

This version currently restricts us in several ways by the necessity to support MacOS 10.11 (El Capitan).
That's the latest OS that can be installed on my ancient 2009 MacBook that I got from eBay.
The latest Qt version to still support this system is 5.11.3, together with the CMake option `DCMAKE_OSX_DEPLOYMENT_TARGET=10.11`.
Also, no C++ standard newer than 14 can be used, which is what Xcode 8.2.1 provides.

So we're stuck with that until either someone buys me a new MacBook or we have a large enough community such that other people can reliably take over the task of maintaining the MacOS version.

The release package is built with

    cmake --build . --target dmg-installer

and requires [node-appdmg](https://github.com/LinusU/node-appdmg) for creating the installer image.
