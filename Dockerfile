FROM ubuntu:22.04
LABEL Description="Build environment"

RUN apt-get update && \
    apt-get -y --no-install-recommends install \
    build-essential g++-12 cmake gdb wget unzip autoconf git libtool \
    ssh curl apt-transport-https ca-certificates libgmp3-dev libmpfr-dev python3.10 && \
    update-ca-certificates && \
    apt-get clean

RUN update-alternatives --remove-all gcc || true
RUN update-alternatives --remove-all g++ || true
RUN update-alternatives --remove-all c++ || true
RUN update-alternatives --remove-all cpp || true
RUN update-alternatives --remove-all gcc-nm || true
RUN update-alternatives --remove-all gcc-ranlib || true
RUN update-alternatives --remove-all gcc-ar || true

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    --slave /usr/bin/g++ g++ /usr/bin/g++-12 \
    --slave /usr/bin/c++ c++ /usr/bin/c++-12 \
    --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-12 \
    --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-12 \
    --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-12

RUN mkdir -p /root/.ssh && touch /root/.ssh/known_hosts
RUN ssh-keyscan github.com >> /root/.ssh/known_hosts

ENV GIT_DIR /var/local/git/
WORKDIR ${GIT_DIR}

### Install googletest
RUN echo "== Cloning googletest master from github.com ==" && \
    cd ${GIT_DIR} && \
    git clone https://github.com/google/googletest.git ${GIT_DIR}/googletest && \
    mkdir -p ${GIT_DIR}/googletest/build && \
    cd ${GIT_DIR}/googletest/build && \
    cmake .. && make -j $(nproc) && make -j $(nproc) install && \
    cd  ${GIT_DIR} && rm -rf ${GIT_DIR}/googletest

### Install Boost
ENV BOOST_RELEASE_TAG boost_1_83_0
ENV BOOST_RELEASE_DIR 1.83.0
RUN echo "== Cloning boost version ${BOOST_RELEASE_TAG} from boostorg.jfrog.io ==" && \
    cd ${GIT_DIR} && \
    wget --no-check-certificate --quiet \
    https://boostorg.jfrog.io/artifactory/main/release/${BOOST_RELEASE_DIR}/source/${BOOST_RELEASE_TAG}.tar.gz && \
    tar xzf ./${BOOST_RELEASE_TAG}.tar.gz && \
    cd ./${BOOST_RELEASE_TAG} && \
    ./bootstrap.sh && \
    ./b2 install toolset=gcc cxxflags="-std=c++20" && \
    cd ${GIT_DIR} && \
    rm -rf ./${BOOST_RELEASE_TAG}

### Install gRPC
ENV GRPC_RELEASE_TAG v1.58.1
RUN echo "== Cloning grpc version ${GRPC_RELEASE_TAG} from github.com ==" && \
    git clone -b ${GRPC_RELEASE_TAG} --recurse-submodules https://github.com/grpc/grpc ${GIT_DIR}/grpc && \
    mkdir -p ${GIT_DIR}/grpc/build && \
    cd ${GIT_DIR}/grpc/build/ && \
    cmake -DCMAKE_CXX_FLAGS=-std=c++20 .. && \
    make -j $(nproc) && \
    make -j $(nproc) install && make clean && ldconfig && \
    cd ${GIT_DIR} && rm -rf grpc

### Install CPP-Utilities

RUN mkdir -p /var/local/dklib/include
RUN mkdir -p /var/local/dklib/src
RUN mkdir -p /var/local/dklib/test
RUN mkdir -p /var/local/dklib/build
COPY ./include /var/local/dklib/include
COPY ./src /var/local/dklib/src
COPY ./test /var/local/dklib/test
COPY CMakeLists.txt /var/local/dklib
WORKDIR /var/local/dklib/build
RUN cd /var/local/dklib/build && \
    cmake .. && \
    make -j $(nproc) && \
    make -j $(nproc) install && \
    rm -rf /var/local/dklib

RUN mkdir -p /app/scripts
COPY ./do_build /app/scripts
RUN chmod +x /app/scripts/do_build

WORKDIR /app/scripts

SHELL ["/bin/bash", "-c"]



