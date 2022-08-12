# Create a library called "libSts"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/passive
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/sts
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
 #setup/CbmStsAddress.cxx
 #setup/CbmStsElement.cxx
 #setup/CbmStsModule.cxx
 #setup/CbmStsSenzor.cxx
 #setup/CbmStsSensorPoint.cxx
 #setup/CbmStsSensorType.cxx
 #setup/CbmStsSensorTypeDssd.cxx
 #setup/CbmStsSetup.cxx

 CbmStsDetectorId.cxx
 CbmStsDigi.cxx
 CbmStsDigiMatch.cxx
 CbmStsDigiLight.cxx
 CbmStsHit.cxx
 CbmStsCluster.cxx

 CbmGeoSts.cxx
 CbmGeoStsPar.cxx
 CbmSts.cxx
 CbmStsContFact.cxx
 CbmStsDigiPar.cxx
 CbmStsDigiScheme.cxx
 #CbmStsIdealDigitize.cxx
 CbmStsDigitize.cxx
 #CbmStsDigitizeTb.cxx
 #CbmStsIdealFindHits.cxx
 CbmStsClusterFinder.cxx
 CbmStsFindHits.cxx

 CbmStsFitTracks.cxx
 #CbmStsHitProducerIdeal.cxx
 #CbmStsIdealMatchHits.cxx
 CbmStsMatchHits.cxx
 CbmStsMatchTracks.cxx
 CbmStsParAsciiFileIo.cxx
 CbmStsParRootFileIo.cxx
 CbmStsRadTool.cxx
 CbmStsSensor.cxx
 CbmStsSensorDigiPar.cxx
 CbmStsSector.cxx
 CbmStsSectorDigiPar.cxx
 CbmStsStation.cxx
 CbmStsStationDigiPar.cxx
 CbmStsTrackFinderIdeal.cxx
 CbmStsTrackFitterIdeal.cxx
 CbmStsSimulationQa.cxx
 CbmStsFindHitsQa.cxx
 CbmStsReconstructionQa.cxx
 CbmBmnStsDigitize.cxx
 BmnGemFastDigitize.cxx
 BmnToCbmHitConverter.cxx
 BmnStsMatchTracks.cxx
)

Set(HEADERS)
Set(LINKDEF StsLinkDef.h)
Set(LIBRARY_NAME Sts)
Set(DEPENDENCIES Gem Silicon BmnBase Passive Base Gdml Core)

GENERATE_LIBRARY()
