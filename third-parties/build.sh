#!/bin/bash

# build these third-party library, and install them into
# *installation* subdirectory 

source ../bin/env-config.sh

BIN_DIR=$(cd `dirname $0`; pwd)
PROJECT_ROOT_DIR=${BIN_DIR}/..
THIRDPARTY_ROOT=${PROJECT_ROOT_DIR}/third-parties
INSTALLATION_DIR=${THIRDPARTY_ROOT}/installation

# gflag
cd ${THIRDPARTY_ROOT}
if [[ -d ${GFLAG_FULLNAME} ]]
then
    echo "Directory ${GFLAG_FULLNAME} found"
else
    # untar the file
    tar -xf ${GFLAG_TAR_NAME}
fi
cd ${GFLAG_FULLNAME}

if [[ ! -d build ]]
then
    echo "Start to compile and install gflags..."
    mkdir build/ && cd build/

    cmake -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=ON \
        -DCMAKE_INSTALL_PREFIX=${INSTALLATION_DIR} ..
    make -j4
    make install
fi

# glog
cd ${THIRDPARTY_ROOT}
if [[ -d ${GLOG_FULLNAME} ]]
then
    echo "Directory ${GLOG_FULLNAME} found"
else
    # untar the file
    tar -xf ${GLOG_TAR_NAME}
fi
cd ${GLOG_FULLNAME}

if [[ ! -f config.log ]]
then
    echo "Start to compile and install glog..."
    CPPFLAGS="-I${INSTALLATION_DIR}/include ${CPPFLAGS}" \
    LDFLAGS="-L${INSTALLATION_DIR}/lib ${LDFLAGS}" \
    ./configure --prefix=${INSTALLATION_DIR}
    make -j4
    make install
fi

# googletest
cd ${THIRDPARTY_ROOT}
if [[ -d ${GOOGLETEST_FULLNAME} ]]
then
    echo "Directory ${GOOGLETEST_FULLNAME} found"
else
    # untar the file
    tar -xf ${GOOGLETEST_TAR_NAME}
fi
cd ${GOOGLETEST_FULLNAME}

if [[ ! -d build ]]
then
    echo "Start to compile and install googletest..."
    mkdir build/ && cd build/

    cmake -DCMAKE_INSTALL_PREFIX=${INSTALLATION_DIR} ..
    make -j4
    make install
fi


# protobuf
cd ${THIRDPARTY_ROOT}
if [[ -d ${PROTOBUF_FULLNAME} ]]
then
    echo "Directory ${PROTOBUF_FULLNAME} found"
else
    # untar the file
    tar -xf ${PROTOBUF_TAR_NAME}
fi
cd ${PROTOBUF_FULLNAME}

if [[ ! -f config.log ]]
then
    echo "Start to compile and install protobuf..."
    CPPFLAGS="-I${INSTALLATION_DIR}/include ${CPPFLAGS}" \
    LDFLAGS="-L${INSTALLATION_DIR}/lib ${LDFLAGS}" \
    ./configure --prefix=${INSTALLATION_DIR}/
    make -j4
    make install
fi
