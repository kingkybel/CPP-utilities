# C++-utilities
A collection of C++-sample-files.

# Installation
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
If the build - subdirectory already exists rename it before executing docker-compose.
