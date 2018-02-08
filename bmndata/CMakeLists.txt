# Create a library called "libBmnData" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/globaltracking
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/silicon
${CMAKE_SOURCE_DIR}/uni_db
${CMAKE_SOURCE_DIR}/uni_db/db_classes
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR} 
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
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
BmnTrack.cxx
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
BmnDchHitOriginal.cxx
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
BmnTrigDigit.cxx
BmnTrigWaveDigit.cxx
BmnDchDigit.cxx
BmnEcalDigit.cxx
BmnADCDigit.cxx
BmnTacquilaDigit.cxx
BmnADCSRCDigit.cxx
BmnTDCDigit.cxx
BmnHRBDigit.cxx
BmnSyncDigit.cxx
BmnTTBDigit.cxx
BmnMSCDigit.cxx
BmnDchRaw2Digit.cxx
BmnMwpcRaw2Digit.cxx
BmnZDCRaw2Digit.cxx
BmnECALRaw2Digit.cxx
BmnLANDRaw2Digit.cxx
BmnGemRaw2Digit.cxx
BmnTrigRaw2Digit.cxx
BmnZDCDigit.cxx
BmnECALDigit.cxx
BmnLANDDigit.cxx
BmnTof2Raw2Digit.cxx
BmnTof2Raw2DigitNew.cxx
BmnTof1Raw2Digit.cxx
BmnRawDataDecoder.cxx
BmnEventHeader.cxx
BmnRunHeader.cxx
BmnCounterTask.cxx
BmnAdcProcessor.cxx
BmnIdentifiableTrack.cxx
BmnFitNode.h
BmnEnums.h
DigiArrays.h
BmnEventQuality.cxx
BmnTriggersCheck.cxx
)

Set(HEADERS)
Set(LINKDEF BmnDataLinkDef.h)
Set(LIBRARY_NAME BmnData)
Set(DEPENDENCIES Core Base)

GENERATE_LIBRARY()
