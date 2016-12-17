Release Build and Deployment on Mac-OSX
=======================================

1. Create a static build of Qt: Download and unpack the source code and run

    * ./configure -static -release -prefix /Users/s/Qt/Static/5.7.1 -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-freetype -qt-sql-sqlite -no-openssl -opensource -confirm-license -make libs -nomake tools -nomake examples -nomake tests

    * make -j 4

    * make install

2. Set up Qt creator to use the static version and do a release build of Bow Simulator

3. Take the resulting bow-simulator app file, right click and chose “Info”. Replace the Icon with resources/icons/logo.png

4. Rename the app to “Bow Simulator”
    * Right click on .app file, “Show Package Contents” go to “Contents”, rename executable
    * Rename executable in “Info.plist”
    * Rename .app file itself

5. Create .dmg installer using appdmg (https://github.com/LinusU/node-appdmg) with the spec.json file

    * appdmg spec.json "Bow Simulator.dmg"

6. Replace the logo of the resulting .dmg with logo.png