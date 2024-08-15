# C++-utilities
A collection of C++-sample-files.

# Installation
The installation and build is tested on ./do    
## dependencies

BOOST:
```
# create a directory where you like to clone googletest, eg: ~/Repos and change to it
mkdir ~/Repos ; cd ~/Repos
```
From https://www.boost.org/users/history/version_1_84_0.html download `boost_1_84_0.tar.gz`

```
tar -xvf boost_1_84_0.tar.gz
cd boost_1_84_0
./bootstrap.sh
./b2
```

MPFR:
```
 sudo apt-get install libmpfr-dev libmpfr-doc
```

googletest:
```
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
```
mkdir ./build
cd build
cmake ..
make
sudo make install
```

## use autoconf (legacy - no longer maintained)
```
./create_configure.ksh
./configure
make
```

## Build docker-container
```
docker-compose build
```
If the build - subdirectory already exists rename it before executing docker-compose Before building.
