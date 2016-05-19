# Create a library called "libGem".

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${ROOT_INCLUDE_DIR}
${Boost_INCLUDE_DIR}

${BMNROOT_SOURCE_DIR}/base
${BMNROOT_SOURCE_DIR}/parbase
${BMNROOT_SOURCE_DIR}/fairtools
${BMNROOT_SOURCE_DIR}/globaltracking
${BMNROOT_SOURCE_DIR}/gem
${BMNROOT_SOURCE_DIR}/bmnfield
${BMNROOT_SOURCE_DIR}/bmnbase
${BMNROOT_SOURCE_DIR}/BmnKalmanFilter

${BMNROOT_SOURCE_DIR}/bmndata
#${BMNROOT_SOURCE_DIR}/sts
#${BMNROOT_SOURCE_DIR}/sts/setup

#${BMNROOT_SOURCE_DIR}/KF
#${BMNROOT_SOURCE_DIR}/KF/Interface
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
  BmnGemHit.cxx
  BmnGemTrack.cxx
  BmnGemHitProducer.cxx
  BmnGemGeo.cxx
  BmnGemDigit.cxx
  BmnGemDigitizerTask.cxx
  BmnGemGas.cxx
  BmnManageQA.cxx
  BmnGemDigitizerQAHistograms.cxx
  BmnGemSeedFinder.cxx
  BmnGemSeedFinderQA.cxx
  BmnGemTrackFinderQA.cxx
  BmnGemTrackFinder.cxx
  BmnGemStripReadoutModule.cxx
  BmnGemStripStation.cxx
  BmnGemStripStationSet.cxx
  BmnGemStripStation_FullConfig.cxx
  BmnGemStripStationSet_FullConfig.cxx
  BmnGemStripStation_1stConfig.cxx
  BmnGemStripStationSet_1stConfig.cxx
  BmnGemStripStation_1stConfigShort.cxx
  BmnGemStripStationSet_1stConfigShort.cxx
  BmnGemStripStation_2ndConfig.cxx
  BmnGemStripStationSet_2ndConfig.cxx
  BmnGemStripDigit.cxx
  BmnGemStripDigitizer.cxx
  BmnGemStripHit.cxx
  BmnGemStripHitMaker.cxx
  BmnKalmanFilter_tmp.cxx
  BmnGemVertexFinder.cxx
)

set(HEADERS
  BmnGemHit.h
  BmnGemTrack.h
  BmnGemHitProducer.h
  BmnGemGeo.h
  BmnGemDigit.h
  BmnGemDigitizerTask.h
  BmnGemGas.h
  BmnManageQA.h
  BmnGemDigitizerQAHistograms.h
  BmnGemSeedFinder.h
  BmnGemSeedFinderQA.h
  BmnGemTrackFinderQA.h
  BmnGemTrackFinder.h
  BmnGemStripReadoutModule.h
  BmnGemStripStation.h
  BmnGemStripStationSet.h
  BmnGemStripStation_FullConfig.h
  BmnGemStripStationSet_FullConfig.h
  BmnGemStripStation_1stConfig.h
  BmnGemStripStationSet_1stConfig.h
  BmnGemStripStation_1stConfigShort.h
  BmnGemStripStationSet_1stConfigShort.h
  BmnGemStripStation_2ndConfig.h
  BmnGemStripStationSet_2ndConfig.h
  BmnGemStripDigit.h
  BmnGemStripDigitizer.h
  BmnGemStripHit.h
  BmnGemStripHitMaker.h
  BmnKalmanFilter_tmp.h
  BmnGemVertexFinder.h
)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3 -w")
  Message(STATUS "Gem will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "Gem will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-O3 -w")
ENDIF (SSE_FOUND)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

set(LINKDEF  GemLinkDef.h)
Set(LIBRARY_NAME Gem)
Set(DEPENDENCIES
    KF Sts BmnBase BmnData Base
)

GENERATE_LIBRARY()

