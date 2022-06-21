# Create a library called "libGem"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/detectors/gem
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
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
  BmnGemStripMedium.cxx
  BmnGemStripTransform.cxx

  BmnGemStripConfiguration.h
  BmnGemStripMediumConfiguration.h

  BmnGemStripHit.cxx
  BmnGemTrack.cxx
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

Set(HEADERS)
set(LINKDEF GemLinkDef.h)
Set(LIBRARY_NAME Gem)
Set(DEPENDENCIES BmnBase GenVector UniDb Base XMLParser Core)

GENERATE_LIBRARY()
