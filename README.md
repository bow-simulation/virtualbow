# VirtualBow

![Build Status](https://github.com/bow-simulation/virtualbow/actions/workflows/build.yml/badge.svg)

VirtualBow is a free, open-source software tool for designing and simulating bows.
Visit http://www.virtualbow.org for more information about the program and its features.

# Building

Building VirtualBow requires CMake, a C++17 compiler and the following external dependencies: 

* [Qt](https://www.qt.io/)
* [Boost](https://www.boost.org/)
* [Catch](https://github.com/catchorg/Catch2)
* [Eigen](http://eigen.tuxfamily.org/) (Master branch)
* [Json](https://github.com/nlohmann/json)
* [NLopt](https://github.com/stevengj/nlopt)

The following dependencies are included directly in the source code:

* [QCustomPlot](https://www.qcustomplot.com/)
* [tk::spline](https://kluge.in-chemnitz.de/opensource/spline/)

Optional pre-built dependencies for the supported platforms and compilers are available at [virtualbow-dependencies](https://github.com/bow-simulation/virtualbow-dependencies/releases).
They each contain a file named `paths.cmake` that will set up the `CMAKE_PREFIX_PATH` such that the correct libraries are found by CMake.
The easiest way to use it is by setting `CMAKE_TOOLCHAIN_FILE` to `paths.cmake` in your build,

    -DCMAKE_TOOLCHAIN_FILE=[dependencies]/paths.cmake

If you're using Qt Creator you can set this property under *Projects* - *Build Settings* - *CMake*. (If the configuration step keeps failing, try *Build* - *Clear CMake Configuration* and then run CMake again via *Build* - *Run CMake*.)

## Windows

Using the GCC compiler and tools provided by [MinGW-w64](https://wiki.qt.io/MinGW),

    mkdir build && cd build
    cmake ../ -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=[...]/paths.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build .

Building with Microsoft Visual C++ should also be possible, but it is not officially supported or tested.
Contributions that fix issues with MSVC will of course be accepted.

## Linux and MacOS

On Linux, the pre-built dependencies don't include Qt.
Instead the Qt libraries packaged with the system are used.
On Ubuntu they can be installed with

    sudo apt install qt5-default
    
The rest of the build process is the same on Linux and MacOS

    mkdir build && cd build
    cmake ../ -DCMAKE_TOOLCHAIN_FILE=[...]/paths.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build .

# Contributing

Contributions are much appreciated!
Please follow the steps below to get your changes included in this repository:

1. Fork the repository and create a new branch from `develop`
2. Implement, commit and push your changes
3. Create a pull request back to the `develop` branch of this repository. Link any relevant [issues](https://github.com/bow-simulation/virtualbow/issues).
4. Wait and see if the build pipeline passes. Keep updating your branch until it does.
5. Your changes will be reviewed for merging as soon as possible

# License

VirtualBow is released under the GNU General Public License v3.0.
