# Create a library called "libGlobalTracking".

Set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/bmnfield
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/dch
${CMAKE_SOURCE_DIR}/mwpc
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
)

link_directories(${LINK_DIRECTORIES})

# List of source files
Set(SRCS
    BmnDetectorSetup.cxx
    BmnGlobalTrackingQA.cxx
    BmnManageQA.cxx
    BmnTrackFinder.cxx
    BmnHitToTrackMerger.cxx
    BmnKalmanFilter.cxx
    BmnTrackExtrapolator.cxx
    BmnTrackPropagator.cxx
    BmnMatrixMath.cxx
    BmnGeoNavigator.cxx
    BmnMaterialEffects.cxx
    BmnTrackSelector.cxx
    BmnGlobalTracking.cxx
    BmnHitMatchingQA.cxx
)

Set(HEADERS
    BmnDetectorSetup.h
    BmnGlobalTrackingQA.h
    BmnManageQA.h  
    BmnTrackFinder.h
    BmnHitToTrackMerger.h
    BmnKalmanFilter.h
    BmnTrackExtrapolator.h
    BmnTrackPropagator.h
    BmnMatrixMath.h
    BmnDefaultSettings.h
    BmnGeoNavigator.h
    BmnMaterialInfo.h
    BmnMaterialEffects.h
    BmnTrackSelector.h
    BmnGlobalTracking.h
    BmnHitMatchingQA.h
    BmnHitFinderRun1.h
)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

Set(LINKDEF GlobalTrackingLinkDef.h)
Set(LIBRARY_NAME GlobalTracking)
Set(DEPENDENCIES KF Sts BmnBase BmnData Base)

GENERATE_LIBRARY()

