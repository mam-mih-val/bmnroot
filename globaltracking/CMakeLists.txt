# Create a library called "libGlobalTracking"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/raw
 ${CMAKE_SOURCE_DIR}/bmndata/gem
 ${CMAKE_SOURCE_DIR}/bmndata/header
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/KF
 ${CMAKE_SOURCE_DIR}/KF/Interface
 ${CMAKE_SOURCE_DIR}/detectors/sts
 ${CMAKE_SOURCE_DIR}/bmndst
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/globaltracking
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
 BmnGlobalTrack.cxx
 BmnGlobalTracking.cxx
 BmnUpstreamTracking.cxx
 #BmnSRCTriggersCheck.cxx - compilation failed
 BmnTriggersCheck.cxx
 BmnVertexFinder.cxx
 SrcVertexFinder.cxx
 BmnGemDchTrack.cxx
)

Set(HEADERS)
Set(LINKDEF GlobalTrackingLinkDef.h)
Set(LIBRARY_NAME GlobalTracking)
Set(DEPENDENCIES Gem Silicon Mwpc Dch BmnData UniDb Base Core KF Sts BmnDst)

GENERATE_LIBRARY()
