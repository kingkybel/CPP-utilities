#!/bin/bash -x

#
# Generated - do not edit!
#

# Macros
TOP=`pwd`
CND_PLATFORM=GNU-Linux
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build
CND_DLIB_EXT=so
NBTMPDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tmp-packaging
TMPDIRNAME=tmp-packaging
OUTPUT_PATH=${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libC--utilities.${CND_DLIB_EXT}
OUTPUT_BASENAME=libC--utilities.${CND_DLIB_EXT}
PACKAGE_TOP_DIR=util/

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
makeDirectory "${NBTMPDIR}/util/include"
copyFileToTmpDir "include/anyutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/anyutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/include"
copyFileToTmpDir "include/bayesutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/bayesutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/include"
copyFileToTmpDir "include/csvutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/csvutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/include"
copyFileToTmpDir "include/dateutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/dateutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/include"
copyFileToTmpDir "include/floatingpoint.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/floatingpoint.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/include"
copyFileToTmpDir "include/graphutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/graphutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/include"
copyFileToTmpDir "include/statutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/statutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/include"
copyFileToTmpDir "include/stringutil.h" "${NBTMPDIR}/${PACKAGE_TOP_DIR}include/stringutil.h" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/src"
copyFileToTmpDir "src/anyutil.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}src/anyutil.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/src"
copyFileToTmpDir "src/bayesutil.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}src/bayesutil.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/src"
copyFileToTmpDir "src/csvutil.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}src/csvutil.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/src"
copyFileToTmpDir "src/dateutil.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}src/dateutil.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/src"
copyFileToTmpDir "src/floatingpoint.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}src/floatingpoint.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/src"
copyFileToTmpDir "src/graphutil.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}src/graphutil.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/src"
copyFileToTmpDir "src/statutil.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}src/statutil.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/src"
copyFileToTmpDir "src/stringutil.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}src/stringutil.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject"
copyFileToTmpDir "nbproject/project.xml" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/project.xml" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject"
copyFileToTmpDir "nbproject/configurations.xml" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/configurations.xml" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject/private"
copyFileToTmpDir "nbproject/private/configurations.xml" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/private/configurations.xml" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject/private"
copyFileToTmpDir "nbproject/private/Makefile-variables.mk" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/private/Makefile-variables.mk" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject/private"
copyFileToTmpDir "nbproject/private/private.xml" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/private/private.xml" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject/private"
copyFileToTmpDir "nbproject/private/launcher.properties" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/private/launcher.properties" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject"
copyFileToTmpDir "nbproject/Makefile-impl.mk" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/Makefile-impl.mk" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject"
copyFileToTmpDir "nbproject/Makefile-Debug.mk" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/Makefile-Debug.mk" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject"
copyFileToTmpDir "nbproject/Package-Debug.bash" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/Package-Debug.bash" 0755

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject"
copyFileToTmpDir "nbproject/Makefile-Release.mk" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/Makefile-Release.mk" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject"
copyFileToTmpDir "nbproject/Package-Release.bash" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/Package-Release.bash" 0755

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/nbproject"
copyFileToTmpDir "nbproject/Makefile-variables.mk" "${NBTMPDIR}/${PACKAGE_TOP_DIR}nbproject/Makefile-variables.mk" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util"
copyFileToTmpDir "Makefile" "${NBTMPDIR}/${PACKAGE_TOP_DIR}Makefile" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util"
copyFileToTmpDir "dox.cfg" "${NBTMPDIR}/${PACKAGE_TOP_DIR}dox.cfg" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util"
copyFileToTmpDir "layout.xml" "${NBTMPDIR}/${PACKAGE_TOP_DIR}layout.xml" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/tests"
copyFileToTmpDir "tests/tests.cc" "${NBTMPDIR}/${PACKAGE_TOP_DIR}tests/tests.cc" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644

cd "${TOP}"
makeDirectory "${NBTMPDIR}/util/lib"
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}lib/${OUTPUT_BASENAME}" 0644


# Generate tar file
cd "${TOP}"
rm -f ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/util-src.tar.gz
cd ${NBTMPDIR}
tar -v -zcf ../../../../${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/util-src.tar.gz *
checkReturnCode

# Cleanup
cd "${TOP}"
rm -rf ${NBTMPDIR}
