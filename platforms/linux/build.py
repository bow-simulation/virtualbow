import os
import sys
import subprocess
import shutil

def build_vtk(build_dir, output_dir):
    subprocess.call(["apt", "install", "-y", "qtbase5-dev", "libqt5x11extras5-dev", "libxt-dev"])

    subprocess.call(["cmake",
    "-H" + ".",
    "-B" + build_dir,
    "-DCMAKE_INSTALL_PREFIX=" + output_dir,
    "-DCMAKE_BUILD_TYPE=Release",
    "-DBUILD_SHARED_LIBS=OFF",
    "-DBUILD_TESTING=OFF",
    "-DVTK_Group_Qt=ON",
    "-DVTK_QT_VERSION=5"])

    subprocess.call(["make", "-C", build_dir])
    subprocess.call(["make", "install", "-C", build_dir])

def build_application(source_dir, build_dir, output_dir):
    subprocess.call(["cmake",
    "-H" + source_dir,
    "-B" + build_dir,
    "-DCMAKE_INSTALL_PREFIX=" + output_dir,
    "-DCMAKE_BUILD_TYPE=Release"])

    subprocess.call(["make", "-C", build_dir])
    subprocess.call(["make", "install", "-C", build_dir])

def build_install_files(output_dir):
    # Executable
    os.makedirs(output_dir + "/usr/local/bin")
    shutil.copy("build/bow-simulator/bin/bow-simulator", output_dir + "/usr/local/bin")    # Todo: Repetition
    
    # Icon
    os.makedirs(output_dir + "/usr/share/pixmaps")
    shutil.copyfile("resources/icons/logo.png", output_dir + "/usr/share/pixmaps/bow-simulator.png")

    # Desktop file
    os.makedirs(output_dir + "/usr/share/applications")
    desktop = open(output_dir + "/usr/share/applications/bow-simulator.desktop", "w")
    desktop.write("[Desktop Entry]\n"
                  "Type=Application\n"
                  "Name=Bow Simulator\n"
                  "Exec=bow-simulator\n"
                  "Icon=/usr/share/pixmaps/bow-simulator.png\n"
                  "Comment=Bow and arrow physics simulation\n"
                  "Categories=Education;Science;NumericalAnalysis;Physics;Engineering;\n"
                  "MimeType=application/json\n")
    desktop.close()

def build_deb_package(version, build_dir, output_dir):
    # Create directory structure that will be installed
    build_install_files(build_dir)

    # Create Debian control file
    os.makedirs(build_dir + "/DEBIAN")
    control = open(build_dir + "/DEBIAN/control", "w")
    control.write("Package: bow-simulator\n"
                  "Version: " + version + "\n"
                  "Architecture: " + "amd64" + "\n"
                  "Maintainer: Stefan Pfeifer <s-pfeifer@gmx.net>\n"
                  "Homepage: https://bow-simulator.gitlab.io\n"
                  "Description: Bow and arrow physics simulation\n"
                  "Depends: qt5-default\n")
    control.close()
    
    # Build package
    subprocess.call(["dpkg-deb", "--build", build_dir, output_dir + "/bow-simulator-" + version + "-" + "amd64" + ".deb"])

def build_rpm_package(version, build_dir, output_dir):
    print("BUILD RPM PACKAGE")

def build_packages(version, build_dir, output_dir):
    build_deb_package(version, build_dir + "/build-deb", output_dir)
    build_rpm_package(version, build_dir + "/build-rpm", output_dir)
