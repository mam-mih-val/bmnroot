# Create a library called "libSts" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/sts
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS_SETUP
#setup/CbmStsAddress.cxx
#setup/CbmStsElement.cxx
#setup/CbmStsModule.cxx
#setup/CbmStsSenzor.cxx
#setup/CbmStsSensorPoint.cxx
#setup/CbmStsSensorType.cxx
#setup/CbmStsSensorTypeDssd.cxx
#setup/CbmStsSetup.cxx
)


set(STS_SRCS
${SRCS_SETUP}
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
SET_SOURCE_FILES_PROPERTIES(${STS_SRCS} PROPERTIES COMPILE_FLAGS
  "-g -O0 ")

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h STS_HEADERS "${STS_SRCS}")

set(STS_LINKDEF StsLinkDef.h)
set(STS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnStsDict.cxx) 

ROOT_GENERATE_DICTIONARY("${STS_HEADERS}" "${STS_LINKDEF}" "${STS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(STS_SRCS ${STS_SRCS} ${STS_DICTIONARY})


add_library(Sts SHARED ${STS_SRCS})
target_link_libraries(Sts ${ROOT_LIBRARIES})
set_target_properties(Sts PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Sts DESTINATION ${CMAKE_BINARY_DIR}/lib)
