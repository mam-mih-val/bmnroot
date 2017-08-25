# Create a library called "libGem".

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${ROOT_INCLUDE_DIR}
${Boost_INCLUDE_DIRS}

${BMNROOT_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${BMNROOT_SOURCE_DIR}/parbase
${BMNROOT_SOURCE_DIR}/fairtools
${BMNROOT_SOURCE_DIR}/globaltracking
${BMNROOT_SOURCE_DIR}/gem
${BMNROOT_SOURCE_DIR}/bmnfield
${BMNROOT_SOURCE_DIR}/bmnbase
${BMNROOT_SOURCE_DIR}/BmnKalmanFilter

${BMNROOT_SOURCE_DIR}/bmndata
${BMNROOT_SOURCE_DIR}/alignment
${CMAKE_SOURCE_DIR}/uni_db
${BMNROOT_SOURCE_DIR}/uni_db/db_classes
#${BMNROOT_SOURCE_DIR}/sts
#${BMNROOT_SOURCE_DIR}/sts/setup

#${BMNROOT_SOURCE_DIR}/KF
#${BMNROOT_SOURCE_DIR}/KF/Interface
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
  BmnGemHit.cxx
  BmnGemTrack.cxx
  BmnGemHitProducer.cxx
  BmnGemGeo.cxx
  BmnGemDigit.cxx
  BmnGemDigitizerTask.cxx
  BmnGemGas.cxx
  BmnManageQA.cxx
  BmnGemDigitizerQAHistograms.cxx
  BmnGemTracking.cxx
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
  BmnGemStripHit.cxx
  BmnGemStripHitMaker.cxx
  BmnKalmanFilter_tmp.cxx
  BmnGemVertexFinder.cxx
)

set(HEADERS
  BmnGemHit.h
  BmnGemTrack.h
  BmnGemHitProducer.h
  BmnGemGeo.h
  BmnGemDigit.h
  BmnGemDigitizerTask.h
  BmnGemGas.h
  BmnManageQA.h
  BmnGemDigitizerQAHistograms.h
  BmnGemTracking.h
  BmnGemStripLayer.h
  BmnGemStripModule.h
  BmnGemStripStation.h
  BmnGemStripStationSet.h
  BmnGemStripStation_RunSummer2016.h
  BmnGemStripStationSet_RunSummer2016.h
  BmnGemStripStation_RunWinter2016.h
  BmnGemStripStationSet_RunWinter2016.h
  BmnGemStripStation_RunSpring2017.h
  BmnGemStripStationSet_RunSpring2017.h
  BmnGemStripDigit.h
  BmnGemStripDigitizer.h
  BmnGemStripHit.h
  BmnGemStripHitMaker.h
  BmnKalmanFilter_tmp.h
  BmnGemVertexFinder.h
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

set(LINKDEF  GemLinkDef.h)
Set(LIBRARY_NAME Gem)
Set(DEPENDENCIES
    KF Sts BmnBase BmnData Base
)

GENERATE_LIBRARY()

