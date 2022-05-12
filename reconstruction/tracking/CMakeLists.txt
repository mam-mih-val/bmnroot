# Create a library called "libBmnTracking"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/SiBT
 ${CMAKE_SOURCE_DIR}/detectors/sts
 ${CMAKE_SOURCE_DIR}/steering
 ${CMAKE_SOURCE_DIR}/reconstruction/KF
 ${CMAKE_SOURCE_DIR}/reconstruction/KF/Interface
 ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
 ${CMAKE_SOURCE_DIR}/reconstruction/tracking
 ${CMAKE_SOURCE_DIR}/reconstruction/tracking/sts
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
  sts/BmnStsVectorFinder.cxx
  
  BmnCellAutoTracking.cxx
  BmnInnerTrackingRun7.cxx
  SrcInnerTrackingRun7.cxx
  BmnBeamTracking.cxx
  BmnCellDuet.cxx
)

Set(HEADERS)
Set(LINKDEF TrackingLinkDef.h)
Set(LIBRARY_NAME BmnTracking)
Set(DEPENDENCIES BmnGlobalTracking KF Sts Gem Silicon BmnSteering BmnField BmnBase Base Core)

GENERATE_LIBRARY()
