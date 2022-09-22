# Create a library called "libBmnQa"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/database/elog_db
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/detectors/ssd/auxx/data
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/sts
 ${CMAKE_SOURCE_DIR}/detectors/tof
 ${CMAKE_SOURCE_DIR}/detectors/tof1
 ${CMAKE_SOURCE_DIR}/detectors/ecal
 ${CMAKE_SOURCE_DIR}/detectors/zdc
 ${CMAKE_SOURCE_DIR}/steering
 ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
 ${CMAKE_SOURCE_DIR}/physics/decay
 ${CMAKE_SOURCE_DIR}/QA
 ${CMAKE_SOURCE_DIR}/QA/report
 ${CMAKE_SOURCE_DIR}/QA/offline
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
 ${Boost_LIBRARY_DIRS}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 report/BmnRunInfo.cxx
 report/BmnSimulationReport.cxx
 report/BmnStudyReport.cxx
 report/BmnHtmlReportElement.cxx
 report/BmnReport.cxx
 report/BmnDrawHist.cxx
 report/BmnDrawOnline.cxx
 report/BmnHistManager.cxx
 report/BmnSimulationReport.cxx
 report/BmnReportElement.h


 BmnHist.cxx
 PadInfo.cxx
 BmnPadBranch.cxx
 BmnPadGenerator.cxx
 BmnQaBase.cxx
 BmnPidQa.cxx
 BmnPidQaReport.cxx
 BmnTrackingQa.cxx
 BmnTrackingQaExp.cxx
 BmnLambdaQa.cxx
 BmnClusteringQa.cxx
 BmnUtils.cxx
 BmnTrackingQaReport.cxx
 BmnTrackingQaExpReport.cxx
 BmnLambdaQaReport.cxx
 BmnClusteringQaReport.cxx
 BmnCustomQa.cxx
 BmnMCTrack.cxx
 BmnMCTrackCreator.cxx
 BmnAcceptanceFunction.h
 BmnMCPoint.h
 BmnMatchRecoToMC.cxx

 offline/BmnQaHistoManager.cxx
 offline/BmnQaOffline.cxx
 offline/BmnCoordinateDetQa.cxx
 offline/BmnTimeDetQa.cxx
 offline/BmnCalorimeterDetQa.cxx
 offline/BmnTrigDetQa.cxx
 offline/BmnDstQa.cxx
 offline/BmnQaMonitor.cxx
 offline/BmnTrackingQaOffline.cxx
 offline/BmnTrackingQaOfflineDraw.cxx
)

EXEC_PROGRAM( ${ROOT_CONFIG_EXECUTABLE}
    ARGS "--has-gdml"
    OUTPUT_VARIABLE ROOT_HAS_GDML )

If ( ${ROOT_HAS_GDML} STREQUAL yes )
  Message(STATUS "Build CbmModule with Gdml support")
  Add_Definitions(-DROOT_HAS_GDML)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

Set(HEADERS)
Set(LINKDEF QALinkDef.h)
Set(LIBRARY_NAME BmnQa)
Set(DEPENDENCIES BmnPhysics BmnGlobalTracking BmnSteering Zdc Ecal TOF TOF1 Sts Gem Silicon Dch CSC Mwpc BmnBase UniDb Base Core)

If ( ${ROOT_HAS_GDML} STREQUAL yes )
 Set (DEPENDENCIES ${DEPENDENCIES} Gdml)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

GENERATE_LIBRARY()
