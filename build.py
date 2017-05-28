import os
import sys
import urllib.request
import tarfile
import subprocess

def download_and_extract(path, url):
    # If path already exists, do nothing. Otherwise create it and proceed.
    if os.path.exists(path):
        return
    os.makedirs(path)

    def write_progress(count, block_size, total_size):
       progress = round(100*count*block_size/total_size, 2)
       progress = max(0, min(100, progress))
       sys.stdout.write("Downloading to %s... %d of %d bytes (%0.2f%%)\r" % (path, count*block_size, total_size, progress))

    # Download to a temporary tar file
    file, _ = urllib.request.urlretrieve(url, reporthook=write_progress)
    sys.stdout.write('\n')

    # Extract to path and discard top level directory contained in the tar file (http://stackoverflow.com/a/8261083/4692009)
    print("Extracting...")
    with tarfile.open(file) as tar:
        prefix = os.path.commonprefix(tar.getnames())

        if not prefix.endswith('/'):
            prefix += '/'

        for tarinfo in tar.getmembers():
            if tarinfo.name.startswith(prefix):
                tarinfo.name = tarinfo.name[len(prefix):]
                tar.extract(tarinfo, path)

def build_qt():
    if os.path.exists("build/qt"):
        return
    os.makedirs("build/qt-build")
    os.chdir("build/qt-build")

    # Install necessary libraries (http://doc.qt.io/qt-5/linux-requirements.html)
    subprocess.call(["apt", "install", "-y", "libfontconfig1-dev", "libfreetype6-dev", "libx11-dev", "libxext-dev",
    "libxfixes-dev", "libxi-dev", "libxrender-dev", "libxcb1-dev", "libx11-xcb-dev", "libxcb-glx0-dev"])

    # Fontconfig: https://doc.qt.io/archives/qtextended4.4/buildsystem/over-configure-options-qt-1.html
    subprocess.call(["apt", "install", "-y", "libfontconfig", "libfreetype"])

    # Install some more libraries, because why not? (https://wiki.qt.io/Install_Qt_5_on_Ubuntu, http://stackoverflow.com/questions/18794201/using-qt-without-opengl)
    subprocess.call(["apt", "install", "-y", "libglu1-mesa-dev", "mesa-common-dev"])

    # http://doc.qt.io/qt-5.8/configure-options.html
    # https://forum.qt.io/topic/38062/disabling-webkit/4
    subprocess.call(["../qt-source/configure", "-prefix", os.path.abspath("../qt"),
    "-opensource", "-confirm-license", "-static", "-release", "-platform", "linux-g++",
    "-qt-zlib", "-qt-libjpeg", "-qt-libpng", "-qt-xcb", "-qt-xkbcommon", "-qt-pcre", "-qt-harfbuzz", "-system-freetype", "-fontconfig",
    "-nomake", "tools", "-nomake", "examples", "-nomake", "tests",
    "-skip", "qtdeclarative"])

    # Todo: Skip building more of the unneccessary stuff, see -skip here: http://doc.qt.io/qt-5.8/configure-options.html
    # https://forum.qt.io/topic/65629/qt-5-6-linux-compile-fails/3
    # https://doc.qt.io/qt-5/qtgui-attribution-harfbuzz-ng.html
    # http://lists.qt-project.org/pipermail/interest/2016-April/022323.html

    subprocess.call(["make"])
    #subprocess.call(["make", "install"])
    os.chdir("../../")

def build_vtk():
    if os.path.exists("build/vtk"):
        return
    os.makedirs("build/vtk-build")

    # http://stackoverflow.com/a/24435795/4692009
    subprocess.call(["cmake",
    "-Hbuild/vtk-source",
    "-Bbuild/vtk-build",
    "-DCMAKE_INSTALL_PREFIX=build/vtk",
    "-DCMAKE_BUILD_TYPE=Release",
    "-DBUILD_SHARED_LIBS=OFF",
    "-DBUILD_TESTING=OFF",
    "-DVTK_Group_Qt=ON",
    "-DVTK_QT_VERSION=5",
    "-DQT_QMAKE_EXECUTABLE=" + os.path.abspath("build/qt/bin"),
    "-DQt5_DIR=" + os.path.abspath("build/qt/lib/cmake/Qt5")])

    subprocess.call(["make", "-C", "build/vtk-build"])
    subprocess.call(["make", "install", "-C", "build/vtk-build"])

# def build_application(build_dir):


# Eigen (http://eigen.tuxfamily.org)
download_and_extract("build/eigen", "http://bitbucket.org/eigen/eigen/get/3.3.3.tar.gz")

# Catch (https://github.com/philsquared/Catch)
download_and_extract("build/catch", "https://github.com/philsquared/Catch/archive/v1.9.3.tar.gz")

# Jsoncons (https://github.com/danielaparker/jsoncons)
download_and_extract("build/jsoncons", "https://github.com/danielaparker/jsoncons/archive/v0.99.7.2.tar.gz")

# Boost (http://www.boost.org/)
download_and_extract("build/boost", "https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.gz")

# Qt (https://www.qt.io/)
download_and_extract("build/qt-source", "https://download.qt.io/development_releases/qt/5.9/5.9.0-beta4/single/qt-everywhere-opensource-src-5.9.0-beta4.tar.xz")
build_qt()

# VTK (http://www.vtk.org/)
#download_and_extract("build/vtk-source", "https://gitlab.kitware.com/stfnp/vtk/repository/archive.tar.gz?ref=vtkcamera-horizontal-parallel-scale")
#build_vtk()