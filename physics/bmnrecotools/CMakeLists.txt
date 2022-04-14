# Create a library called "libRecoTools"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/generators
 ${CMAKE_SOURCE_DIR}/detectors/bd
 ${CMAKE_SOURCE_DIR}/detectors/tof1
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/bmndst
 ${CMAKE_SOURCE_DIR}/physics/bmnrecotools
)

Set(SYSTEM_INCLUDE_DIRECTORIES
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
 BmnFairRunSim.cxx
 BmnRecoTools.cxx
 BmnCheckMCTask.cxx
 BmnPVGetter.cxx
 MpdLAQGSMGeneratorExt.cxx
 BmnEmbedding.cxx
 BmnRescale.cxx
 BmnTrackConv.cxx
)

Set(HEADERS)
Set(LINKDEF RecoToolsLinkDef.h)
Set(LIBRARY_NAME RecoTools)
Set(DEPENDENCIES BmnDst Gem Silicon CSC BmnData MpdGen BmnField Base Core Bd TOF1 Dch)

GENERATE_LIBRARY()
