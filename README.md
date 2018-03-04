# About

Bow Simulator is a tool for simulating the physics of bow and arrow.
Visit https://bow-simulator.org for more information.

# Build instructions

The instructions below are how the "official" release versions are compiled.
Specific versions of tools and libraries are only mentioned as a reference.
Those aren't hard requirements, they are just most likely to work.

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
On the first build CMake will download and compile some external libraries (see `CMakeLists.txt`).
This can take an hour or two, depending on your hardware.

### Building the installer

Building the Windows installer requires some additional tools,

* [Inno Setup 5.5.9](http://www.jrsoftware.org/isdl.php). Add the installation directory `[...]\Inno Setup 5\` to the system path.

* [rcedit 1.0.0](https://github.com/electron/rcedit/releases). Download the executable, rename it to `rcedit` and add its location to the system path.

Open the project in Qt Creator and build the optional target `iss-installer` under *Projects*, *Build Steps*, *Details*.

## Linux
