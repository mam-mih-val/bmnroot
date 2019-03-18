# CMakeList file for library libSsd.
# Last update: D. Baranov, 04.11.2018


# -----   Specify library name   --------------------------
Set(LIBRARY_NAME BmnSsd)
# ---------------------------------------------------------


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

    aux/data/BmnSsdAddress.cxx
    aux/data/BmnModuleList.cxx
    aux/data/BmnAddress.cxx
    aux/data/BmnSsdCluster.cxx
    aux/data/BmnCluster.cxx
    aux/data/BmnSsdDigi.cxx
    aux/data/BmnDigi.cxx
    aux/data/BmnEvent.cxx
    aux/data/BmnVertex.cxx
    aux/data/BmnSsdPoint.cxx
    aux/data/BmnDigitize.cxx
    aux/data/BmnDaqBuffer.cxx
    aux/data/BmnSsdHit.cxx
    aux/data/BmnPixelHit.cxx
    aux/data/BmnBaseHit.cxx

    #BmnSSDHitProducer.cxx
)
# -----  End of sources   ---------------------------------



# ----  Specify include directories -----------------------
set(INCLUDE_DIRECTORIES
    ${BASE_INCLUDE_DIRECTORIES}
    ${CMAKE_SOURCE_DIR}/bmndata
    #${CMAKE_SOURCE_DIR}/bmndata/ssd

    ${CMAKE_SOURCE_DIR}/ssd
    ${CMAKE_SOURCE_DIR}/ssd/setup
    ${CMAKE_SOURCE_DIR}/ssd/mc
    ${CMAKE_SOURCE_DIR}/ssd/digitize
    ${CMAKE_SOURCE_DIR}/ssd/reco
    ${CMAKE_SOURCE_DIR}/ssd/aux
    ${CMAKE_SOURCE_DIR}/ssd/aux/data
)

set(SYSTEM_INCLUDE_DIRECTORIES
    ${BASE_INCLUDE_DIRECTORIES}
    ${Boost_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR}
)
# ----  End of include directories ------------------------


# ----  Specify link directories --------------------------
set(LINK_DIRECTORIES
    ${ROOT_LIBRARY_DIR}
    ${FAIRROOT_LIBRARY_DIR}
    ${Boost_LIBRARY_DIRS}
)
# -----  End of link directories   ------------------------


# -----   Specify library dependences   -------------------
Set(DEPENDENCIES Base)
# ---------------------------------------------------------


# -----  Specify LinkDef file   ---------------------------
set(LINKDEF ${LIBRARY_NAME}LinkDef.h)
# ---------------------------------------------------------


# -----  Let cmake do the job   ---------------------------
include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})
link_directories( ${LINK_DIRECTORIES})
GENERATE_LIBRARY()
# ---------------------------------------------------------
