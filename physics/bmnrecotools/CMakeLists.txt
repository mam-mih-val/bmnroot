# Create a library called "libRecoTools"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/generators
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/csc
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/bmndst
 ${CMAKE_SOURCE_DIR}/physics/bmnrecotools
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR} 
 ${Boost_INCLUDE_DIRS}
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
 BmnFairRunSim.cxx
 BmnRecoTools.cxx
 BmnCheckMCTask.cxx
 BmnPVGetter.cxx
 MpdLAQGSMGeneratorExt.cxx
 BmnEmbedding.cxx
 BmnRescale.cxx
)

Set(HEADERS)
Set(LINKDEF RecoToolsLinkDef.h)
Set(LIBRARY_NAME RecoTools)
Set(DEPENDENCIES BmnDst Gen Silicon CSC BmnData MpdGen BmnField Base Core)

GENERATE_LIBRARY()
