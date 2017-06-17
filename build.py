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

def build_vtk():
    if os.path.exists("build/vtk"):
        return
    os.makedirs("build/vtk-build")

    subprocess.call(["apt", "install", "-y", "qtbase5-dev", "libqt5x11extras5-dev", "libxt-dev"])

    subprocess.call(["cmake",
    "-Hbuild/vtk-source",
    "-Bbuild/vtk-build",
    "-DCMAKE_INSTALL_PREFIX=build/vtk",
    "-DCMAKE_BUILD_TYPE=Release",
    "-DBUILD_SHARED_LIBS=OFF",
    "-DBUILD_TESTING=OFF",
    "-DVTK_Group_Qt=ON",
    "-DVTK_QT_VERSION=5"])

    subprocess.call(["make", "-C", "build/vtk-build"])
    subprocess.call(["make", "install", "-C", "build/vtk-build"])

def build_bow_simulator():
    if os.path.exists("build/bow-simulator"):
        return
    os.makedirs("build/bow-simulator")

    subprocess.call(["cmake",
    "-H.",
    "-Bbuild/bow-simulator",
    "-DCMAKE_BUILD_TYPE=Release"])

    subprocess.call(["make", "-C", "build/bow-simulator"])


# Eigen (http://eigen.tuxfamily.org)
download_and_extract("build/eigen", "http://bitbucket.org/eigen/eigen/get/3.3.3.tar.gz")

# Catch (https://github.com/philsquared/Catch)
download_and_extract("build/catch", "https://github.com/philsquared/Catch/archive/v1.9.3.tar.gz")

# Jsoncons (https://github.com/danielaparker/jsoncons)
download_and_extract("build/jsoncons", "https://github.com/danielaparker/jsoncons/archive/v0.99.7.2.tar.gz")

# Boost (http://www.boost.org/)
download_and_extract("build/boost", "https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.gz")

# VTK (http://www.vtk.org/)
download_and_extract("build/vtk-source", "https://gitlab.kitware.com/stfnp/vtk/repository/archive.tar.gz")
build_vtk()

# Application
build_bow_simulator()
