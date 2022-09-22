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

# Set the build type: None, Debug, Release, RelWithDebInfo, MinSizeRel, OMP
# OMP - the quickest multithreading BmnRoot version for a multicore machine
If(NOT CMAKE_BUILD_TYPE)
  Message("Set BuildType DEBUG")
  set(CMAKE_BUILD_TYPE Debug)
EndIf(NOT CMAKE_BUILD_TYPE)

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
 SET(ZEROMQ_ROOT $ENV{SIMPATH})
 SET(GEANT3_PATH $ENV{GEANT3_ROOT})
 SET(BOOST_ROOT $ENV{BOOST_ROOT})
 SET(GSL_DIR $ENV{GSL_ROOT})
ENDIF (NOT ALIBUILD)

if(APPLE)
  if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
  endif()
  if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
  endif()
endif()

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules_old" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${SIMPATH}/share/fairmq/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

# Generate compile_commands.json file (https://clang.llvm.org/docs/JSONCompilationDatabase.html)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define CMAKE_INSTALL_*DIR family of variables
include(GNUInstallDirs)
#include(FairRootTargetRootDictionary)

# Define install dirs
set(PROJECT_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
set(PROJECT_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(PROJECT_INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR}/bmnroot) # TODO: change 'bmnroot' to ${PROJECT_NAME_LOWER}

# https://cmake.org/Wiki/CMake_RPATH_handling
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_LIBDIR}" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../${PROJECT_INSTALL_LIBDIR}")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_INSTALL_RPATH "@loader_path/../${PROJECT_INSTALL_LIBDIR}")
  else()
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_LIBDIR}")
  endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-Wl,--enable-new-dtags")
  set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-Wl,--enable-new-dtags")
endif()

if(CMAKE_GENERATOR STREQUAL "Ninja" AND
  ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9) OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)))
  # Force colored warnings in Ninja's output, if the compiler has -fdiagnostics-color support.
  # Rationale in https://github.com/ninja-build/ninja/issues/814
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif()

# Set a default if not specified by the user
if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()

# Check if the compiler support specific C++11 features
# Up to now this is only a check since the code does not use
# any of the features of the new standard
Set(CheckSrcDir "${FAIRROOTPATH}/share/fairbase/cmake/checks")
include(CheckCXX11Features)
include(CheckSymbolExists)
If(NOT _HAS_CXX11_FLAG)
  Message(FATAL_ERROR "The used C++ compiler (${CMAKE_CXX_COMPILER}) does not support C++11. BmnRoot can only be compiled with compilers supporting C++11. Please install such a compiler.")
EndIf()

find_package(FairRoot REQUIRED)

# Load some basic macros which are needed later on
include(BmnMacros)
include(FairMacros)
include(WriteConfigFile)
include(Dart)
include(CTest)
include(CheckCompiler)
include(CbmMacros)

# Set the build type: None, Debug, Release, RelWithDebInfo, MinSizeRel, OMP
# OMP - the quickest multithreading BmnRoot version for a multicore machine
If(NOT CMAKE_BUILD_TYPE)
  Message("Set BuildType DEBUG")
  set(CMAKE_BUILD_TYPE Debug)
EndIf(NOT CMAKE_BUILD_TYPE)

include(CheckCompiler)
#Check the compiler and set the compile and link flags
Check_Compiler()

# Extract the FairRoot version from fairroot-config. The version info is of the form Major.Minor.Patch and is stored in the variable FairRoot_VERSION and FairSoft_VERSION
FairRootVersion()
FairSoftVersion()
Message(STATUS "FairRoot Version - ${FairRoot_VERSION}")
Message(STATUS "FairSoft Version - ${FairSoft_VERSION}")

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

if(SIMPATH)
  set(CMAKE_PREFIX_PATH ${SIMPATH} ${CMAKE_PREFIX_PATH})
  set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${SIMPATH}/lib)
endif()

# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is mandatory
find_package(ROOT 6.10.08 REQUIRED)
find_package(VMC)
if(ROOT_vmc_FOUND)
  set(VMCLIB ROOT::VMC)
else()
  set(VMCLIB VMCLibrary)
endif()
find_package(FairLogger REQUIRED)
find_package(Pythia8)
find_package(Pythia6)
find_package(Geant3)
find_package(GEANT4)
find_package(GEANT4DATA)
find_package(GEANT4VMC)
find_package(GSL)
find_package(CLHEP)
find_package(SSE)
find_package(PLUTO)
find_package(GENERATORS REQUIRED)
find_package(XML2 REQUIRED)
find_package(ZeroMQ REQUIRED)
find_package(CURL REQUIRED)
#find_package(TDAQ)
#find_package(CUDA)
#find_package(IWYU)
#find_package(Protobuf)
#find_package(DDS)

#find_package2(PUBLIC ROOT  VERSION 6.10.00  REQUIRED)
#if(ROOT_vmc_FOUND)
#  set(VMCLIB ROOT::VMC)
#else()
#  find_package2(PUBLIC VMC REQUIRED)
#  set(VMCLIB VMCLibrary)
#endif()
#find_package2(PUBLIC FairLogger  VERSION 1.2.0 REQUIRED)
#foreach(dep IN LISTS FairLogger_PACKAGE_DEPENDENCIES)
#if(NOT dep STREQUAL "Boost")
#  find_package2(PUBLIC ${dep} REQUIRED VERSION ${FairLogger_${dep}_VERSION})
#  set(PROJECT_${dep}_VERSION ${FairLogger_${dep}_VERSION})
#  if(dep STREQUAL "fmt") # handling of external fmt installation
#    get_target_property(FMT_INCLUDE_DIR fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
#    set(FairLogger_INCDIR ${FairLogger_INCDIR} ${FMT_INCLUDE_DIR})
#  endif()
#endif()
#endforeach()
#find_package2(PUBLIC Pythia6)
#find_package2(PUBLIC Pythia8)
#find_package2(PUBLIC Geant3)
#find_package2(PUBLIC Geant4)
#find_package2(PUBLIC Geant4VMC)
#find_package2(PUBLIC GSL)
#find_package2(PUBLIC Protobuf)
#find_package2(PUBLIC msgpack)
#find_package2(PUBLIC FlatBuffers)
#find_Package2(PUBLIC VGM)
#find_package2(PUBLIC IWYU)
if(NOT DEFINED GTEST_ROOT)
  set(GTEST_ROOT ${SIMPATH})
endif()
#find_package2(PUBLIC FairMQ VERSION 1.4.0)
#find_package2(PUBLIC DDS)
#find_package2(PUBLIC yaml-cpp)

IF (NOT ALIBUILD)
 If(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
   set(BOOST_ROOT ${SIMPATH})
   set(GSL_DIR ${SIMPATH})
 Else(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
   set(BOOST_ROOT ${SIMPATH}/basics/boost)
   set(GSL_DIR ${SIMPATH}/basics/gsl)
 EndIf(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
 
 if(NOT DEFINED Boost_NO_SYSTEM_PATHS)
  set(Boost_NO_SYSTEM_PATHS TRUE)
 endif()
 set(Boost_NO_BOOST_CMAKE TRUE)
ENDIF (NOT ALIBUILD)

Message("-- Looking for Boost ...")
if(Boost_NO_BOOST_CMAKE)
 # If an older version of boost is found both of the variables below are
 # cached and in a second cmake run, a good boost version is found even
 # if the version is to old.
 # To overcome this problem both variables are cleared before checking for boost.
 Unset(Boost_INCLUDE_DIR CACHE)
 Unset(Boost_LIBRARY_DIRS CACHE)
endif()

list(APPEND boost_dependencies filesystem serialization boost_program_options)

find_package(Boost 1.41)
#find_package(Boost 1.78)
#find_package2(PUBLIC Boost
#  VERSION 1.67
#  COMPONENTS ${boost_dependencies}
#  ADD_REQUIREMENTS_OF FairMQ
#)
If (Boost_FOUND)
  Set(Boost_Avail 1)
Else (Boost_FOUND)
  Set(Boost_Avail 0)
EndIf (Boost_FOUND)

# set a variable which should be used in all CMakeLists.txt
# Defines all basic include directories from fairbase
SetBasicVariables()
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

if(SIMPATH)
  set(ROOT_INCLUDE_DIR ${ROOT_INCLUDE_DIR} ${VMC_INCLUDE_DIRS})
  set(BASE_INCLUDE_DIRECTORIES ${BASE_INCLUDE_DIRECTORIES} ${FairLogger_INCDIR})
endif()

# Recurse into the given subdirectories. This does not actually cause another cmake executable to run
# The same process will walk through the project's entire directory structure
add_subdirectory (database)
add_subdirectory (field)
add_subdirectory (passive)
add_subdirectory (generators)
add_subdirectory (base)
add_subdirectory (detectors)
add_subdirectory (steering)
add_subdirectory (miscellaneous)
add_subdirectory (alignment)
add_subdirectory (decoder)
add_subdirectory (reconstruction/dst)
add_subdirectory (reconstruction/KF)
add_subdirectory (reconstruction/cat)
add_subdirectory (reconstruction/tracking)
add_subdirectory (reconstruction/globaltracking)
add_subdirectory (reconstruction/identification)
add_subdirectory (reconstruction/tof2)
add_subdirectory (reconstruction/zdc)
add_subdirectory (reconstruction/fhcal)
add_subdirectory (reconstruction/bmnrecotools)
add_subdirectory (physics)
add_subdirectory (monitor)
add_subdirectory (eventdisplay)
add_subdirectory (online)
add_subdirectory (QA)
add_subdirectory (macro)
add_subdirectory (services)
add_subdirectory (external)
add_subdirectory (analysis/common/analysis_tree_converter)

if(EXISTS "${CMAKE_SOURCE_DIR}/macro/nica_scheduler/CMakeLists.txt")
 add_subdirectory (macro/nica_scheduler)
endif()

Option(BUILD_DOXYGEN "Build Doxygen" OFF)
if(BUILD_DOXYGEN)
  MESSAGE(STATUS "*** Building the Doxygen documentation ***")
  add_subdirectory(doxygen)
endif(BUILD_DOXYGEN)

if(IWYU_FOUND)
  add_custom_target(checkHEADERS DEPENDS $ENV{ALL_HEADER_RULES})
endif(IWYU_FOUND)

#find_package2(PRIVATE ClangFormat)
#if(ClangFormat_FOUND)
#  add_custom_target(FormatCheck
#    ${CMAKE_COMMAND} -E env FAIRROOT_GIT_CLANG_FORMAT_BIN=${GIT_CLANG_FORMAT_BIN} ${CMAKE_SOURCE_DIR}/check-format.sh
#    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#  )
#endif()

WRITE_CONFIG_FILE(config.sh)
SET(VMCWORKDIR ${CMAKE_INSTALL_PREFIX})
SET(ROOT_INCLUDE_PATH ${CMAKE_INSTALL_PREFIX}/include ${ROOT_INCLUDE_PATH})
WRITE_CONFIG_FILE(install_config.sh)
