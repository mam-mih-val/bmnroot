# Create a library called "libBmnQa"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndst
 ${CMAKE_SOURCE_DIR}/ssd/auxx/data
 ${CMAKE_SOURCE_DIR}/csc
 ${CMAKE_SOURCE_DIR}/dch
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/sts
 ${CMAKE_SOURCE_DIR}/steering
 ${CMAKE_SOURCE_DIR}/globaltracking
 ${CMAKE_SOURCE_DIR}/physics/decay
# ${CMAKE_SOURCE_DIR}/monitor
 ${CMAKE_SOURCE_DIR}/QA
 ${CMAKE_SOURCE_DIR}/QA/report
 ${CMAKE_SOURCE_DIR}/QA/offline
 ${CMAKE_SOURCE_DIR}/uni_db/elog_classes
 ${CMAKE_SOURCE_DIR}/bmnfield
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

 BmnTrackingQa.cxx
 BmnLambdaQa.cxx
 BmnClusteringQa.cxx
 BmnUtils.cxx
 BmnTrackingQaReport.cxx
 BmnLambdaQaReport.cxx
 BmnClusteringQaReport.cxx
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
Set(LINKDEF BmnQALinkDef.h)
Set(LIBRARY_NAME BmnQa)
Set(DEPENDENCIES BmnPhysics GlobalTracking BmnSteering BmnDst Sts Gem Silicon Dch CSC BmnData UniDb Base Core)

If ( ${ROOT_HAS_GDML} STREQUAL yes )
 Set (DEPENDENCIES ${DEPENDENCIES} Gdml)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

GENERATE_LIBRARY()
