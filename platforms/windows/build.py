import os
import sys
import subprocess
import shutil

generator = "NMake Makefiles"    # "Visual Studio 15 2017 Win64"
qt_path = "C:/Qt/5.9.1/msvc2015"

def build_vtk(source_dir, build_dir, output_dir):
    subprocess.call(["cmake",
    "-H" + source_dir,
    "-B" + build_dir,
	"-G" + generator,
    "-DCMAKE_INSTALL_PREFIX=" + output_dir,
	"-DCMAKE_BUILD_TYPE=Release",
    "-DBUILD_SHARED_LIBS=OFF",
    "-DBUILD_TESTING=OFF",
    "-DVTK_Group_Qt=ON",
    "-DVTK_QT_VERSION=5",
    "-DQt5_DIR=" + qt_path + "/lib/cmake/Qt5"])

    subprocess.call(["cmake", "--build", build_dir, "--target", "install"])
    
# Todo: Parametrize over Qt directory
def build_application(source_dir, build_dir, output_dir):
    subprocess.call(["cmake",
    "-H" + source_dir,
    "-B" + build_dir,
	"-G" + generator,
    "-DCMAKE_INSTALL_PREFIX=" + output_dir,
	"-DCMAKE_BUILD_TYPE=Release",
    "-DCMAKE_WIN32_EXECUTABLE=ON",
	"-DQt5Widgets_DIR=" + qt_path + "/lib/cmake/Qt5Widgets",
    "-DQt5PrintSupport_DIR=" + qt_path + "/lib/cmake/Qt5PrintSupport"])

    subprocess.call(["cmake", "--build", build_dir, "--target", "install"])
    subprocess.call([qt_path + "/bin/windeployqt", output_dir + "/bin/bow-simulator.exe",
                     "--no-translations", "--no-compiler-runtime"])
    subprocess.call(["rcedit", output_dir + "/bin/bow-simulator.exe", "--set-icon", "resources/icons/logo.ico"])
    
def build_packages(version, build_dir, output_dir):
    print("Build Packages")