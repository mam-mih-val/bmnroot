# Create a library called "libBmnData"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmndata/common
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/source
 ${CMAKE_SOURCE_DIR}/bmndata/header
 ${CMAKE_SOURCE_DIR}/bmndata/raw
 ${CMAKE_SOURCE_DIR}/bmndata/gem
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
 ${Boost_LIBRARY_DIRS}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 cbm/CbmStsPoint.cxx
 cbm/CbmDigi.cxx
 cbm/CbmBaseHit.cxx
 cbm/CbmHit.cxx
 cbm/CbmStripHit.cxx
 cbm/CbmVertex.cxx
 
 cbm/CbmStsTrack.cxx
 cbm/CbmStsTrackFinder.cxx
 cbm/CbmStsTrackFitter.cxx
 cbm/CbmTofMerger.cxx
 cbm/CbmTrackMerger.cxx
 cbm/CbmPrimaryVertexFinder.cxx
 cbm/CbmFindPrimaryVertex.cxx
 cbm/CbmL1Counters.h
 cbm/CbmTrackMatch.cxx
 cbm/CbmGlobalTrack.cxx

 cbm/CbmMvdDetectorId.cxx
 cbm/CbmMvdGeoPar.cxx
 cbm/CbmMvdPoint.cxx
 cbm/CbmMvdHit.cxx
 cbm/CbmMvdHitMatch.cxx

 cbm/CbmTofPoint.cxx
 cbm/CbmTofHit.cxx
 cbm/CbmTofTrack.cxx
 cbm/CbmGeoSttPar.cxx

 math/BmnMath.cxx
 math/BmnMatrixMath.cxx
 math/FitWLSQ.cxx
 math/BmnGeoNavigator.cxx
 math/BmnKalmanFilter.cxx
 math/BmnMaterialEffects.cxx
 math/BmnMaterialInfo.h

 common/BmnFunctionSet.cxx
 common/BmnCounterTask.cxx

 source/BmnFileSource.cxx
 source/BmnDecoSource.cxx

 header/BmnEventHeader.cxx
 header/BmnSpillHeader.cxx
 header/DigiRunHeader.cxx

 raw/RawTypes.h
 raw/BmnADCDigit.cxx
 raw/BmnTDCDigit.cxx
 raw/BmnTQDCADCDigit.cxx
 raw/BmnTTBDigit.cxx
 raw/BmnMSCDigit.cxx
 raw/BmnHRBDigit.cxx
 raw/BmnSyncDigit.cxx
 raw/BmnTrigDigit.cxx
 raw/BmnTrigWaveDigit.cxx
 raw/BmnTrigInfo.cxx
 raw/BmnTrigUnion.h
 raw/DigiArrays.h

 gem/BmnGemStripHit.cxx
 gem/BmnGemTrack.cxx

 BmnEnums.h
 BmnDetectorList.cxx
 CbmMCTrack.cxx
 CbmStack.cxx

 BmnStripData.h
 BmnStripDigit.cxx
 BmnCaloDigit.cxx
 BmnTacquilaDigit.cxx
 BmnDigiContainerTemplate.cxx
 BmnHit.cxx
 BmnLink.cxx
 BmnMatch.cxx
 BmnTrack.cxx
 BmnIdentifiableTrack.cxx
 BmnTrackMatch.cxx
 BmnVertex.cxx
 BmnEventQuality.cxx
 BmnFitNode.h
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
Set(DEPENDENCIES Base Core ${ZeroMQ_LIBRARY_SHARED} boost_serialization)

GENERATE_LIBRARY()
