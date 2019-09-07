# VirtualBow

[![Build Status](https://bow-simulation.visualstudio.com/virtualbow/_apis/build/status/bow-simulation.virtualbow?branchName=develop)](https://bow-simulation.visualstudio.com/virtualbow/_build/latest?definitionId=1&branchName=develop)

VirtualBow is a free, open-source software tool for designing and simulating bows.
Visit http://www.virtualbow.org for more information about the project.

# Building

Building VirtualBow requires CMake and a C++ compiler.
External dependencies are handled with [vcpkg](https://github.com/Microsoft/vcpkg), a cross-platform C++ package manager.
(The following instructions are slightly different from the official release builds.
For more information on those please refer to [Release.md](Release.md) and [azure-pipelines.yml](azure-pipelines.yml).)

After setting up vcpkg, the required dependencies can be installed as

    ./vcpkg install boost catch2 eigen3 nlohmann-json qt5-base

Add vcpkg's toolchain file to any subsequent `cmake` calls in order to make them available.

    -DCMAKE_TOOLCHAIN_FILE=[VCPKG_ROOT]/scripts/buildsystems/vcpkg.cmake

If you're using Qt Creator you can set this under *Projects* - *Build Settings* - *CMake*.
(If the configuration step keeps failing, try manually creating the folder specified as *Build directory* and then run CMake again.)

## Windows

    mkdir build && cd build
    cmake ../ -DCMAKE_TOOLCHAIN_FILE=[VCPKG_ROOT]/scripts/buildsystems/vcpkg.cmake
    cmake --build . --config Release

## Linux and MacOS

On linux you can optionally save some time by installing pre-compiled releases of Qt and Boost via your system's package manager.

    sudo apt install qtbase5-dev libqt5x11extras5-dev libboost-all-dev

Then build the application

    mkdir build && cd build
    cmake ../ -DCMAKE_TOOLCHAIN_FILE=[VCPKG_ROOT]/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
    cmake --build .

# Contributing

Any contribution is very much appreciated.
Follow the steps below to get your changes included in this repository.

1. Fork the repository and create a new branch from `develop`
2. Implement, commit and push your changes
3. Create a pull request back to the `develop` branch of this repository. Link any relevant [issues](https://github.com/bow-simulation/virtualbow/issues).
4. Wait and see if all checks pass. Keep updating your branch until they do.
5. Your changes will be reviewed as soon as possible

# License

VirtualBow is licensed under the GNU General Public License v3.0.
