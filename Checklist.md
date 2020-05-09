# Release Checklist

* Solve all issues of the next milestone
* Implement conversion of bow files between versions
* Execute manual tests defined below on all platforms
* Update user manual, place copy into resources/docs
* Update version number and copyright notice in
    * User manual
    * Theory manual
    * CMakeLists.txt
* Create new changelog entry
* Create commit, merge to master and possibly develop
* Download artifacts from the CI-Pipeline and test manually
* On GitHub, create a new tag and release from the master branch, upload artifacts, user manual and theory manual
* Update website
    * Links on the download page
    * User manual and theory manual
    * Release announcement
* Post to r/VirtualBow 

# Manual Test Protocol

* Tested platforms:
    * Windows 10
    * MacOS 10.11
    * Ubuntu 16.04
    * Fedora 30

* Installation

* virtualbow-gui
    * Simulate all bows in /examples to verify compatibility with older versions
    
* virtualbow-slv

* virtualbow-post
