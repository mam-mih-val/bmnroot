# Create a library called "libBmnBase"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/base/source
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/services
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${Boost_INCLUDE_DIRS}
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
 math/BmnProfRawTools.cxx

 source/BmnFileSource.cxx
 source/BmnDecoSource.cxx
 source/BmnProfilometerSource.cxx
 source/BmnOnlineShmSource.cxx

 header/BmnEventHeader.cxx
 header/BmnSpillHeader.cxx
 header/DigiRunHeader.cxx
 header/DstRunHeader.cxx
 header/DstEventHeader.cxx

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

 data/CbmMCTrack.cxx
 data/CbmStack.cxx
 data/BmnStripData.h
 data/BmnStripDigit.cxx
 data/BmnCaloDigit.cxx
 data/BmnTacquilaDigit.cxx
 data/BmnDigiContainerTemplate.cxx
 data/BmnHit.cxx
 data/BmnLink.cxx
 data/BmnMatch.cxx
 data/BmnTrack.cxx
 data/BmnIdentifiableTrack.cxx
 data/BmnTrackMatch.cxx
 data/BmnVertex.cxx
 data/BmnEventQuality.cxx
 data/BmnFitNode.h
 
 BmnEnums.h
 BmnDetectorList.cxx
 BmnTask.cxx
 BmnFunctionSet.cxx
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

Set(LINKDEF BaseLinkDef.h)
Set(LIBRARY_NAME BmnBase)
Set(DEPENDENCIES Base Core ${ZeroMQ_LIBRARY_SHARED} boost_serialization rt)

GENERATE_LIBRARY()
