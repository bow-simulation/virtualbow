# VirtualBow

![Build Status](https://github.com/bow-simulation/virtualbow/actions/workflows/build.yml/badge.svg)

VirtualBow is a free, open-source software tool for designing and simulating bows.
Visit http://www.virtualbow.org for more information about the project.

# Building

Building VirtualBow requires CMake, a C++17 compiler and the following external dependencies:

* [Qt 5.9.5](https://www.qt.io/)
* [Boost 1.79.0](https://www.boost.org/)
* [Catch 2.13.9](https://github.com/catchorg/Catch2)
* [Eigen 3.4.0](http://eigen.tuxfamily.org/)
* [Json 3.10.5](https://github.com/nlohmann/json)

Also used are [QCustomPlot](https://www.qcustomplot.com/) and [Calculate](https://github.com/newlawrence/Calculate), but those are already included in the source code.
The version numbers are just the minimum needed, newer versions might work too.

Optional pre-built dependencies for the supported platforms and compilers are available at [virtualbow-dependencies](https://github.com/bow-simulation/virtualbow-dependencies/releases).
They each contain a file named `paths.cmake` that will set up the `CMAKE_PREFIX_PATH` so that the libraries are found by CMake.
The easiest way to use it is by setting `CMAKE_TOOLCHAIN_FILE` to `paths.cmake` in your build,

    -DCMAKE_TOOLCHAIN_FILE=[dependencies]/paths.cmake

If you're using Qt Creator you can set this property under *Projects* - *Build Settings* - *CMake*. (If the configuration step keeps failing, try *Build* - *Clear CMake Configuration* and then run CMake again via *Build* - *Run CMake*.)

Below are some brief instructions for building on the supported platforms.
When in doubt, see the CI build definitions of [virtualbow](.github/workflows/build.yml) and [virtualbow-dependencies](https://github.com/bow-simulation/virtualbow-dependencies/blob/master/.github/workflows/build.yml) for the definitive source of truth.

## Windows

Using the GCC compiler and tools provided by [MinGW-w64](https://wiki.qt.io/MinGW),

    mkdir build && cd build
    cmake ../virtualbow -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=[...]/paths.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build .

Building with Microsoft Visual C++ should also be possible, but it is not officially supported or tested.
Contributions that fix issues with MSVC will be accepted.

## Linux and MacOS

On Linux, the Qt libraries packaged with the system are used.
On Ubuntu they can be installed with

    sudo apt install qtbase5-dev

The rest of the build process is the same on Linux and MacOS

    mkdir build && cd build
    cmake ../virtualbow -DCMAKE_TOOLCHAIN_FILE=[...]/paths.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build .

# Contributing

The development of VirtualBow is planned and coordinated openly by using GitHub's [issue tracker](https://github.com/bow-simulation/virtualbow/issues) and the [milestones](https://github.com/bow-simulation/virtualbow/milestones) feature.
Feel free to participate by creating new issues or commenting on existing ones.

For contributing to this repository please follow the steps below:

1. Fork the repository and create a new branch from `develop`
2. Implement, commit and push your changes
3. Create a pull request back to the `develop` branch of this repository. Link any relevant [issues](https://github.com/bow-simulation/virtualbow/issues).
4. Wait and see if the build pipeline passes. Keep updating your branch until it does.
5. Your changes will be reviewed for merging as soon as possible

All contributions are appreciated!

# License

VirtualBow is released under the GNU General Public License v3.0.
