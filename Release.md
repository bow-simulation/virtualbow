# Building the Release Versions

## Windows

The Windows version is built with [MinGW-w64](http://mingw-w64.org/doku.php).
It seems to generate faster code than Microsoft Visual C++ for our use case.

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

# Release Checklist

* Update version number and copyright notice in
    * User manual
    * Theory manual
    * CMakeLists.txt
* Solve all issues in the current milestone
* Implement a conversion from bow files of the previous version
* Execute manual tests defined below on all platforms
* Update user manual, place copy into resources/docs
* Create new changelog entry
* Create commit, merge to master and possibly develop
* Download artifacts from the CI-Pipeline and test manually
* On GitHub, create a new tag and release from the master branch, upload artifacts, user manual and theory manual
* Update website
    * Links on the download page
    * User manual and theory manual
    * Release announcement
* Post to r/VirtualBow 

# Manual Tests

* Tested platforms:
    * Windows 10
    * MacOS 10.11
    * Ubuntu 16.04
    * Fedora 30

* Install with default settings, check if VirtualBow and VirtualBow Post are available

* VirtualBow
    * Launch the aplication
    * Open *Help* - *User Manual*, check for correct version
    * Open *Help* - *About*, check for correct version
    * Launch a dynamic simulation with the default bow, skim results

* VirtualBow Post
    * Launch the application
    * Open results produced by previous simulation

* VirtualBow Solver
    * Simulate all `.bow` files of the previous version in /examples to verify compatibility
