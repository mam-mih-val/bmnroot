# Create a library called "libBmnData" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/globaltracking
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
 ${ZeroMQ_INCLUDE_DIR} 
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
 math/BmnGeoNavigator.cxx
 math/BmnKalmanFilter.cxx
 math/BmnMaterialEffects.cxx
 math/BmnMaterialInfo.h

 CbmDetectorList.cxx
 CbmMCBuffer.cxx
 CbmMCPointBuffer.h
 CbmDaqBuffer.cxx
 CbmMCTrack.cxx
 CbmStack.cxx
 CbmDigi.cxx
 CbmBaseHit.cxx
 CbmHit.cxx
 CbmPixelHit.cxx
 CbmTrackMatch.cxx
 CbmGlobalTrack.cxx

 CbmStsDetectorId.cxx
 CbmStsPoint.cxx
 CbmStsDigi.cxx
 CbmStsDigiMatch.cxx
 CbmStsDigiLight.cxx
 CbmStsHit.cxx
 CbmStsCluster.cxx
 CbmStsTrack.cxx
 CbmMvdDetectorId.cxx
 CbmMvdGeoPar.cxx
 CbmMvdPoint.cxx
 CbmMvdHit.cxx
 CbmMvdHitMatch.cxx
 CbmTofPoint.cxx
 CbmTofHit.cxx
 CbmTofTrack.cxx
 CbmEcalPoint.cxx
 CbmPsdPoint.cxx
 CbmPsdDigi.cxx
 CbmPsdHit.cxx
 CbmPsdEventData.cxx
 CbmGeoSttPar.cxx

 BmnEnums.h
 BmnStripData.h
 BmnHit.cxx
 BmnLink.cxx
 BmnMatch.cxx
 BmnTrack.cxx
 BmnTrackMatch.cxx
 BmnGlobalTrack.cxx
 BmnIdentifiableTrack.cxx
 BmnEventHeader.cxx
 BmnEventQuality.cxx
 BmnRunHeader.cxx
 BmnFileSource.cxx
 BmnDecoSource.cxx
 BmnFitNode.h
 BmnCounterTask.cxx

 BmnTrigInfo.cxx
 BmnTrigDigit.cxx
 BmnTrigWaveDigit.cxx
 BmnADCDigit.cxx
 BmnTDCDigit.cxx
 BmnTacquilaDigit.cxx
 BmnTQDCADCDigit.cxx
 BmnHRBDigit.cxx
 BmnSyncDigit.cxx
 BmnTTBDigit.cxx
 BmnMSCDigit.cxx
 DigiArrays.h

 BmnSiliconHit.cxx
 BmnGemStripHit.cxx
 BmnGemTrack.cxx
 BmnDchDigit.cxx
 BmnDchHitOriginal.cxx
 BmnDchHit.cxx
 BmnCscDigit.cxx
 BmnMwpcDigit.cxx
 BmnMwpcHit.cxx
 BmnTof1Digit.cxx
 BmnTof2Digit.cxx
 BmnTofHit.cxx
 BmnECALDigit.cxx
 BmnZDCDigit.cxx
 BmnLANDDigit.cxx

#BmnVertex.cxx
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
Set(DEPENDENCIES Minuit Core Base ${ZeroMQ_LIBRARY_SHARED})

GENERATE_LIBRARY()
