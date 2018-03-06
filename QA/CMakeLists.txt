# Create a library called "libBmnQa" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/bmndata/cbm
${CMAKE_SOURCE_DIR}/bmndata/math
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/silicon
${CMAKE_SOURCE_DIR}/globaltracking
${CMAKE_SOURCE_DIR}/QA
${CMAKE_SOURCE_DIR}/QA/report
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
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
BmnTrackingQa.cxx
BmnClusteringQa.cxx
BmnUtils.cxx
BmnTrackingQaReport.cxx
BmnClusteringQaReport.cxx
BmnMCTrack.cxx
BmnMCTrackCreator.cxx
BmnAcceptanceFunction.h
report/BmnSimulationReport.cxx
report/BmnStudyReport.cxx
report/BmnHtmlReportElement.cxx
report/BmnReport.cxx
report/BmnDrawHist.cxx
report/BmnHistManager.cxx
BmnMatchRecoToMC.cxx
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
Set(DEPENDENCIES Core Base)

If ( ${ROOT_HAS_GDML} STREQUAL yes )
 Set (DEPENDENCIES ${DEPENDENCIES} Gdml)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

GENERATE_LIBRARY()
