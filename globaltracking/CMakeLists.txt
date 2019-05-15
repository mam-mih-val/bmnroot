# Create a library called "libGlobalTracking".

Set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/dch
 ${CMAKE_SOURCE_DIR}/mwpc
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/globaltracking
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
Set(SRCS
 BmnDetectorSetup.cxx
 BmnGlobalTracking.cxx
 #BmnSRCTriggersCheck.cxx
 BmnTriggersCheck.cxx
 BmnVertexFinder.cxx
)

Set(HEADERS
 BmnDetectorSetup.h
 BmnGlobalTracking.h
 #BmnSRCTriggersCheck.h - compilation failed
 BmnTriggersCheck.h
 BmnHitFinderRun1.h
 BmnVertexFinder.h
)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

Set(LINKDEF GlobalTrackingLinkDef.h)
Set(LIBRARY_NAME GlobalTracking)
Set(DEPENDENCIES Gem KF Sts BmnData Base)

GENERATE_LIBRARY()

