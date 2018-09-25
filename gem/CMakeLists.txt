# Create a library called "libGem".

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/alignment
 ${CMAKE_SOURCE_DIR}/globaltracking
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/steering
 ${CMAKE_SOURCE_DIR}/gem
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
  BmnGemHitProducer.cxx
  BmnGemGeo.cxx
  BmnGemDigit.cxx
  BmnGemGas.cxx
  BmnManageQA.cxx
  BmnGemDigitizerQAHistograms.cxx
  BmnGemTracking.cxx
  BmnCellAutoTracking.cxx
  BmnCellDuet.cxx
  BmnGemStripLayer.cxx
  BmnGemStripModule.cxx
  BmnGemStripStation.cxx
  BmnGemStripStationSet.cxx
  BmnGemStripStation_RunSummer2016.cxx
  BmnGemStripStationSet_RunSummer2016.cxx
  BmnGemStripStation_RunWinter2016.cxx
  BmnGemStripStationSet_RunWinter2016.cxx
  BmnGemStripStation_RunSpring2017.cxx
  BmnGemStripStationSet_RunSpring2017.cxx
  BmnGemStripDigit.cxx
  BmnGemStripDigitizer.cxx
  BmnGemStripHitMaker.cxx
#  BmnKalmanFilter.cxx
  BmnGemStripMedium.cxx
  #BmnMaterialEffects.cxx
  BmnGemStripTransform.cxx
)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3 -w")
   Message(STATUS "Gem will be compiled with SSE support")
#  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-msse -g -w")
#  Message(STATUS "Gem will be compiled with SSE support in DEBUG mode without optimization")
ELSE (SSE_FOUND)
  Message(STATUS "Gem will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-O3 -w")
ENDIF (SSE_FOUND)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "-std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "-std=c++0x")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

Set(HEADERS)
set(LINKDEF GemLinkDef.h)
Set(LIBRARY_NAME Gem)
Set(DEPENDENCIES KF Sts BmnData Base GenVector)

GENERATE_LIBRARY()
