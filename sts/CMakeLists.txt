# Create a library called "libSts" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/passive
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/sts
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
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

CbmGeoSts.cxx 
CbmGeoStsPar.cxx 
CbmSts.cxx 
CbmStsContFact.cxx 
CbmStsDigiPar.cxx 
CbmStsDigiScheme.cxx 
CbmStsIdealDigitize.cxx 
CbmStsDigitize.cxx 
CbmStsDigitizeTb.cxx 
CbmStsIdealFindHits.cxx 
CbmStsClusterFinder.cxx 
CbmStsFindHits.cxx 
CbmStsFindTracks.cxx 
CbmStsFindTracksQa.cxx 
CbmStsFitTracks.cxx 
CbmStsHitProducerIdeal.cxx 
CbmStsIdealMatchHits.cxx 
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
)

#AZ - debug
SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS
  "-g -O0 ")

Set(HEADERS)
Set(LINKDEF StsLinkDef.h)
Set(LIBRARY_NAME Sts)
Set(DEPENDENCIES Core)

GENERATE_LIBRARY()
