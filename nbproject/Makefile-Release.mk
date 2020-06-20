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
CND_PLATFORM=GNU-Linux
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
	${OBJECTDIR}/src/FFT.o \
	${OBJECTDIR}/src/anyutil.o \
	${OBJECTDIR}/src/bayesutil.o \
	${OBJECTDIR}/src/csvutil.o \
	${OBJECTDIR}/src/dateutil.o \
	${OBJECTDIR}/src/floatingpoint.o \
	${OBJECTDIR}/src/graphutil.o \
	${OBJECTDIR}/src/iosutil.o \
	${OBJECTDIR}/src/limited_int.o \
	${OBJECTDIR}/src/primes.o \
	${OBJECTDIR}/src/statutil.o \
	${OBJECTDIR}/src/stringutil.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f4 \
	${TESTDIR}/TestFiles/f3 \
	${TESTDIR}/TestFiles/f2 \
	${TESTDIR}/TestFiles/f1

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/tests/logValTest.o \
	${TESTDIR}/tests/matrixTest.o \
	${TESTDIR}/tests/utiltests.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 -std=c++17
CXXFLAGS=-m64 -std=c++17

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib64 -L/usr/local/lib/boost -L/usr/local/lib -Wl,-rpath,'/usr/local/lib64' -Wl,-rpath,'/usr/lib' -Wl,-rpath,'/usr/local/lib' -lgmp -lgmpxx -lboost_filesystem -lboost_system

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libC--utilities.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libC--utilities.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libC--utilities.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/src/FFT.o: src/FFT.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FFT.o src/FFT.cc

${OBJECTDIR}/src/anyutil.o: src/anyutil.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/anyutil.o src/anyutil.cc

${OBJECTDIR}/src/bayesutil.o: src/bayesutil.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/bayesutil.o src/bayesutil.cc

${OBJECTDIR}/src/csvutil.o: src/csvutil.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/csvutil.o src/csvutil.cc

${OBJECTDIR}/src/dateutil.o: src/dateutil.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/dateutil.o src/dateutil.cc

${OBJECTDIR}/src/floatingpoint.o: src/floatingpoint.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/floatingpoint.o src/floatingpoint.cc

${OBJECTDIR}/src/graphutil.o: src/graphutil.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/graphutil.o src/graphutil.cc

${OBJECTDIR}/src/iosutil.o: src/iosutil.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iosutil.o src/iosutil.cc

${OBJECTDIR}/src/limited_int.o: src/limited_int.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/limited_int.o src/limited_int.cc

${OBJECTDIR}/src/primes.o: src/primes.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/primes.o src/primes.cc

${OBJECTDIR}/src/statutil.o: src/statutil.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/statutil.o src/statutil.cc

${OBJECTDIR}/src/stringutil.o: src/stringutil.cc
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/stringutil.o src/stringutil.cc

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f4: ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f4 $^ ${LDLIBSOPTIONS}   -L/usr/local/lib `cppunit-config --libs`   

${TESTDIR}/TestFiles/f3: ${TESTDIR}/tests/logValTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f3 $^ ${LDLIBSOPTIONS}   -L/usr/local/lib `cppunit-config --libs`   

${TESTDIR}/TestFiles/f2: ${TESTDIR}/tests/matrixTest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f2 $^ ${LDLIBSOPTIONS}   -L/usr/local/lib 

${TESTDIR}/TestFiles/f1: ${TESTDIR}/tests/utiltests.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS}   -L/usr/local/lib 


${TESTDIR}/tests/logValTest.o: tests/logValTest.cc 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit `cppunit-config --cflags` -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/logValTest.o tests/logValTest.cc


${TESTDIR}/tests/matrixTest.o: tests/matrixTest.cc 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/matrixTest.o tests/matrixTest.cc


${TESTDIR}/tests/utiltests.o: tests/utiltests.cc 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/utiltests.o tests/utiltests.cc


${OBJECTDIR}/src/FFT_nomain.o: ${OBJECTDIR}/src/FFT.o src/FFT.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/FFT.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FFT_nomain.o src/FFT.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/FFT.o ${OBJECTDIR}/src/FFT_nomain.o;\
	fi

${OBJECTDIR}/src/anyutil_nomain.o: ${OBJECTDIR}/src/anyutil.o src/anyutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/anyutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/anyutil_nomain.o src/anyutil.cc;\
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
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/bayesutil_nomain.o src/bayesutil.cc;\
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
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/csvutil_nomain.o src/csvutil.cc;\
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
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/dateutil_nomain.o src/dateutil.cc;\
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
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/floatingpoint_nomain.o src/floatingpoint.cc;\
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
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/graphutil_nomain.o src/graphutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/graphutil.o ${OBJECTDIR}/src/graphutil_nomain.o;\
	fi

${OBJECTDIR}/src/iosutil_nomain.o: ${OBJECTDIR}/src/iosutil.o src/iosutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/iosutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iosutil_nomain.o src/iosutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/iosutil.o ${OBJECTDIR}/src/iosutil_nomain.o;\
	fi

${OBJECTDIR}/src/limited_int_nomain.o: ${OBJECTDIR}/src/limited_int.o src/limited_int.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/limited_int.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/limited_int_nomain.o src/limited_int.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/limited_int.o ${OBJECTDIR}/src/limited_int_nomain.o;\
	fi

${OBJECTDIR}/src/primes_nomain.o: ${OBJECTDIR}/src/primes.o src/primes.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/primes.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/primes_nomain.o src/primes.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/primes.o ${OBJECTDIR}/src/primes_nomain.o;\
	fi

${OBJECTDIR}/src/statutil_nomain.o: ${OBJECTDIR}/src/statutil.o src/statutil.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/statutil.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/statutil_nomain.o src/statutil.cc;\
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
	    $(COMPILE.cc) -I/usr/local/include -I/usr/include -Iinclude -I/usr/local/include/cppunit -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/stringutil_nomain.o src/stringutil.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/stringutil.o ${OBJECTDIR}/src/stringutil_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f4 || true; \
	    ${TESTDIR}/TestFiles/f3 || true; \
	    ${TESTDIR}/TestFiles/f2 || true; \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
