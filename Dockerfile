ARG UBUNTU_VERSION=24.04
FROM ubuntu:${UBUNTU_VERSION} AS builder
ARG GPP_VERSION=13
ARG CPP_STANDARD=23
ARG PYTHON_VERSION_1=3
ARG PYTHON_VERSION_2=12
ARG BOOST_VERSION_1=1
ARG BOOST_VERSION_2=86
ARG BOOST_VERSION_3=0
ARG GRPC_VERSION_1=1
ARG GRPC_VERSION_2=66
ARG GRPC_VERSION_3=2

LABEL Description="C++ build environment"

ENV TZ=Europe/London
SHELL ["/bin/bash", "-c"]
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone && \
    DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get -y --no-install-recommends install \
        build-essential \
        g++-${GPP_VERSION} \
        cmake \
        gdb \
        wget \
        unzip \
        git \
        libtool \
        ssh \
        curl \
        apt-transport-https \
        ca-certificates \
        libgmp3-dev \
        libmpfr-dev  \
        python${PYTHON_VERSION_1}.${PYTHON_VERSION_2} \
        python3-pip \
        cmake \
        ninja-build \
        lcov \
        python3-venv

COPY bash_aliases_for_docker /root/.bash_aliases
COPY requirements.txt ./
RUN /usr/bin/python3 -m venv /root/venv && \
    source /root/venv/bin/activate && \
    pip3 install --no-cache-dir --upgrade pip && \
    pip3 install --no-cache-dir -r requirements.txt

RUN apt-get clean

RUN update-alternatives --remove-all gcc || true
RUN update-alternatives --remove-all g++ || true
RUN update-alternatives --remove-all c++ || true
RUN update-alternatives --remove-all cpp || true
RUN update-alternatives --remove-all gcc-nm || true
RUN update-alternatives --remove-all gcc-ranlib || true
RUN update-alternatives --remove-all gcc-ar || true

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GPP_VERSION} 100 \
    --slave /usr/bin/g++ g++ /usr/bin/g++-${GPP_VERSION} \
    --slave /usr/bin/c++ c++ /usr/bin/c++-${GPP_VERSION} \
    --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-${GPP_VERSION} \
    --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-${GPP_VERSION} \
    --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-${GPP_VERSION}

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
ENV BOOST_RELEASE_TAG boost_${BOOST_VERSION_1}_${BOOST_VERSION_2}_${BOOST_VERSION_3}
ENV BOOST_RELEASE_DIR ${BOOST_VERSION_1}.${BOOST_VERSION_2}.${BOOST_VERSION_3}
RUN echo "== Cloning boost version ${BOOST_RELEASE_TAG} from boostorg.jfrog.io ==" && \
    cd ${GIT_DIR} && \
    wget --no-check-certificate \
    https://boostorg.jfrog.io/artifactory/main/release/${BOOST_RELEASE_DIR}/source/${BOOST_RELEASE_TAG}.tar.gz && \
    tar xzf ./${BOOST_RELEASE_TAG}.tar.gz && \
    cd ./${BOOST_RELEASE_TAG} && \
    ./bootstrap.sh --prefix=/usr --with-python-version=${PYTHON_VERSION_1}.${PYTHON_VERSION_2}  && \
    ./b2 install toolset=gcc cxxflags="-std=c++${CPP_STANDARD}" && \
    cd ${GIT_DIR} &&  \
    rm -rf ./${BOOST_RELEASE_TAG}

### Install gRPC
ENV GRPC_RELEASE_TAG v${GRPC_VERSION_1}.${GRPC_VERSION_2}.${GRPC_VERSION_3}
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
COPY ./cmake /var/local/dklib/cmake
COPY CMakeLists.txt /var/local/dklib
WORKDIR /var/local/dklib/build
RUN cd /var/local/dklib/build && \
    cmake -Wno-dev -DCMAKE_INSTALL_PREFIX=/usr /var/local/dklib && \
    cmake --build /var/local/dklib/build --parallel $(nproc)  && \
    cmake --install /var/local/dklib/build && \
    cd /var/local && \
    rm -rf /var/local/dklib

RUN mkdir -p /app/scripts
COPY ./do_build /app/scripts
RUN chmod +x /app/scripts/do_build

WORKDIR /app/scripts

