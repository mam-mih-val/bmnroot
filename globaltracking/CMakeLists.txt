# Create a library called "libGlobalTracking".

Set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${ROOT_INCLUDE_DIR}
${Boost_INCLUDE_DIRS}

${BMNROOT_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${BMNROOT_SOURCE_DIR}/parbase
${BMNROOT_SOURCE_DIR}/fairtools
${BMNROOT_SOURCE_DIR}/bmnbase
${BMNROOT_SOURCE_DIR}/bmndata
${BMNROOT_SOURCE_DIR}/bmnfield
${BMNROOT_SOURCE_DIR}/gem
${BMNROOT_SOURCE_DIR}/globaltracking
#${BMNROOT_SOURCE_DIR}/sts
#${BMNROOT_SOURCE_DIR}/sts/setup

#${BMNROOT_SOURCE_DIR}/KF
#${BMNROOT_SOURCE_DIR}/KF/Interface
)

include_directories( ${INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

Set(SRCS
    BmnDetectorSetup.cxx
    BmnGlobalTrackingQA.cxx
    BmnManageQA.cxx
    #BmnFindGlobalTracks.cxx
    BmnTrackFinder.cxx
    BmnHitToTrackMerger.cxx
    BmnKalmanFilter.cxx
    BmnTrackExtrapolator.cxx
    BmnTrackPropagator.cxx
    #BmnTrackFitter.cxx
    BmnMath.cxx
    BmnMatrixMath.cxx
    BmnTrackingGeometryConstructor.cxx
    BmnGeoNavigator.cxx
    BmnMaterialEffects.cxx
    BmnTrackSelector.cxx
    BmnGlobalTracking.cxx
    BmnHitMatchingQA.cxx
    #BmnSeedFinder.cxx
)

Set(HEADERS
    BmnDetectorSetup.h
    BmnGlobalTrackingQA.h
    BmnManageQA.h
    #BmnFindGlobalTracks.h    
    BmnTrackFinder.h
    BmnHitToTrackMerger.h
    BmnKalmanFilter.h
    BmnTrackExtrapolator.h
    BmnTrackPropagator.h
    #BmnTrackFitter.h
    BmnMath.h
    BmnMatrixMath.h
    BmnDefaultSettings.h
    BmnTrackingGeometryConstructor.h
    BmnGeoNavigator.h
    BmnMaterialInfo.h
    BmnMaterialEffects.h
    BmnTrackSelector.h
    BmnGlobalTracking.h
    BmnHitMatchingQA.h
    #BmnSeedFinder.h
    BmnHitFinderRun1.h
    BmnTrackFinderRun1.h
)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

Set(LINKDEF  GlobalTrackingLinkDef.h)
Set(LIBRARY_NAME GlobalTracking)
Set(DEPENDENCIES
    KF Sts BmnBase BmnData Base
)

GENERATE_LIBRARY()

