#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/anyutil.o \
	${OBJECTDIR}/src/bayesutil.o \
	${OBJECTDIR}/src/csvutil.o \
	${OBJECTDIR}/src/dateutil.o \
	${OBJECTDIR}/src/floatingpoint.o \
	${OBJECTDIR}/src/graphutil.o \
	${OBJECTDIR}/src/statutil.o \
	${OBJECTDIR}/src/stringutil.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L${BOOST_LIB_PATH} -Wl,-rpath,${BOOST_LIB_PATH} -lgmp -lgmpxx -lboost_filesystem

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/src/anyutil.o: src/anyutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/anyutil.o src/anyutil.cc

${OBJECTDIR}/src/bayesutil.o: src/bayesutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/bayesutil.o src/bayesutil.cc

${OBJECTDIR}/src/csvutil.o: src/csvutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/csvutil.o src/csvutil.cc

${OBJECTDIR}/src/dateutil.o: src/dateutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/dateutil.o src/dateutil.cc

${OBJECTDIR}/src/floatingpoint.o: src/floatingpoint.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/floatingpoint.o src/floatingpoint.cc

${OBJECTDIR}/src/graphutil.o: src/graphutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/graphutil.o src/graphutil.cc

${OBJECTDIR}/src/statutil.o: src/statutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/statutil.o src/statutil.cc

${OBJECTDIR}/src/stringutil.o: src/stringutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/stringutil.o src/stringutil.cc

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f1: ${TESTDIR}/tests/tests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc}   -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/tests/tests.o: tests/tests.cc 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/tests.o tests/tests.cc


${OBJECTDIR}/src/anyutil_nomain.o: ${OBJECTDIR}/src/anyutil.o src/anyutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/anyutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/anyutil_nomain.o src/anyutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/anyutil.o ${OBJECTDIR}/src/anyutil_nomain.o;\
	fi

${OBJECTDIR}/src/bayesutil_nomain.o: ${OBJECTDIR}/src/bayesutil.o src/bayesutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/bayesutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/bayesutil_nomain.o src/bayesutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/bayesutil.o ${OBJECTDIR}/src/bayesutil_nomain.o;\
	fi

${OBJECTDIR}/src/csvutil_nomain.o: ${OBJECTDIR}/src/csvutil.o src/csvutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/csvutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/csvutil_nomain.o src/csvutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/csvutil.o ${OBJECTDIR}/src/csvutil_nomain.o;\
	fi

${OBJECTDIR}/src/dateutil_nomain.o: ${OBJECTDIR}/src/dateutil.o src/dateutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/dateutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/dateutil_nomain.o src/dateutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/dateutil.o ${OBJECTDIR}/src/dateutil_nomain.o;\
	fi

${OBJECTDIR}/src/floatingpoint_nomain.o: ${OBJECTDIR}/src/floatingpoint.o src/floatingpoint.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/floatingpoint.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/floatingpoint_nomain.o src/floatingpoint.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/floatingpoint.o ${OBJECTDIR}/src/floatingpoint_nomain.o;\
	fi

${OBJECTDIR}/src/graphutil_nomain.o: ${OBJECTDIR}/src/graphutil.o src/graphutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/graphutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/graphutil_nomain.o src/graphutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/graphutil.o ${OBJECTDIR}/src/graphutil_nomain.o;\
	fi

${OBJECTDIR}/src/statutil_nomain.o: ${OBJECTDIR}/src/statutil.o src/statutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/statutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/statutil_nomain.o src/statutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/statutil.o ${OBJECTDIR}/src/statutil_nomain.o;\
	fi

${OBJECTDIR}/src/stringutil_nomain.o: ${OBJECTDIR}/src/stringutil.o src/stringutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/stringutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -O2 -I${BOOST_INCLUDE_PATH} -Iinclude -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/stringutil_nomain.o src/stringutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/stringutil.o ${OBJECTDIR}/src/stringutil_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libutil.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
