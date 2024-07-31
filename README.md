# VirtualBow

![Build Status](https://github.com/bow-simulation/virtualbow/actions/workflows/build.yml/badge.svg)

VirtualBow software tool for designing and simulating bows.
Visit http://www.virtualbow.org for more information about the project.

# Building

## Solver

The solver is the part that does the numerical computations and is written in Rust.
Building the solver therefore requires a Rust compiler and the Cargo build tool.
Building the solver executable is as easy as running `cargo build --release`, which automatically downloads and compiles the required dependencies as well.
To run the tests, use `cargo test --release` (the release flag helps with performance since the simulation is very slow in debug mode).

## GUI

The VirtualBow GUI application built around the solver is written in C++.
Building it requires, in addition to the solver requirements, CMake, a C++17 compiler and the following external dependencies:

* [Qt 5.9.5](https://www.qt.io/)
* [Boost 1.79.0](https://www.boost.org/)
* [Eigen 3.4.0](http://eigen.tuxfamily.org/)
* [Json 3.10.5](https://github.com/nlohmann/json)

Also used are [QCustomPlot](https://www.qcustomplot.com/) and [Calculate](https://github.com/newlawrence/Calculate), but those are already included with this repository.
The version numbers are just the minimum needed, newer versions might work too.

Optional pre-built dependencies for the supported platforms and compilers are available at [virtualbow-dependencies](https://github.com/bow-simulation/virtualbow-dependencies/releases).
They each contain a file named `paths.cmake` that will set up the `CMAKE_PREFIX_PATH` so that the libraries are found by CMake.
The easiest way to use it is by setting `CMAKE_TOOLCHAIN_FILE` to `paths.cmake` in your build,

    -DCMAKE_TOOLCHAIN_FILE=[dependencies]/paths.cmake

If you're using Qt Creator you can set this property under *Projects* - *Build Settings* - *CMake*. (If the configuration step keeps failing, try *Build* - *Clear CMake Configuration* and then run CMake again via *Build* - *Run CMake*.)

Below are some brief instructions for building on the supported platforms.
When in doubt, see the CI build definitions of [virtualbow](.github/workflows/build.yml) and [virtualbow-dependencies](https://github.com/bow-simulation/virtualbow-dependencies/blob/master/.github/workflows/build.yml) for how they are actually compiled for releases.

### Windows

Using the GCC compiler and tools provided by [MinGW-w64](https://wiki.qt.io/MinGW),

    mkdir build && cd build
    cmake ../virtualbow/gui -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=[...]/paths.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build .

Building with Microsoft Visual C++ should also be possible, but it is not officially supported or tested.
Contributions that fix issues with MSVC will be accepted.

### Linux and MacOS

On Linux, the Qt libraries packaged with the system are used.
On Ubuntu they can be installed with

    sudo apt install qtbase5-dev

The rest of the build process is the same on Linux and MacOS

    mkdir build && cd build
    cmake ../virtualbow -DCMAKE_TOOLCHAIN_FILE=[...]/paths.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build .

# Contributing

Contributions of any kind are very welcome!

The development of VirtualBow is discussed and planned publicly on our [issue tracker](https://github.com/bow-simulation/virtualbow/issues) and the [discussions](https://github.com/bow-simulation/virtualbow/discussions).
Feel free to participate by either commenting on existing topics or opening your own ones for feedback, new ideas, feature requests or bug reports.
If you plan a larger contribution, consider discussing it with us first to make sure that it fits the scope and vision of the project.

For actually contributing to this repository please follow these steps:

1. **Fork the repository** and create a new branch for your changes from the latest `develop` branch. The branching strategy used in this repository is called [Git Flow](https://nvie.com/posts/a-successful-git-branching-model/) in case you want to learn more about it.

2. **Implement and test your changes** and add unit tests and/or documentation for your new code if applicable. The code style used in VirtualBow is not explicitly specified anywhere and not 100% consistent either. Just try to match the style of the existing/surrounding code reasonably well.

3. **Create a pull request** against the `develop` branch of this repository. Write a short summary of your changes and include any other relevant information like e.g. links to related issues or discussions.

4. **Wait and see if the build pipeline passes.** This automatic process ensures that the project still compiles successfully and all tests work as expected. Don't worry if this step fails initially, you can keep updating your branch with new commits until all checks have passed.

5. **Sign the Contributor License Agreement**. If this is your first contribution to VirtualBow, you will be asked to sign our [Contributor License Agreement](https://cla-assistant.io/bow-simulation/virtualbow) (CLA). This document clarifies the rights to your contribution and ensures that we can use your contribution(s) for our purposes. Our CLA is adapted from the [Apache Individual Contributor License Agreement](https://www.apache.org/licenses/icla.pdf) with only minor changes, so it is a fairly standard license agreement.

6. **You are almost done!** Your changes will now be reviewed for merging as soon as possible.

# License

VirtualBow is released under the [PolyForm Noncommercial License 1.0.0](LICENSE.md) and is therefore free to use for non-commercial (e.g. personal or academic) purposes.
