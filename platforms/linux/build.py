import os
import sys
import subprocess
import shutil
import tarfile

def ensure_os_specific_dependencies():
    subprocess.call(["sudo", "apt", "install", "-y",
                     "cmake", "qtbase5-dev", "libqt5x11extras5-dev", "libxt-dev"
    ])

def build_vtk(source_dir, build_dir, output_dir):
    subprocess.call(["cmake",
    "-H" + source_dir,
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

def create_desktop_file(output_path, icon_path):
    file = open(output_path, "w")
    file.write("[Desktop Entry]\n"
               "Type=Application\n"
               "Name=Bow Simulator\n"
               "Exec=bow-simulator\n"
               "Icon=" + icon_path + "\n"
               "Comment=Bow and arrow physics simulation\n"
               "Categories=Education;Science;NumericalAnalysis;Physics;Engineering;\n"
               "MimeType=application/json\n")
    file.close()

def create_install_tree(output_dir):
    # Executable
    os.makedirs(output_dir + "/usr/local/bin")
    shutil.copy("build/bow-simulator/bin/bow-simulator", output_dir + "/usr/local/bin")    # Todo: Repetition

    # Icon
    os.makedirs(output_dir + "/usr/share/pixmaps")
    shutil.copyfile("resources/icons/logo.png", output_dir + "/usr/share/pixmaps/bow-simulator.png")

    # Desktop file
    os.makedirs(output_dir + "/usr/share/applications")
    create_desktop_file(output_dir + "/usr/share/applications/bow-simulator.desktop", "/usr/share/pixmaps/bow-simulator.png")

def build_deb_package(version, build_dir, output_dir):
    # Create directory structure that will be installed
    create_install_tree(build_dir)

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
    subprocess.call(["dpkg-deb", "--build", build_dir, output_dir + "/bow_simulator-" + version + "-" + "linux64" + ".deb"])

def build_snap_package(version, build_dir, output_dir):
    # Icon
    os.makedirs(build_dir + "/snap/gui")
    shutil.copyfile("resources/icons/logo.png", build_dir + "/snap/gui/icon.png")

    # Desktop file
    create_desktop_file(build_dir + "/snap/gui/bow-simulator.desktop", "${SNAP}/meta/gui/icon.png")

    # Snapcraft file
    file = open(build_dir + "/snapcraft.yaml", "w")
    file.write("name: bow-simulator\n"
               "version: " + version + "\n"
               "summary: Bow and arrow physics simulation\n"
               "description: Build virtual bow models and simulate their static and dynamic performance"
               "\n"
               "grade: stable\n"
               "confinement: strict\n"
               "\n"
               "apps:\n"
               "  bow-simulator:\n"
               "    command: desktop-launch bow-simulator\n"
               "    plugs: [x11, home]\n"
               "\n"
               "parts:\n"
               "  bow-simulator:\n"
               "    plugin: cmake\n"
               "    source: " + os.path.abspath(".") + "\n"
               "    configflags: [-DCMAKE_BUILD_TYPE=Release]\n"
               "    after: [desktop-qt5]\n")
    file.close()

    # Build package    # Todo: Set input and output folders for snapcraft
    cwd = os.getcwd()
    os.chdir(build_dir)
    subprocess.call(["snapcraft", "snap", "--output", "output.snap"])
    os.chdir(cwd)

    shutil.move(build_dir + "/output.snap", output_dir + "/bow_simulator-" + version + "-" + "linux64" + ".snap")

def build_packages(version, build_dir, output_dir):
    build_deb_package(version, build_dir + "/build-deb", output_dir)
    build_snap_package(version, build_dir + "/build-snap", output_dir)
