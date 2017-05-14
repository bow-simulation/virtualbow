import os
import sys
import urllib.request
import tarfile

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

# def build_qt(source, destination):

# Eigen (http://eigen.tuxfamily.org)
download_and_extract("build/eigen", "http://bitbucket.org/eigen/eigen/get/3.3.3.tar.gz")

# Catch (https://github.com/philsquared/Catch)
download_and_extract("build/catch", "https://github.com/philsquared/Catch/archive/v1.9.3.tar.gz")

# Jsoncons (https://github.com/danielaparker/jsoncons)
download_and_extract("build/jsoncons", "https://github.com/danielaparker/jsoncons/archive/v0.99.7.2.tar.gz")

# Boost (http://www.boost.org/)
download_and_extract("build/boost", "https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.gz")

# Qt (https://www.qt.io/)
download_and_extract("build/qt-source", "http://download.qt.io/official_releases/qt/5.8/5.8.0/single/qt-everywhere-opensource-src-5.8.0.tar.gz")

# VTK (http://www.vtk.org/)
download_and_extract("build/vtk-source", "https://gitlab.kitware.com/stfnp/vtk/repository/archive.tar.gz?ref=vtkcamera-horizontal-parallel-scale")
