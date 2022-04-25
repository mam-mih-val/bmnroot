# Create a library called "libCSC"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/detectors/csc
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
 BmnCSC.cxx
 BmnCSCPoint.cxx
 BmnCSCStationSet.cxx
 BmnCSCStation.cxx
 BmnCSCModule.cxx
 BmnCSCLayer.cxx
 BmnCSCDigit.cxx
 BmnCSCDigitizer.cxx
 BmnCSCHit.cxx
 BmnCSCHitMaker.cxx
 BmnCSCTransform.cxx

 #BmnCSCDigit.cxx
 #BmnCSCHit.cxx

 BmnCSCConfiguration.h
)

Set(HEADERS)
Set(LINKDEF CSCLinkDef.h)
Set(LIBRARY_NAME CSC)
Set(DEPENDENCIES BmnBase Base XMLParser Gdml GenVector Core)

GENERATE_LIBRARY()
