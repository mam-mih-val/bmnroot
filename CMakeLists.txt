# The name of our project is "BMNROOT".  CMakeLists files in this project can
# refer to the root source directory of the project as ${BMNROOT_SOURCE_DIR}
# or as ${CMAKE_SOURCE_DIR} and to the root binary directory of the project as
# ${BMNROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}.

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2 FATAL_ERROR)

### CMP0025   Compiler id for Apple Clang is now AppleClang.
### CMP0042   MACOSX_RPATH is enabled by default.
foreach(p
  CMP0025 # CMake 3.0
  CMP0028 # double colon for imported and alias targets
  CMP0042 # CMake 3.0
  CMP0054 # Only interpret ``if()`` arguments as variables or keywords when unquoted.
  )
  if(POLICY ${p})
  cmake_policy(SET ${p} NEW)
  endif()
endforeach()

# Set name of our project to "BMNROOT"
project(BMNROOT)

Option(USE_PATH_INFO "Information from PATH and LD_LIBRARY_PATH are used." ON)
Option(ALIBUILD "Flag if we are building with AliBuild." OFF)
# Possibility to run BmnRoot on cluster nodes with different OS (stability cannot be guaranteed)
Option(IGNORE_DIFFERENT_SYSTEM "Allow running compiled BmnRoot on different OS" ON)

#Check if necessary environment variables are set
#If not stop execution
IF (NOT ALIBUILD)
 IF(NOT DEFINED ENV{FAIRROOTPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable FAIRROOTPATH which is needed to find FairRoot. Please set this variable and execute cmake again.")
 ENDIF(NOT DEFINED ENV{FAIRROOTPATH})
 IF(NOT DEFINED ENV{SIMPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
 ENDIF(NOT DEFINED ENV{SIMPATH})

 SET(SIMPATH $ENV{SIMPATH})
 SET(FAIRROOTPATH $ENV{FAIRROOTPATH})
 SET(ZEROMQ_ROOT $ENV{SIMPATH})
ELSE (NOT ALIBUILD)
 SET(FAIRROOTPATH $ENV{FAIRROOT_ROOT})
 SET(GEANT3_PATH $ENV{GEANT3_ROOT})
 SET(BOOST_ROOT $ENV{BOOST_ROOT})
 SET(GSL_DIR $ENV{GSL_ROOT})
ENDIF (NOT ALIBUILD)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/
# is checked
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules_old" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${SIMPATH}/share/fairmq/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

Set(CheckSrcDir "${FAIRROOTPATH}/share/fairbase/cmake/checks")

find_package(FairRoot REQUIRED)

# Load some basic macros which are needed later on
include(BmnMacros)
include(FairMacros)
include(WriteConfigFile)
include(CTest)
include(CheckCompiler)

# Extract the FairRoot version from fairroot-config. The version info is of the form Major.Minor.Patch and is stored in the variable FairRoot_VERSION and FairSoft_VERSION
FairRootVersion()
FairSoftVersion()
Message(STATUS "FairRoot Version - ${FairRoot_VERSION}")
Message(STATUS "FairSoft Version - ${FairSoft_VERSION}")

# Set the build type: None, Debug, Release, RelWithDebInfo, MinSizeRel, OMP
# OMP - the quickest multithreading BmnRoot version for a multicore machine
If(NOT CMAKE_BUILD_TYPE)
  Message("Set BuildType DEBUG")
  set(CMAKE_BUILD_TYPE Debug)
EndIf(NOT CMAKE_BUILD_TYPE)

#Check the compiler and set the compile and link flags
Check_Compiler()

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")
Set(VMCWORKDIR ${CMAKE_SOURCE_DIR})

If(USE_PATH_INFO)
  Set(PATH "$PATH")
  If (APPLE)
    Set(LD_LIBRARY_PATH $ENV{DYLD_LIBRARY_PATH})
  Else (APPLE)
    Set(LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
  EndIf (APPLE)
Else(USE_PATH_INFO)
  STRING(REGEX MATCHALL "[^:]+" PATH $ENV{PATH})
EndIf(USE_PATH_INFO)

# Check if the user wants to build the project in the source
# directory and if the install directory is different from the build directory
CHECK_OUT_OF_SOURCE_BUILD()
CHECK_INSTALL_DIRECTORY()

# Check if we are on an UNIX system. If not stop with an error message
IF(NOT UNIX)
  MESSAGE(FATAL_ERROR "You're not on an UNIX system. The project was up to now only tested on UNIX systems, so we break here. If you want to go on please edit the CMakeLists.txt in the source directory.")
ENDIF(NOT UNIX)

# Check if the external packages are installed into a separate install directory
CHECK_EXTERNAL_PACKAGE_INSTALL_DIR()

# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is
# mandatory
find_package(ROOT 6.10.08 REQUIRED)
find_package(Pythia8)
find_package(Pythia6)
find_package(GEANT3 REQUIRED)
find_package(GEANT4)
find_package(GEANT4DATA)
find_package(GEANT4VMC)
find_package(CLHEP)
find_package(SSE)
find_package(PLUTO)
find_package(GENERATORS REQUIRED)
find_package(XML2 REQUIRED)
find_package(TDAQ)
find_package(ZeroMQ REQUIRED)
find_package(GSL)
#find_package(CUDA)
#find_package(IWYU)
#find_package(Protobuf)
#find_package(DDS)

IF (NOT ALIBUILD)
Set(Boost_NO_SYSTEM_PATHS TRUE)
Set(Boost_NO_BOOST_CMAKE TRUE)
If(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
  set(BOOST_ROOT ${SIMPATH})
  set(GSL_DIR ${SIMPATH})
Else(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
  set(BOOST_ROOT ${SIMPATH}/basics/boost)
  set(GSL_DIR ${SIMPATH}/basics/gsl)
EndIf(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
ENDIF (NOT ALIBUILD)
Message("-- Looking for Boost ...")
# If an older version of boost is found both of the variables below are
# cached and in a second cmake run, a good boost version is found even
# if the version is to old.
# To overcome this problem both variables are cleared before checking for boost.
Unset(Boost_INCLUDE_DIR CACHE)
Unset(Boost_LIBRARY_DIRS CACHE)
find_package(Boost 1.41)
If (Boost_FOUND)
  Set(Boost_Avail 1)
Else (Boost_FOUND)
  Set(Boost_Avail 0)
EndIf (Boost_FOUND)

# set a variable which should be used in all CMakeLists.txt
# Defines all basic include directories from fairbase
SetBasicVariables()
find_package(FairLogger)
set(BMNROOT TRUE)
add_definitions(-DBMNROOT)

# Set the BmnRoot libraries version
SET(BMNROOT_MAJOR_VERSION 0)
SET(BMNROOT_MINOR_VERSION 0)
SET(BMNROOT_PATCH_VERSION 0)
SET(BMNROOT_VERSION "${BMNROOT_MAJOR_VERSION}.${BMNROOT_MINOR_VERSION}.${BMNROOT_PATCH_VERSION}")
SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
    VERSION "${BMNROOT_VERSION}"
    SOVERSION "${BMNROOT_MAJOR_VERSION}"
    SUFFIX ".so"
)
set(FAIRROOT_FOUND FALSE)
Generate_Version_Info()
set(FAIRROOT_FOUND TRUE)

# Check if the compiler support specific C++11 features
# Up to now this is only a check since the code does not use
# any of the features of the new standard
include(CheckCXX11Features)
IF(HAS_CXX11_SHAREDPOINTER)
  Add_Definitions(-DHAS_SHAREDPOINTER)
ENDIF(HAS_CXX11_SHAREDPOINTER)

# Recurse into the given subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.
add_subdirectory (database)
add_subdirectory (passive)
add_subdirectory (bmnfield)
add_subdirectory (generators)
add_subdirectory (bmndata)
add_subdirectory (armTriggers)
add_subdirectory (bc)
add_subdirectory (bd)
add_subdirectory (SiMD)
add_subdirectory (FD)
add_subdirectory (scwall)
add_subdirectory (hodo)
add_subdirectory (mwpc)
#add_subdirectory (ssd)
add_subdirectory (tof1)
add_subdirectory (csc)
add_subdirectory (large_csc)
add_subdirectory (dch)
add_subdirectory (ecal)
add_subdirectory (zdc)
add_subdirectory (land)
add_subdirectory (silicon)
add_subdirectory (gem)
add_subdirectory (sts)
add_subdirectory (steering)
add_subdirectory (bmndst)
add_subdirectory (miscellaneous)
add_subdirectory (KF)
add_subdirectory (cat)
add_subdirectory (alignment)
add_subdirectory (decoder)
add_subdirectory (tof)
add_subdirectory (identification)
add_subdirectory (tracking)
add_subdirectory (globaltracking)
add_subdirectory (physics)
add_subdirectory (physics/bmnrecotools)
add_subdirectory (monitor)
add_subdirectory (QA)
add_subdirectory (eventdisplay)
add_subdirectory (macro)
add_subdirectory (services)

Option(BUILD_DOXYGEN "Build Doxygen" OFF)
if(BUILD_DOXYGEN)
  MESSAGE(STATUS "*** Building the Doxygen documentaion ***")
  ADD_SUBDIRECTORY(doxygen)
endif(BUILD_DOXYGEN)

WRITE_CONFIG_FILE(config.sh)

SET(VMCWORKDIR ${CMAKE_INSTALL_PREFIX})
SET(ROOT_INCLUDE_PATH ${CMAKE_INSTALL_PREFIX}/include ${ROOT_INCLUDE_PATH})
WRITE_CONFIG_FILE(install_config.sh)
