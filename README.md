# About

Bow Simulator is a tool for simulating the statics and dynamics of user-defined bow designs.
It is released under the GNU General Public License v3.0.
Visit https://bow-simulator.org for more information about the project.

# How to Contribute

1. Fork this repository on GitLab and create a new branch from `develop`
2. Implement and commit your changes on your new branch
3. Create a merge request back to the `develop` branch of this repository. Add a short summary of the changes and [link any relevant issues](https://docs.gitlab.com/ce/user/project/issues/crosslinking_issues.html).
4. Your changes will be reviewed as soon as possible

# Build instructions

Bow Simulator is built with CMake.
The instructions below use the Qt-Creator IDE, which has built-in CMake support.
This is however not strictly necessary, you can just as well run CMake yourself.

The first build may take an hour or longer, depending on your hardware.
This is because some necessary external dependencies will be downloaded and compiled first (see `CMakeLists.txt` for details).

## Windows

### Prerequisites

* [Microsoft Visual Studio 2017](https://www.visualstudio.com/downloads/). Select *Build Tools for Visual Studio 2017* if you don't want to install the whole Visual Studio IDE.

* [CMake 3.10.2](https://cmake.org/download/). Check the installer option to add CMake to the system path.

* [Qt 5.10.1 (Open Source)](https://www.qt.io/download). The necessary components depend on the target architecture:
    * Qt 5.10.1 32-bit (MSVC 2015)
    * Qt 5.10.1 64-bit (MSVC 2015)

### Compiling with Qt Creator

Download or clone this repository and open `CMakeLists.txt` with Qt Creator.
Pick the kit *Desktop Qt 5.10.1 MSVC2015 32bit* or *64bit* and configure the project.

Select the build type (e.g. *Release*) and perform a build.

If Qt Creator didn't auto-detect the C++ compiler or CMake, go to *Projects*, *Manage Kits...* and set them manually (*Microsoft Visual C++ Compiler 15.0 x86* for the 32bit Kit, *amd64* for the 64bit one).

Using the default CMake generator (*NMake Makefiles JOM*) might give you an error like this: "The C compiler ... is not able to compile a simple test program".
This can be solved by adding the directory `Qt/Tools/QtCreator/bin` of the Qt installation to the system path.

### Building the installer

Building the Windows installer requires some additional tools,

* [Inno Setup 5.5.9](http://www.jrsoftware.org/isdl.php). Add the installation directory to the system path.

* [rcedit 1.0.0](https://github.com/electron/rcedit/releases). Download the executable, rename it to `rcedit` and add its location to the system path.

In Qt Creator, check the target `iss-installer` under *Projects*, *Build Steps*, *Details* and perform a build.

## Linux

### Prerequisites

* GCC C and C++ Compilers. On Ubuntu they're in the `build-essentials` package.

* CMake. Package `cmake` on Ubuntu.

* [Qt 5.10.1 (Open Source)](https://www.qt.io/download). Install the following components:
    * Qt 5.10.1 Desktop gcc 64-bit

* Additional graphics libraries. On Ubuntu those are `mesa-common-dev`, `libglu1-mesa-dev` and `libxt-dev`.

### Compiling with Qt Creator

Download or clone this repository and open `CMakeLists.txt` with Qt Creator.
Pick the kit *Desktop Qt 5.10.1 GCC 64bit* and configure the project.

Select the build type (e.g. *Release*) and perform a build.

### Building the deb Package

Building the debian package requires `dpkg` to be installed. In Qt Creator, check the target `deb-package` under *Projects*, *Build Steps*, *Details* and perform a build.

### Building the AppImage

Building the AppImage requires [linuxdeployqt](https://github.com/probonopd/linuxdeployqt/releases), which is itself an AppImage.
Download it from github and make it make it executable via `chmod a+x ...`.
Ensure that it is found as `linuxdeployqt` by the command line, for example by dropping or linking it in your `/usr/local/bin/` folder.
Also make sure that the `qmake` executable is found and points to the correct Qt installation (e.g. `/home/Qt/5.10.1/gcc_64/bin/qmake`).

In Qt Creator, check the target `appimage` under *Projects*, *Build Steps*, *Details* and perform a build.
