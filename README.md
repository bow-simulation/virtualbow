# About

Bow Simulator is a tool for simulating the physics of bow and arrow.
Visit https://bow-simulator.org for more information.

# Build instructions

Bow Simulator is built with CMake.
The instructions below use the Qt-Creator IDE, which has built-in CMake support.
This is however not strictly necessary, you can just as well run CMake yourself.

The first build may take an hour or two, depending on your hardware.
This is because some necessary external dependencies will be downloaded and compiled first (see `CMakeLists.txt` for details).

## Windows

### Prerequisites

* [Microsoft Visual Studio 2017](https://www.visualstudio.com/downloads/) or alternatively the [Build Tools for Visual Studio 2017](https://www.visualstudio.com/downloads/#build-tools-for-visual-studio-2017) if you don't want the whole Visual Studio IDE.

* [Open Source edition of Qt](https://www.qt.io/download). The necessary components depend on the target architecture:
    * Qt 5.10.1 32-bit (MSVC 2015)
    * Qt 5.10.1 64-bit (MSVC 2015)

* [CMake 3.10.2](https://cmake.org/download/). Check the installer option to add CMake to the system path.

### Compiling with Qt Creator

Download or clone this repository and open `CMakeLists.txt` with Qt Creator.

Todo: Configuring Kits (Compiler, CMake, ...)

Todo: Generators, adding `[Qt-install-dir]\Tools\QtCreator\bin` to the system path.

Select the build type (e.g. *Release*) and perform a build.

### Building the installer

Building the Windows installer requires some additional tools,

* [Inno Setup 5.5.9](http://www.jrsoftware.org/isdl.php). Add the installation directory `[...]\Inno Setup 5\` to the system path.

* [rcedit 1.0.0](https://github.com/electron/rcedit/releases). Download the executable, rename it to `rcedit` and add its location to the system path.

Open the project in Qt Creator and check the optional target `iss-installer` under *Projects*, *Build Steps*, *Details*.

## Linux

### Prerequisites

* CMake and Qt-Creator (`cmake`, `qtcreator` on Ubuntu)

* Development libraries for Qt (`qtbase5-dev`, `libqt5x11extras-dev`, `libxt-dev` on Ubuntu)

### Compiling with Qt Creator

Download or clone this repository and open `CMakeLists.txt` with Qt Creator.
Pick the default *Desktop* kit and configure the project.

Select the build type (e.g. *Release*) and perform a build.

### Building the deb Package

Building the debian package requires `dpkg` to be installed. 

Open the project in Qt Creator and check the optional target `deb-package` under *Projects*, *Build Steps*, *Details*.

### Building the AppImage

Building the AppImage requires the [linuxdeployqt](https://github.com/probonopd/linuxdeployqt/releases), which is itself an AppImage.
Download it from github, rename it to `linuxdeployqt` and make it make it executable (`chmod a+x linuxdeployqt`).
Make sure it is found by the command line, e.g. by dropping it into your `/usr/local/bin/` folder.

Open the project in Qt Creator and check the optional target `appimage` under *Projects*, *Build Steps*, *Details*.
