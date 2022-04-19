# Create a library called "libSiBT" (Silicon Beam Tracker)

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/raw
 ${CMAKE_SOURCE_DIR}/bmndata/header
 ${CMAKE_SOURCE_DIR}/detectors/SiBT
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
 BmnSiBT.cxx
 BmnSiBTPoint.cxx
 BmnSiBTStationSet.cxx
 BmnSiBTStation.cxx
 BmnSiBTModule.cxx
 BmnSiBTLayer.cxx
 BmnSiBTDigit.cxx
 BmnSiBTDigitizer.cxx
 BmnSiBTHit.cxx
 BmnSiBTHitMaker.cxx
 BmnSiBTTransform.cxx

 BmnSiBTConfiguration.h
)

Set(HEADERS)
Set(LINKDEF SiBTLinkDef.h)
Set(LIBRARY_NAME SiBT)
Set(DEPENDENCIES BmnData Base XMLParser Gdml Core)

GENERATE_LIBRARY()
