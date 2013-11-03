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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1868121903/circles.o \
	${OBJECTDIR}/_ext/1868121903/functions.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_highgui.so /usr/local/lib/libopencv_imgproc.so /usr/local/lib/libopencv_objdetect.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lesson3

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lesson3: /usr/local/lib/libopencv_core.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lesson3: /usr/local/lib/libopencv_highgui.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lesson3: /usr/local/lib/libopencv_imgproc.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lesson3: /usr/local/lib/libopencv_objdetect.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lesson3: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lesson3 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1868121903/circles.o: /home/stasstels/NetBeansProjects/git/ImageAnalysis/Lesson3/circles.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1868121903
	${RM} $@.d
	$(COMPILE.cc) -Werror -I/usr/local/include/opencv -I/usr/local/include/opencv2 -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1868121903/circles.o /home/stasstels/NetBeansProjects/git/ImageAnalysis/Lesson3/circles.cpp

${OBJECTDIR}/_ext/1868121903/functions.o: /home/stasstels/NetBeansProjects/git/ImageAnalysis/Lesson3/functions.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1868121903
	${RM} $@.d
	$(COMPILE.cc) -Werror -I/usr/local/include/opencv -I/usr/local/include/opencv2 -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1868121903/functions.o /home/stasstels/NetBeansProjects/git/ImageAnalysis/Lesson3/functions.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -Werror -I/usr/local/include/opencv -I/usr/local/include/opencv2 -std=c++98 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lesson3

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
