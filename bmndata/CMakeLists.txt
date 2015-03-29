# Create a library called "libBmnData" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/globaltracking
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(BMNDATA_SRCS
CbmBaseHit.cxx
CbmHit.cxx
CbmPixelHit.cxx
CbmDetectorList.cxx
CbmStack.cxx
CbmMCTrack.cxx
CbmStsPoint.cxx
CbmStsDetectorId.cxx
CbmStsDigi.cxx
CbmStsDigiMatch.cxx
CbmMCBuffer.cxx
CbmMvdPoint.cxx
CbmMvdDetectorId.cxx
CbmTofPoint.cxx
CbmTofHit.cxx         
CbmTofTrack.cxx     
CbmEcalPoint.cxx
CbmDaqBuffer.cxx
CbmDigi.cxx
CbmStsDigiLight.cxx
CbmStsHit.cxx
CbmStsCluster.cxx
CbmStsTrack.cxx
CbmTrackMatch.cxx
CbmTrack.cxx
CbmMvdHit.cxx
CbmMvdHitMatch.cxx
CbmVertex.cxx
CbmGlobalTrack.cxx
CbmPsdPoint.cxx
CbmPsdHit.cxx
CbmPsdDigi.cxx
CbmPsdEventData.cxx
CbmMvdGeoPar.cxx
CbmGeoSttPar.cxx
BmnDchHit.cxx
BmnGlobalTrack.cxx
BmnMwpcHit.cxx
BmnTofHit.cxx
BmnLink.cxx
BmnMatch.cxx
BmnTrackMatch.cxx
BmnHit.cxx
BmnMwpcDigit.cxx
BmnTof1Digit.cxx
BmnTof2Digit.cxx
BmnDchDigit.cxx
BmnEcalDigit.cxx
BmnADCDigit.cxx
BmnTDCDigit.cxx
BmnSyncDigit.cxx
BmnTTBDigit.cxx
BmnDchRaw2Digit.cxx
BmnZDCRaw2Digit.cxx
BmnZDCDigit.cxx
BmnTof2Raw2Digit.cxx
BmnTof1Raw2Digit.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h BMNDATA_HEADERS "${BMNDATA_SRCS}")

set(BMNDATA_LINKDEF BmnDataLinkDef.h)
set(BMNDATA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnDataDict.cxx) 

ROOT_GENERATE_DICTIONARY("${BMNDATA_HEADERS}" "${BMNDATA_LINKDEF}" "${BMNDATA_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(BMNDATA_SRCS ${BMNDATA_SRCS} ${BMNDATA_DICTIONARY})


add_library(BmnData SHARED ${BMNDATA_SRCS})
target_link_libraries(BmnData ${ROOT_LIBRARIES})
set_target_properties(BmnData PROPERTIES VERSION 0.0.1 SOVERSION 0)

################ install ###################
install(TARGETS BmnData DESTINATION ${CMAKE_BINARY_DIR}/lib)
