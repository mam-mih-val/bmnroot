# Create a library called "libBmnEmbedding"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/physics/particles
 ${CMAKE_SOURCE_DIR}/physics/bmnrecotools
 ${CMAKE_SOURCE_DIR}/embedding
 ### Additional include sources 
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
 BmnLambdaEmbedding.cxx   
 BmnLambdaStore.cxx 
 BmnLambdaMisc.cxx 
 BmnLambdaEmbeddingMonitor.cxx
 BmnLambdaEmbeddingQa.cxx 
 BmnInnerTrackerGeometryDraw.cxx 
 BmnLambdaEmbeddingDrawHistos.cxx
 BmnEfficiency.cxx
)

Set(HEADERS)
Set(LINKDEF EmbeddingLinkDef.h)
Set(LIBRARY_NAME BmnEmbedding)
Set(DEPENDENCIES BmnRecoTools BmnParticles BmnDecoder Gem Silicon CSC BmnBase BmnField UniDb Base Core)

GENERATE_LIBRARY()
