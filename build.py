import os
import sys
import shutil
import urllib.request
import tarfile
import platform

# Creates the directory 'path' if not present. Returns false if it already exists.
def create_directory(path):
    if os.path.exists(path):
        return False
    else:
        os.makedirs(path)
        return True

# Download .tar.gz from url and extract it to path
def download_and_extract(path, url):
    # If path already exists, do nothing. Otherwise create it and proceed.
    if not create_directory(path):
        return

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

# Download dependencies

# Eigen (http://eigen.tuxfamily.org)
download_and_extract("build/eigen", "http://bitbucket.org/eigen/eigen/get/3.3.3.tar.gz")
# Catch (https://github.com/philsquared/Catch)
download_and_extract("build/catch", "https://github.com/philsquared/Catch/archive/v1.9.3.tar.gz")
# Json for modern C++ (https://github.com/nlohmann/json)
# Use latest develop branch (zip): https://github.com/nlohmann/json/archive/develop.zip
download_and_extract("build/json", "https://github.com/nlohmann/json/archive/v2.1.1.tar.gz")
# Boost (http://www.boost.org/)
download_and_extract("build/boost", "http://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.gz")
# VTK (Fork) (http://www.vtk.org)
download_and_extract("build/vtk/source", "https://gitlab.kitware.com/stfnp/vtk/repository/archive.tar.gz?ref=vtkcamera-horizontal-parallel-scale")

# Build application depending on platform

if    platform.system() ==   "Linux": from platforms.linux.build   import *
elif  platform.system() == "Windows": from platforms.windows.build import *
elif  platform.system() ==  "Darwin": from platforms.mac_os.build  import *
else: sys.exit(platform.system() + " is not a supported platform.");

if create_directory("build/vtk/build"):
    build_vtk("build/vtk/source", "build/vtk/build", "build/vtk")

if create_directory("build/bow-simulator/build"):
    build_application(".", "build/bow-simulator/build", "build/bow-simulator")

if create_directory("build/packages"):
    build_packages("0.2", "build/packages/build", "build/packages")
