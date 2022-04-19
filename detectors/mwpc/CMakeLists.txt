# Create a library called "libMwpc"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/raw
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmndata/gem
 ${CMAKE_SOURCE_DIR}/bmndata/header
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
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
 BmnMwpcSegment.cxx
 BmnMwpcDigit.cxx
 BmnMwpcHit.cxx
 BmnMwpcHitProducer.cxx
 BmnMwpcHitFinder.cxx
 BmnMwpcGeometry.cxx
 BmnMwpcGeometrySRC.cxx
 BmnMwpcTrack.cxx
 BmnMwpcTrackFinder.cxx
 BmnMwpc.cxx
 BmnMwpcContFact.cxx
 BmnMwpcGeo.cxx
 BmnMwpcGeoPar.cxx
 BmnMwpcPoint.cxx
)

Set(HEADERS)
Set(LINKDEF MwpcLinkDef.h)
Set(LIBRARY_NAME Mwpc)
Set(DEPENDENCIES BmnData Gdml Base Core)

GENERATE_LIBRARY()
