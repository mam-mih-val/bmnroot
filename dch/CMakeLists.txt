# Create a library called "libDch"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/dch
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${GEANT3_INCLUDE_DIR}
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
 BmnDch.cxx
 BmnDchPoint.cxx
 BmnDchContFact.cxx
 BmnDchGeo.cxx
 BmnDchGeoPar.cxx
 BmnDchHitProducer.cxx
 BmnDchTrackFinder.cxx
 BmnDchTrack.cxx

 DCHgeometryconstants_bmn.h
)

Set(HEADERS)
Set(LINKDEF DchLinkDef.h)
Set(LIBRARY_NAME Dch)
Set(DEPENDENCIES BmnData Base Core)

GENERATE_LIBRARY()
