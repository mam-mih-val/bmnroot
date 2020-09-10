# Create a library called "libBmnEmbedding"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/csc
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/bmndst
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/physics/particles
 ${CMAKE_SOURCE_DIR}/physics/bmnrecotools
 ${CMAKE_SOURCE_DIR}/embedding
 ### Additional include sources 
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
 BmnLambdaEmbedding.cxx   
 BmnLambdaStore.cxx 
 BmnLambdaMisc.cxx 
 BmnLambdaEmbeddingMonitor.cxx
 BmnLambdaEmbeddingQa.cxx 
 BmnInnerTrackerGeometryDraw.cxx 
 BmnLambdaEmbeddingDrawHistos.cxx
)

Set(HEADERS)
Set(LINKDEF EmbeddingLinkDef.h)
Set(LIBRARY_NAME BmnEmbedding)
Set(DEPENDENCIES RecoTools BmnParticles Decoder BmnDst Gem Silicon CSC BmnData BmnField UniDb Base Core)

GENERATE_LIBRARY()
