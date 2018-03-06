# The name of our project is "BMNROOT".  CMakeLists files in this project can
# refer to the root source directory of the project as ${BMNROOT_SOURCE_DIR} 
# or as ${CMAKE_SOURCE_DIR} and to the root binary directory of the project as 
# ${BMNROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}.
# This difference is important for the base classes which are in FAIRROOT, 
# PANDAROOT, and BMNROOT.

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION  2.8.2 FATAL_ERROR)

### CMP0025   Compiler id for Apple Clang is now AppleClang.
### CMP0042   MACOSX_RPATH is enabled by default.

foreach(p
  CMP0025 # CMake 3.0
  CMP0042 # CMake 3.0
  )
  if(POLICY ${p})
  cmake_policy(SET ${p} NEW)
  endif()
endforeach()

# Set name of our project to "BMNROOT". Has to be done
# after check of cmake version                        
project(BMNROOT)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ 
# is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/cuda" ${CMAKE_MODULE_PATH})

# Load some basic macros which are needed later on
include(FairMacros)
include(WriteConfigFile)
include(CTest)
include(CheckCompiler)

# Set the build type. Possibilities are None, Debug, Release, 
# RelWithDebInfo and MinSizeRel 
#SET(CMAKE_BUILD_TYPE Release)
If(NOT CMAKE_BUILD_TYPE)
       Message("Set BuildType DEBUG")
       set(CMAKE_BUILD_TYPE Debug)
EndIf(NOT CMAKE_BUILD_TYPE)

#Check the compiler and set the compile and link flags
Check_Compiler()

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

Option(USE_PATH_INFO "Information from PATH and LD_LIBRARY_PATH are used." ON)

If(USE_PATH_INFO)
  Set(PATH $ENV{PATH})
  If (APPLE)
    Set(LD_LIBRARY_PATH $ENV{DYLD_LIBRARY_PATH})
  Else (APPLE)
    Set(LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
  EndIf (APPLE)
EndIf(USE_PATH_INFO)

# Check if the user wants to build the project in the source
# directory and if the install directory is different from the build
# directory
CHECK_OUT_OF_SOURCE_BUILD()
CHECK_INSTALL_DIRECTORY()

# Check if we are on an UNIX system. If not stop with an error
# message
IF(NOT UNIX)
  MESSAGE(FATAL_ERROR "You're not on an UNIX system. The project was up to now only tested on UNIX systems, so we break here. If you want to go on please edit the CMakeLists.txt in the source directory.")
ENDIF(NOT UNIX)  

#Check if necessary environment variables are set
#If not stop execution
IF(NOT DEFINED ENV{SIMPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
ENDIF(NOT DEFINED ENV{SIMPATH})
STRING(REGEX MATCHALL "[^:]+" PATH $ENV{PATH})
SET(SIMPATH $ENV{SIMPATH})

# Check if the external packages are installed into a separate install
# directory
CHECK_EXTERNAL_PACKAGE_INSTALL_DIR()

# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is
# mandatory
find_package(ROOT 5.34.28 REQUIRED)
find_package(Pythia6)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules_old" ${CMAKE_MODULE_PATH})
find_package(GEANT3 REQUIRED)
find_package(GEANT4)
find_package(GEANT4DATA)
find_package(GEANT4VMC)
find_package(CLHEP)
find_package(RuleChecker)
find_package(SSE)
find_package(PLUTO)
find_package(GENERATORS REQUIRED)
find_package(XML2 REQUIRED)
find_package(TDAQ)

#find_package(CUDA)
#find_package(IWYU)
find_package(ZeroMQ)
#find_package(Protobuf)
#find_package(DDS)

If(FAIRSOFT_EXTERN)
  set(BOOST_ROOT ${SIMPATH})
  set(GSL_DIR ${SIMPATH})
Else(FAIRSOFT_EXTERN)
  set(BOOST_ROOT ${SIMPATH}/basics/boost)
  set(GSL_DIR ${SIMPATH}/basics/gsl)
EndIf(FAIRSOFT_EXTERN)

Set(Boost_NO_SYSTEM_PATHS TRUE)
Set(Boost_NO_BOOST_CMAKE TRUE)
Message("-- Looking for Boost ...")
# If an older version of boost is found both of the variables below are
# cached and in a second cmake run, a good boost version is found even
# if the version is to old.
# To overcome this problem both variables are cleared before checking
# for boost.
Unset(Boost_INCLUDE_DIR CACHE)
Unset(Boost_LIBRARY_DIRS CACHE)
find_package(Boost 1.41 COMPONENTS thread system timer program_options random filesystem chrono exception regex)
If (Boost_FOUND)
  Set(Boost_Avail 1)
  Set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${Boost_LIBRARY_DIR})
Else (Boost_FOUND)
  Set(Boost_Avail 0)
EndIf (Boost_FOUND)

# Set the library version in the main CMakeLists.txt
SET(FAIRROOT_MAJOR_VERSION 15)
SET(FAIRROOT_MINOR_VERSION 11)
SET(FAIRROOT_PATCH_VERSION 0)
SET(FAIRROOT_VERSION "${FAIRROOT_MAJOR_VERSION}.${FAIRROOT_MINOR_VERSION}.${FAIRROOT_PATCH_VERSION}")

If(ROOT_FOUND_VERSION LESS 59999)
  SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
      VERSION "${FAIRROOT_VERSION}"
      SOVERSION "${FAIRROOT_MAJOR_VERSION}"
      SUFFIX ".so"
  )
Else()
  SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
      VERSION "${FAIRROOT_VERSION}"
      SOVERSION "${FAIRROOT_MAJOR_VERSION}"
  )
EndIf()

# Set some useful variables
SetBasicVariables()
SET(PATH ${EXECUTABLE_OUTPUT_PATH} ${PATH})
set(BMNROOT TRUE)
add_definitions(-DBMNROOT)

# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.
add_subdirectory (fairtools)
add_subdirectory (base)
add_subdirectory (geobase)
add_subdirectory (parbase)
add_subdirectory (generators)
add_subdirectory (geane)
add_subdirectory (passive)
add_subdirectory (trackbase)
add_subdirectory (MbsAPI)
add_subdirectory (datamatch)

#add_subdirectory (cuda)
add_subdirectory (bmndata)
add_subdirectory (bmnfield)
add_subdirectory (bd)
add_subdirectory (dch)
add_subdirectory (ecal)
add_subdirectory (gem)
add_subdirectory (land)
add_subdirectory (mwpc)
add_subdirectory (psd)
add_subdirectory (recoil)
add_subdirectory (silicon)
add_subdirectory (sts)
add_subdirectory (tof)
add_subdirectory (tof1)
add_subdirectory (zdc)
add_subdirectory (decoder)
add_subdirectory (QA)
add_subdirectory (KF)
add_subdirectory (cat)
add_subdirectory (alignment)
add_subdirectory (globaltracking)
add_subdirectory (eventdisplay)
add_subdirectory (uni_db)
add_subdirectory (macro)
#add_subdirectory (monitor)
add_subdirectory (physics/particles)

if(EXISTS "${CMAKE_SOURCE_DIR}/macro/mpd_scheduler/CMakeLists.txt")
 add_subdirectory (macro/mpd_scheduler)
endif()

Option(BUILD_DOXYGEN "Build Doxygen" OFF)
if(BUILD_DOXYGEN)
  MESSAGE(STATUS "*** Building the Doxygen documentaion ***")
  ADD_SUBDIRECTORY(doxygen)
endif(BUILD_DOXYGEN)

if(RULE_CHECKER_FOUND)
  ADD_CUSTOM_TARGET(RULES 
     COMMAND ${RULE_CHECKER_SCRIPT1} ${CMAKE_BINARY_DIR} viol > violations.html
     DEPENDS $ENV{ALL_RULES})
endif(RULE_CHECKER_FOUND)

WRITE_CONFIG_FILE(config.sh)
SET(ROOT_INCLUDE_PATH ${CMAKE_INSTALL_PREFIX}/include)
