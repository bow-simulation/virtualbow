# VirtualBow

[![Build Status](https://bow-simulation.visualstudio.com/virtualbow/_apis/build/status/bow-simulation.virtualbow?branchName=develop)](https://bow-simulation.visualstudio.com/virtualbow/_build/latest?definitionId=1&branchName=develop)

VirtualBow is a free, open-source software tool for designing and simulating bows.
Visit http://www.virtualbow.org for more information about the project.

# Building

Building VirtualBow requires CMake, a C++14 compiler and the following external dependencies: 

* [Qt](https://www.qt.io/)
* [Boost](https://www.boost.org/)
* [Catch](https://github.com/catchorg/Catch2)
* [Eigen](http://eigen.tuxfamily.org/)
* [Json](https://github.com/nlohmann/json)

Pre-built dependencies for the supported platforms can be found at [virtualbow-dependencies](https://github.com/bow-simulation/virtualbow-dependencies/releases).
They each contain a file named `paths.cmake` that will set up the `CMAKE_PREFIX_PATH` such that the correct libraries are found by CMake.
The easiest way to use it is by setting `CMAKE_TOOLCHAIN_FILE` to `paths.cmake` in your build,

    -DCMAKE_TOOLCHAIN_FILE=[dependencies]/paths.cmake

If you're using Qt Creator you can set this property under *Projects* - *Build Settings* - *CMake*. (If the configuration step keeps failing, try cleaning the CMake configuration or manually create the folder specified as *Build directory* and then run CMake again.)

## Windows

Using the GCC compiler from [MinGW-w64](http://mingw-w64.org/doku.php),

    mkdir build && cd build
    cmake ../ -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=[...]/paths.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build .

Building with Microsoft Visual C++ should also be possible, but is not officially supported anymore.
Contributions that fix issues with MSVC will still be accepted though.

## Linux and MacOS

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

VirtualBow is licensed under the GNU General Public License v3.0.