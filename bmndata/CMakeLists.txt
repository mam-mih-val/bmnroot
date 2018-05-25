# Create a library called "libBmnData" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/globaltracking
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/bmndata
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
CbmMvdHit.cxx
CbmMvdHitMatch.cxx
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
BmnTrackMatch.cxx
BmnMwpcDigit.cxx
BmnTof1Digit.cxx
BmnTof2Digit.cxx
BmnTrigDigit.cxx
BmnTrigWaveDigit.cxx
BmnDchDigit.cxx
BmnEcalDigit.cxx
BmnADCDigit.cxx
BmnTacquilaDigit.cxx
BmnTQDCADCDigit.cxx
BmnTDCDigit.cxx
BmnHRBDigit.cxx
BmnSyncDigit.cxx
BmnTTBDigit.cxx
BmnMSCDigit.cxx
BmnZDCDigit.cxx
BmnECALDigit.cxx
BmnLANDDigit.cxx
BmnCscDigit.cxx
BmnEventHeader.cxx
BmnRunHeader.cxx
BmnCounterTask.cxx
BmnIdentifiableTrack.cxx
BmnFitNode.h
BmnEnums.h
DigiArrays.h
BmnEventQuality.cxx
BmnGemStripHit.cxx
BmnGemTrack.cxx
 cbm/CbmVertex.cxx
 cbm/CbmStsTrackFinder.cxx
 cbm/CbmStsTrackFitter.cxx
 cbm/CbmTofMerger.cxx
 cbm/CbmTrackMerger.cxx
 cbm/CbmPrimaryVertexFinder.cxx
 cbm/CbmFindPrimaryVertex.cxx
 math/BmnMath.cxx
 math/BmnMatrixMath.cxx
 math/FitWLSQ.cxx
 BmnFileSource.cxx
 BmnGeoNavigator.cxx
 BmnTrack.cxx
 BmnHit.cxx
 BmnLink.cxx
 BmnMatch.cxx
BmnTrigInfo.cxx
)

Set(HEADERS)

EXEC_PROGRAM( ${ROOT_CONFIG_EXECUTABLE} 
    ARGS "--has-gdml"
    OUTPUT_VARIABLE ROOT_HAS_GDML )

If ( ${ROOT_HAS_GDML} STREQUAL yes )
  Message(STATUS "Build CbmModule with Gdml support")
  Add_Definitions(-DROOT_HAS_GDML)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

If ( ${ROOT_HAS_GDML} STREQUAL yes )
 Set (DEPENDENCIES ${DEPENDENCIES} Gdml)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

Set(LINKDEF BmnDataLinkDef.h)
Set(LIBRARY_NAME BmnData)
Set(DEPENDENCIES Minuit Core Base)

GENERATE_LIBRARY()
