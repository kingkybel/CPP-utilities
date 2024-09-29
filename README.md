# C++-utilities
A collection of C++-sample-files.

# Installation
The installation and build is tested on *ubuntu24.04 LTS*
## dependencies

BOOST:
```
# create a directory where you like to clone googletest, eg: ~/Repos and change to it
mkdir ~/Repos ; cd ~/Repos
```
From https://www.boost.org/users/history/version_1_84_0.html download `boost_1_86_0.tar.gz`

```bash
BOOST_VER=1_86_0
BOOST_VER_DOT=1.86.0
wget -O boost_${BOOST_VER}.tar.gz https://sourceforge.net/projects/boost/files/boost/${BOOST_VER_DOT}/boost_${BOOST_VER}.tar.gz/download
tar -xvf boost_${BOOST_VER}.tar.gz
cd boost_${BOOST_VER}
./bootstrap.sh --prefix=/usr/
./b2
sudo ./b2 install
```

MPFR:
```bash
 sudo apt-get install libmpfr-dev libmpfr-doc
```

googletest:
```bash
# create a directory where you like to clone googletest, eg: ~/Repos and change to it
mkdir ~/Repos ; cd ~/Repos
git clone https://github.com/google/googletest.git
cd googletest
mkdir build
cd build
mkdir build
cmake ..
make -j $(nproc)
sudo make install
```

## use cmake
```bash
mkdir ./build
cd build
cmake -Wno-dev -DCMAKE_INSTALL_PREFIX=/usr ..
cmake --build . --parallel $(nproc)
sudo cmake --install .
```

## use autoconf (legacy - no longer maintained)
```bash
./create_configure.ksh
./configure
make -j $(nproc)
```

## Build docker-container
```bash
docker compose build
```
If the build - subdirectory already exists rename it before executing docker-compose Before building.
