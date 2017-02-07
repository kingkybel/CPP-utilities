#!/bin/bash -x

#
# Generated - do not edit!
#

# Macros
TOP=`pwd`
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build
CND_DLIB_EXT=so
NBTMPDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tmp-packaging
TMPDIRNAME=tmp-packaging
OUTPUT_PATH=${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.${CND_DLIB_EXT}
OUTPUT_BASENAME=libutil.${CND_DLIB_EXT}
PACKAGE_TOP_DIR=libutil.so/

# Functions
function checkReturnCode
{
    rc=$?
    if [ $rc != 0 ]
    then
        exit $rc
    fi
}
function makeDirectory
# $1 directory path
# $2 permission (optional)
{
    mkdir -p "$1"
    checkReturnCode
    if [ "$2" != "" ]
    then
      chmod $2 "$1"
      checkReturnCode
    fi
}
function copyFileToTmpDir
# $1 from-file path
# $2 to-file path
# $3 permission
{
    cp "$1" "$2"
    checkReturnCode
    if [ "$3" != "" ]
    then
        chmod $3 "$2"
        checkReturnCode
    fi
}

# Setup
cd "${TOP}"
mkdir -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package
rm -rf ${NBTMPDIR}
mkdir -p ${NBTMPDIR}

# Copy files and create directories and links
cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/tests"
copyFileToTmpDir "tests/tests.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}tests/tests.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/include"
copyFileToTmpDir "include/anyutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/anyutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/include"
copyFileToTmpDir "include/bayesutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/bayesutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/include"
copyFileToTmpDir "include/csvutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/csvutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/include"
copyFileToTmpDir "include/dateutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/dateutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/include"
copyFileToTmpDir "include/floatingpoint.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/floatingpoint.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/include"
copyFileToTmpDir "include/graphutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/graphutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/include"
copyFileToTmpDir "include/statutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/statutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/libutil.so/include"
copyFileToTmpDir "include/stringutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/stringutil.h" 0644


# Generate tar file
cd "${TOP}"
rm -f ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/libutil.so.tar
cd ${NBTMPDIR}
tar -vcf ../../../../${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/libutil.so.tar *
checkReturnCode

# Cleanup
cd "${TOP}"
rm -rf ${NBTMPDIR}
