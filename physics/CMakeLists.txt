# Create a library called "libBmnParticles"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/uni_db/elog_classes
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/common
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/csc
 ${CMAKE_SOURCE_DIR}/bmndst
## New directory structure for common library to be built ...
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/physics
 ${CMAKE_SOURCE_DIR}/physics/bmnrecotools
 ${CMAKE_SOURCE_DIR}/physics/decay
 ${CMAKE_SOURCE_DIR}/physics/efficiency
 ${CMAKE_SOURCE_DIR}/physics/embedding
 ${CMAKE_SOURCE_DIR}/physics/run7
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

# List of source files
set(SRCS
 decay/BmnTwoParticleDecay.cxx
 decay/BmnParticlePair.cxx
 decay/BmnParticlePairsInfo.cxx
 decay/BmnParticlePairCut.cxx
 decay/BmnMassSpectrumAnal.cxx

efficiency/BmnEfficiency.cxx
efficiency/BmnEfficiencyProbability.cxx
efficiency/BmnRealisticMc.cxx
efficiency/BmnEfficiencyTools.cxx
efficiency/BmnDataTriggerInfo.cxx

embedding/BmnLambdaEmbeddingMonitor.cxx
embedding/BmnParticleStore.cxx
embedding/BmnLambdaEmbeddingQa.cxx
embedding/BmnLambdaEmbeddingDrawHistos.cxx
embedding/BmnLambdaMisc.cxx
embedding/BmnInnerTrackerGeometryDraw.cxx
embedding/BmnLambdaEmbedding.cxx

run7/BmnLambdaAnalRun7.cxx
run7/BmnDataAnalRun7.cxx
run7/BmnTriggerEfficiencyRun7.cxx
)

Set(HEADERS)
Set(LINKDEF LinkDef.h)
Set(LIBRARY_NAME BmnPhysics)
Set(DEPENDENCIES BmnDst Decoder Gem Silicon CSC BmnData BmnField UniDb Base Core)

GENERATE_LIBRARY()
