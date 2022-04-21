# Create a library called "libBmnSsd"
# Last update: D. Baranov, 04.11.2018

# -----   Specify library name   --------------------------
Set(LIBRARY_NAME BmnSsd)
# ---------------------------------------------------------

# ----  Specify include directories -----------------------
set(INCLUDE_DIRECTORIES
    ${BASE_INCLUDE_DIRECTORIES}
    ${CMAKE_SOURCE_DIR}/base
    ${CMAKE_SOURCE_DIR}/detectors/ssd
    ${CMAKE_SOURCE_DIR}/detectors/ssd/setup
    ${CMAKE_SOURCE_DIR}/detectors/ssd/mc
    ${CMAKE_SOURCE_DIR}/detectors/ssd/digitize
    ${CMAKE_SOURCE_DIR}/detectors/ssd/reco
    ${CMAKE_SOURCE_DIR}/detectors/ssd/auxx
    ${CMAKE_SOURCE_DIR}/detectors/ssd/auxx/data
)

set(SYSTEM_INCLUDE_DIRECTORIES
    ${ROOT_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)
# ----  End of include directories ------------------------

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})


# ----  Specify link directories --------------------------
set(LINK_DIRECTORIES
    ${ROOT_LIBRARY_DIR}
    ${FAIRROOT_LIBRARY_DIR}
    ${Boost_LIBRARY_DIRS}
)
# -----  End of link directories   ------------------------

link_directories(${LINK_DIRECTORIES})


# -----  Specify sources for compilation   ----------------
# --- Sources in setup
set(SRCS_SETUP
    setup/BmnSsdElement.cxx
    setup/BmnSsdModule.cxx
    setup/BmnSsdSensor.cxx
    setup/BmnSsdSensorConditions.cxx
    setup/BmnSsdSensorPoint.cxx
    setup/BmnSsdSetup.cxx
    setup/BmnSsdStation.cxx
)
# --- Sources in mc
set(SRCS_MC
    #mc/BmnSsdAcceptance.cxx
    mc/BmnSsdMC.cxx
    mc/BmnSsdTrackStatus.h
    #mc/BmnSsdSensorFactory.cxx
    #mc/BmnSsdMCQa.cxx
)
# --- Sources in digitize
set (SRCS_DIGITIZE
    digitize/BmnSsdDigitize.cxx
    #digitize/BmnSsdDigitizeQa.cxx
    #digitize/BmnSsdDigitizeQaReport.cxx
    digitize/BmnSsdDigitizeParameters.cxx
    digitize/BmnSsdPhysics.cxx
    digitize/BmnSsdSensorDssd.cxx
    #digitize/BmnSsdSensorDssdOrtho.cxx
    digitize/BmnSsdSensorDssdStereo.cxx
    digitize/BmnSsdSignal.cxx;
)
# --- Sources in reco
set (SRCS_RECO
    #reco/BmnSsdClusterAnalysis.cxx
    #reco/BmnSsdClusterFinderModule.cxx
    #reco/BmnSsdFindClusters.cxx
    #reco/BmnSsdFindHits.cxx
    #reco/BmnSsdFindHitsEvents.cxx
    #reco/BmnSsdFindTracksEvents.cxx
    #reco/BmnSsdMatchReco.cxx
    #reco/BmnSsdRecoQa.cxx
    #reco/BmnSsdTestQa.cxx
    #reco/BmnSsdTimeBasedQa.cxx
    #reco/BmnSsdTimeBasedQaReport.cxx
)
# --- All sources
set(SRCS
    ${SRCS_SETUP}
    ${SRCS_MC}
    ${SRCS_DIGITIZE}
    ${SRCS_RECO}
    #BmnSsdContFact.cxx
    #BmnSsdFindTracks.cxx
    #BmnSsdFindTracksQa.cxx
    #BmnSsdFitTracks.cxx
    #BmnSsdHitProducerIdeal.cxx
    #BmnSsdMatchTracks.cxx
    #BmnSsdRadTool.cxx
    #BmnSsdTrackFinderIdeal.cxx
    #BmnSsdTrackFitterIdeal.cxx
    #BmnSsdSimulationQa.cxx

    auxx/data/BmnSsdAddress.cxx
    auxx/data/BmnModuleList.cxx
    auxx/data/BmnAddress.cxx
    auxx/data/BmnSsdCluster.cxx
    auxx/data/BmnCluster.cxx
    auxx/data/BmnSsdDigi.cxx
    auxx/data/BmnDigi.cxx
    auxx/data/BmnEvent.cxx
    auxx/data/BmnVertex.cxx
    auxx/data/BmnSsdPoint.cxx
    auxx/data/BmnDigitize.cxx
    auxx/data/BmnDaqBuffer.cxx
    auxx/data/BmnSsdHit.cxx
    auxx/data/BmnPixelHit.cxx
    auxx/data/BmnBaseHit.cxx

)
# -----  End of sources   ---------------------------------

# -----  Specify LinkDef file   ---------------------------
set(LINKDEF ${LIBRARY_NAME}LinkDef.h)
# ---------------------------------------------------------

# -----   Specify library dependences   -------------------
Set(DEPENDENCIES BmnBase Base Core boost_serialization)
# ---------------------------------------------------------

# -----  Let cmake do the job   ---------------------------
GENERATE_LIBRARY()
# ---------------------------------------------------------
