set(AnalysisTree_LIB_DIR ${CMAKE_BINARY_DIR}/lib)
set(AnalysisTree_LIBRARIES AnalysisTreeBase AnalysisTreeInfra)
set(AnalysisTree_INCLUDE_DIR "${CMAKE_BINARY_DIR}/include")

Set(INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${CMAKE_SOURCE_DIR}/bmndata
  ${CMAKE_SOURCE_DIR}/bmndata/math
  ${CMAKE_SOURCE_DIR}/bmndata/cbm
  ${CMAKE_SOURCE_DIR}/KF
  ${CMAKE_SOURCE_DIR}/KF/Interface
  ${CMAKE_SOURCE_DIR}/detectors/fhcal
  ${AnalysisTree_INCLUDE_DIR}
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${ZeroMQ_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set (LINK_DIRECTORIES
  ${AnalysisTree_LIB_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${ROOT_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS}
)

link_directories(${LINK_DIRECTORIES})

set(SRCS
  BmnConverterManager.cxx
  BmnConverterTask.cxx
  BmnSimEventHeaderConverter.cxx
  BmnRecEventHeaderConverter.cxx
  BmnGlobalTracksConverter.cxx
  BmnStsTracksConverter.cxx
  BmnSimParticlesConverter.cxx
  BmnTofHitsConverter.cxx
  BmnFHCalModulesConverter.cxx
)

Set(HEADERS)

Set(LINKDEF CbmAnalysisTreeInterfaceLinkDef.h)
Set(LIBRARY_NAME CbmAnalysisTreeInterface)
Set(DEPENDENCIES FHCal Geom)

GENERATE_LIBRARY()