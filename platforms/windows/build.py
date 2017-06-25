import os
import sys
import subprocess
import shutil

def build_vtk(source_dir, build_dir, output_dir):
    subprocess.call(["cmake",
    "-H" + source_dir,
    "-B" + build_dir,
	"-GNMake Makefiles",
    "-DCMAKE_INSTALL_PREFIX=" + output_dir,
	"-DCMAKE_BUILD_TYPE=Release",
    "-DBUILD_SHARED_LIBS=OFF",
    "-DBUILD_TESTING=OFF",
    "-DVTK_Group_Qt=ON",
    "-DVTK_QT_VERSION=5",
    "-DQt5_DIR=C:/Qt/5.7/msvc2015/lib/cmake/Qt5"])

    subprocess.call(["nmake", build_dir])

# Todo: Parametrize over Qt directory
def build_application(source_dir, build_dir, output_dir):
    subprocess.call(["cmake",
    "-H" + source_dir,
    "-B" + build_dir,
	"-GNMake Makefiles",
    "-DCMAKE_INSTALL_PREFIX=" + output_dir,
    "-DQt5PrintSupport_DIR=C:/Qt/5.7/msvc2015/lib/cmake/Qt5PrintSupport"])

    subprocess.call(["nmake", build_dir])
    subprocess.call(["C:/Qt/5.7/msvc2015/bin/windeployqt", output_dir + "/bin/bow-simulator.exe"])

def build_packages(version, source_dir, build_dir, output_dir):
    print("Build Packages")
##    
##    # Executable
##    os.makedirs(build_dir + "/package-deb/usr/local/bin")
##    shutil.copy("build/bow-simulator/bin/bow-simulator", build_dir + "/package-deb/usr/local/bin")
##    
##    # Icon
##    os.makedirs(build_dir + "/package-deb/usr/share/pixmaps")
##    shutil.copyfile("resources/icons/logo.png", build_dir + "/package-deb/usr/share/pixmaps/bow-simulator.png")
##
##    # Desktop file
##    os.makedirs(build_dir + "/package-deb/usr/share/applications")
##    desktop = open(build_dir + "/package-deb/usr/share/applications/bow-simulator.desktop", "w")
##    desktop.write("[Desktop Entry]\n"
##                  "Type=Application\n"
##                  "Name=Bow Simulator\n"
##                  "Exec=bow-simulator\n"
##                  "Icon=/usr/share/pixmaps/bow-simulator.png\n"
##                  "Comment=Bow and arrow physics simulation\n"
##                  "Categories=Education;Science;NumericalAnalysis;Physics;Engineering;\n"
##                  "MimeType=application/json\n")
##    desktop.close()
##
##    # Debian control file
##
##    os.makedirs(build_dir + "/package-deb/DEBIAN")
##    control = open(build_dir + "/package-deb/DEBIAN/control", "w")
##    control.write("Package: bow-simulator\n"
##                  "Version: " + version + "\n"
##                  "Architecture: " + "amd64" + "\n"
##                  "Maintainer: Stefan Pfeifer <s-pfeifer@gmx.net>\n"
##                  "Homepage: https://bow-simulator.gitlab.io\n"
##                  "Description: Bow and arrow physics simulation\n"
##                  "Depends: qt5-default\n")
##    control.close()
##    
##    subprocess.call(["dpkg-deb", "--build", build_dir + "/package-deb", install_dir + "bow-simulator-" + version + "-" + "amd64" + ".deb"])
