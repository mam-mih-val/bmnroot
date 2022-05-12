# Create a library called "libDch"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/detectors/dch
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${Geant3_INCLUDE_DIRS}
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
 BmnDchDigit.cxx
 BmnDchHitOriginal.cxx
 BmnDchHit.cxx
 BmnDchHitProducer.cxx
 BmnDchTrackFinder.cxx
 BmnDchTrack.cxx

 DCHgeometryconstants_bmn.h
)

Set(HEADERS)
Set(LINKDEF DchLinkDef.h)
Set(LIBRARY_NAME Dch)
Set(DEPENDENCIES BmnBase Base Gdml Core)

GENERATE_LIBRARY()
