# Release Checklist

* Update version number and copyright notice in
    * User manual
    * Theory manual
    * CMakeLists.txt
* Solve all issues in the current milestone
* Implement a conversion from bow files of the previous version
* Execute manual tests defined below on all platforms
* Update user manual, place copy into resources/docs
* Create new changelog entry
* Create commit, merge to master and possibly develop
* Download artifacts from the CI-Pipeline and test manually
* On GitHub, create a new tag and release from the master branch, upload artifacts, user manual and theory manual
* Update website
    * Links on the download page
    * User manual and theory manual
    * Release announcement
* Post to r/VirtualBow 

# Manual Tests

* Tested platforms:
    * Windows 10
    * MacOS 10.11
    * Ubuntu 16.04
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
