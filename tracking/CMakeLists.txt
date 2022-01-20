# Create a library called "libTracking"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/SiBT
 ${CMAKE_SOURCE_DIR}/steering
 ${CMAKE_SOURCE_DIR}/tracking
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
  BmnCellAutoTracking.cxx
  BmnInnerTrackingRun7.cxx
  SrcInnerTrackingRun7.cxx
  BmnBeamTracking.cxx
  BmnCellDuet.cxx
)

Set(HEADERS)
Set(LINKDEF TrackingLinkDef.h)
Set(LIBRARY_NAME Tracking)
Set(DEPENDENCIES Gem Silicon BmnSteering BmnData Base Core)

GENERATE_LIBRARY()
