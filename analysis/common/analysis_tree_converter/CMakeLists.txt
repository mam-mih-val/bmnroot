Set(LIBRARY_NAME CbmAnalysisTreeInterface)

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
#        BmnFHCalModulesConverter.cxx
       #CbmTofHitsConverter.cxx
    )


set(AnalysisTree_LIB_DIR ${CMAKE_BINARY_DIR}/lib)
set(AnalysisTree_LIBRARIES AnalysisTreeBase AnalysisTreeInfra)
set(AnalysisTree_INCLUDE_DIR "${CMAKE_BINARY_DIR}/include")

Set(INCLUDE_DIRECTORIES
  /lustre/hades/user/parfenov/Soft/FAIRSOFT/fairsoft_jun19p1/install/include/root6/
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/bmndata
  ${CMAKE_SOURCE_DIR}/bmndata/math
  ${CMAKE_SOURCE_DIR}/bmndata/cbm
  ${CMAKE_SOURCE_DIR}/KF
  ${CMAKE_SOURCE_DIR}/KF/Interface
  ${CMAKE_SOURCE_DIR}/cat
  ${CMAKE_SOURCE_DIR}/cat/ParticleFinder
  ${CMAKE_SOURCE_DIR}/cat/L1Algo
  ${CMAKE_SOURCE_DIR}/fhcal
#  ${KFParticle_INCLUDE_DIR}
  ${AnalysisTree_INCLUDE_DIR}
  ${ROOT_INCLUDE_DIR}
)

Include_Directories (${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
  ${Boost_INCLUDE_DIR} 
)

Include_Directories (SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

message(STATUS "!!! ${INCLUDE_DIRECTORIES}")

set (LINK_DIRECTORIES
  ${AnalysisTree_LIBRARY_DIR}
  ${KFParticle_LIB_DIR}
  ${Cat_LIB_DIR}
  ${Cat_LIBRARY_DIR}
  ${FHCalls_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${ROOT_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS}
)

message(STATUS "Cat library: ${KFParticle_LIB_DIR}" )
 
link_directories(${LINK_DIRECTORIES})

IF (SSE_FOUND)
  Message(STATUS "${LIBRARY_NAME} will be compiled with SSE support")
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3")
ELSE (SSE_FOUND)
  MESSAGE(STATUS "${LIBRARY_NAME} will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3")
ENDIF (SSE_FOUND)

Set(LINKDEF CbmAnalysisTreeInterfaceLinkDef.h)

Set(_AnalysisTree_LIB AnalysisTreeBase AnalysisTreeInfra)

Set(DEPENDENCIES
        Cat
        FHCal
        Geom
        #	CbmData
        #	CbmBase
#	L1
#	CbmSimGenerators
)

Set(DEFINITIONS -DDO_TPCCATRACKER_EFF_PERFORMANCE -DNonhomogeneousField -DCBM -DUSE_TIMERS)

ADD_DEFINITIONS(${DEFINITIONS})

GENERATE_LIBRARY()

#Install(FILES ../../../macro/analysis/common/analysis_tree_converter/run_analysis_tree_maker.C
#        DESTINATION share/cbmroot/macro/analysis_tree
#       )

Add_Dependencies(CbmAnalysisTreeInterface ANALYSISTREE Cat KF)
