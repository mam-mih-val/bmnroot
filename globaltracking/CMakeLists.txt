# Create a library called "libGlobalTracking"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/dch
 ${CMAKE_SOURCE_DIR}/mwpc
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/KF
 ${CMAKE_SOURCE_DIR}/KF/Interface
 ${CMAKE_SOURCE_DIR}/sts
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
 BmnGlobalTracking.cxx
 BmnUpstreamTracking.cxx
 #BmnSRCTriggersCheck.cxx - compilation failed
 BmnTriggersCheck.cxx
 BmnVertexFinder.cxx
 SrcVertexFinder.cxx
)

Set(HEADERS)
Set(LINKDEF GlobalTrackingLinkDef.h)
Set(LIBRARY_NAME GlobalTracking)
Set(DEPENDENCIES Gem Silicon Mwpc Dch BmnData UniDb Base Core KF Sts BmnDst)

GENERATE_LIBRARY()
