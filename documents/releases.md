# Building the Release Versions

## Windows

The Windows version is built with [MinGW-w64](https://wiki.qt.io/MinGW).
It seems to generate faster code than Microsoft Visual C++ for our use case.

The Windows installer can be built with

    cmake --build . --target iss-installer

This requires [Inno Setup](http://www.jrsoftware.org/isinfo.php) to be installed and added to `PATH`.

## Linux

The Linux version is built with GCC on the oldest feasible Ubuntu LTS release, currently 18.04.
(The still supported LTS release 16.04 is not feasible, because Qt >= 5.6 is needed.)
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
The CMake option `DCMAKE_OSX_DEPLOYMENT_TARGET` is used to target versions.

The release package is built with

    cmake --build . --target dmg-installer

and requires [node-appdmg](https://github.com/LinusU/node-appdmg) for creating the installer image.

# Release Checklist

* Solve all issues in the current milestone
* Update version number and copyright notice in
    * User manual
    * Theory manual
    * CMakeLists.txt
* Implement the conversion from bow files of the previous version
* Execute basic manual tests defined below on all platforms and other tests if relevant
* Update user manual, place copy into resources/docs
* Create new changelog entry
* Create commit, merge to master and possibly develop
* Create a tag of the form vX.Y.Z, which triggers an automatic release
* Edit the release description and add latest changelog entry
* Update the website
    * Links on the download page
    * User manual and theory manual
    * Release announcement
    * Screenshots
* Post to r/VirtualBow 

# Manual Tests

## Basic Tests

* Tested platforms:
    * Windows 10
    * MacOS 10.11
    * Ubuntu 18.04
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

## File Actions

Described below is what should be happen when one of the file menu actions (New File, Open, Save, Save As, Quit) is performed, depending on the state of the editor (no file loaded, file loaded, file modified) and the subsequent choices made by the user.

- **No file loaded (default inputs)**
    - **New File:** Open a file dialog to pick a new filename and location
        - **Save:** Create new file at the chosen location and show its contents in the editor
        - **Cancel:** No action, back to the main window
    - **Open:** Open a file dialog and let the user pick an existing file
        - **Open:** Open the chosen file and show its contents in the editor
        - **Cancel:** No action, back to the main window
    - **Save:** Disabled
    - **Save As:** Disabled
    - **Quit:** Exit the application

- **File loaded**
    - **New File:** Open a file dialog to pick a new filename and location
        - **Save:** Create new file at the chosen location and show its contents in the editor
        - **Cancel:** No action, back to the main window
    - **Open:** Open a file dialog and let the user pick an existing file
        - **Open:** Open the chosen file and show its contents in the editor
        - **Cancel:** No action, back to the main window
    - **Save:** Save the contents of the editor to the current file (no effect)
    - **Save As:** Open a file dialog to pick a new filename and location
        - **Save:** Save contents to the chosen location and show them in the editor
        - **Cancel:** No action, back to the main window
    - **Quit:** Exit the application

- **File modified**
    - **New File:** Ask whether to save changes to the current file
        - **Yes:** Save current editor content. Open a file dialog to pick a new filename and location
            - **Save:** Create new file at the chosen location and show its contents in the editor
            - **Cancel:** No action, back to the main window
        - **No:** Create new file at the chosen location and show its contents in the editor
        - **Cancel:** No action, back to the main window
     - **Open:** Ask whether to save changes to the current file
        - **Yes:** Save current editor content. Open file dialog and let the user pick an existing file
            - **Open:** Create new file at the chosen location and show its contents in the editor
            - **Cancel:** No action, back to the main window
        - **No:** Create new file at the chosen location and show its contents in the editor
        - **Cancel:** No action, back to the main window
    - **Save:** Save the contents of the editor to the current file
    - **Save As:** Open a file dialog to pick a new filename and location
        - **Save:** Save contents to the chosen location and show them in the editor
        - **Cancel:** No action, back to the main window
    - **Quit:** Ask whether to save changes to the current file
        - **Yes:** Save the current editor content. Exit the application.
        - **No:** Exit the application
        - **Cancel:** No action, back to the main window
