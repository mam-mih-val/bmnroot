# Create a library called "libMwpc"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/mwpc
)

Set(SYSTEM_INCLUDE_DIRECTORIES
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
BmnMwpcSegment.cxx
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
Set(DEPENDENCIES BmnData Base Core)

GENERATE_LIBRARY()
