# VirtualBow

## About

VirtualBow is a free, open-source software tool for designing and simulating bows. It is written in C++ and has a Qt user interface.

Visit http://bow-simulator.org for more information about the project.

## Building

VirtualBow requires a C++14 compiler and uses the CMake build system. The following instructions also use the cross platform [vcpkg](https://github.com/Microsoft/vcpkg) package manager for convenience, although not strictly necessary. After setting up vcpkg, the required dependencies can be installed as

    ./vcpkg install boost catch2 eigen3 nlohmann-json qt5-base

Add vcpkg's toolchain file to any `cmake` calls via

`-DCMAKE_TOOLCHAIN_FILE=[...]/vcpkg/scripts/buildsystems/vcpkg.cmake`

in order to make them available. (If you're using Qt Creator you can define this under *Projects* - *Build Steps* - *Details* - *Tool arguments*)

### Windows

...

### Linux

On linux you can optionally save some time by installing Qt and boost as pre-compiled packages via your system's package manager instead,

    sudo apt install qtbase5-dev libqt5x11extras5-dev libboost-all-dev

Build the application

    cmake [...]/virtualbow -DCMAKE_TOOLCHAIN_FILE=[...]/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
    make

### MacOS

...

## Contributing

1. Fork this repository and create a new branch from `develop`
2. Implement and commit your changes on this new branch
3. Create a merge request against the `develop` branch of this repository
4. Your changes will be reviewed as soon as possible

## License

VirtualBow is licensed under the GNU General Public License v3.0.
