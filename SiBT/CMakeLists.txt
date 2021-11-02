# Create a library called "libSiBT" (Silicon Beam Tracker)

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/SiBT
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
 BmnSiBT.cxx
 BmnSiBT.cxx
 BmnSiBTStationSet.cxx
 BmnSiBTStation.cxx
 BmnSiBTModule.cxx
 BmnSiBTLayer.cxx
 BmnSiBTDigitizer.cxx
 BmnSiBTHitMaker.cxx
 BmnSiBTTransform.cxx

 BmnSiBTConfiguration.h
)

Set(HEADERS)
Set(LINKDEF SiBTLinkDef.h)
Set(LIBRARY_NAME SiBT)
Set(DEPENDENCIES BmnData Base XMLParser Core)

GENERATE_LIBRARY()
