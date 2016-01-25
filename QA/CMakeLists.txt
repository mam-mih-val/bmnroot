# Create a library called "libBmnQa" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${Boost_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/globaltracking
${CMAKE_SOURCE_DIR}/QA
${CMAKE_SOURCE_DIR}/sts
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(BMNQA_SRCS
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
report/BmnTextReportElement.cxx
report/BmnLatexReportElement.cxx
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


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h BMNQA_HEADERS "${BMNQA_SRCS}")

set(BMNQA_LINKDEF BmnQALinkDef.h)
set(BMNQA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnQaDict.cxx) 

ROOT_GENERATE_DICTIONARY("${BMNQA_HEADERS}" "${BMNQA_LINKDEF}" "${BMNQA_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(BMNQA_SRCS ${BMNQA_SRCS} ${BMNQA_DICTIONARY})

If ( ${ROOT_HAS_GDML} STREQUAL yes )
 Set (DEPENDENCIES ${DEPENDENCIES} Gdml)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

add_library(BmnQa SHARED ${BMNQA_SRCS})
target_link_libraries(BmnQa ${ROOT_LIBRARIES})
set_target_properties(BmnQa PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS BmnQa DESTINATION ${CMAKE_BINARY_DIR}/lib)


