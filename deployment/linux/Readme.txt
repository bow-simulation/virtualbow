1. Install the following libraries (source: wolhsoft.ru/pgewiki/Building_static_Qt_5)

    * sudo apt-get install "^libxcb.*" libx11-dev libx11-xcb-dev libxcursor-dev libxrender-dev libxrandr-dev libxext-dev libxi-dev libxss-dev libxt-dev libxv-dev libxxf86vm-dev libxinerama-dev libxkbcommon-dev libfontconfig1-dev libasound2-dev libpulse-dev libdbus-1-dev udev mtdev-tools webp libudev-dev libglm-dev libwayland-dev libegl1-mesa-dev mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev libgles2-mesa libgles2-mesa-dev libmirclient-dev libproxy-dev libgtk2.0-dev

2. Download and unpack the Qt source code. Build and install a static version of Qt by running

    * ./configure -prefix /home/s/Qt/Static/5.7.1 -static -release -platform linux-g++ -qt-xcb -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-libjpeg -qt-freetype -qt-sql-sqlite -no-openssl -opensource -confirm-license -make libs -nomake tools -nomake examples -nomake tests

    * make -j 4
    
    * sudo make install

3. Setup Qt creator

    - Go to "Tools"-"Options"-"Build & Run" tab "Qt Versions"
    - Click "Add" and chose the static version's qmake at home/s/Qt/Static/5.7.1/bin/qmake
    - Rename, for example "Qt 5.7.1 GCC Static 32bit"
    - Click "Apply"
    - Go to tab "Kits", click "Add" and set a name like "Desktip Qt 5.7.1 GCC Static 64bit"
    - At "Qt Version" pick the previously defined static version
    - Ok

4. Open the project file in Qt Creator, configure with the new static kit and make a release build
