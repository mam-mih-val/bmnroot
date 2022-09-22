# Create a library called "libBmnParticles"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/database/elog_db
 ${CMAKE_SOURCE_DIR}/database/tango
 ${CMAKE_SOURCE_DIR}/database/tools
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/tof1
 ${CMAKE_SOURCE_DIR}/detectors/tof
 ${CMAKE_SOURCE_DIR}/detectors/tofcal
 ${CMAKE_SOURCE_DIR}/detectors/ecal
 ${CMAKE_SOURCE_DIR}/detectors/zdc
 ${CMAKE_SOURCE_DIR}/detectors/scwall
 ${CMAKE_SOURCE_DIR}/detectors/hodo
 ${CMAKE_SOURCE_DIR}/detectors/fhcal
 ${CMAKE_SOURCE_DIR}/detectors/ndet
 ${CMAKE_SOURCE_DIR}/detectors/land
## New directory structure for common library to be built ...
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/reconstruction/bmnrecotools
 ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
 ${CMAKE_SOURCE_DIR}/physics
 ${CMAKE_SOURCE_DIR}/physics/decay
 ${CMAKE_SOURCE_DIR}/physics/efficiency
 ${CMAKE_SOURCE_DIR}/physics/embedding
 ${CMAKE_SOURCE_DIR}/physics/run7
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
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
Set(LINKDEF PhysicsLinkDef.h)
Set(LIBRARY_NAME BmnPhysics)
Set(DEPENDENCIES BmnRecoTools BmnGlobalTracking BmnDecoder LAND FHCal Hodo ScWall Ndet Zdc Ecal TofCal TOF TOF1 Gem Silicon CSC Dch Mwpc BmnBase BmnField DbTools UniDb ElogDb)

GENERATE_LIBRARY()
