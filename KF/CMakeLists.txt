# Create a library called "libKF"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/passive
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/tof
 ${CMAKE_SOURCE_DIR}/sts
 ${CMAKE_SOURCE_DIR}/cat
 ${CMAKE_SOURCE_DIR}/KF
 ${CMAKE_SOURCE_DIR}/KF/Interface
 ${CMAKE_SOURCE_DIR}/KF/KFParticlesFinder
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
CbmKF.cxx 
CbmKFFieldMath.cxx 
CbmKFHit.cxx 
CbmKFMaterial.cxx 
CbmKFMath.cxx 
CbmKFPixelMeasurement.cxx 
CbmKFPrimaryVertexFinder.cxx 
#CbmKFSecondaryVertexFinder.cxx 
CbmKFTrackInterface.cxx 
CbmKFUMeasurement.cxx 
CbmKFVertexInterface.cxx 
CbmKFParticle.cxx
#CbmKFParticle_simd.cxx
#CbmKFParticleInterface.cxx

#Interface/CbmEcalTrackExtrapolationKF.cxx
Interface/CbmKFStsHit.cxx 
Interface/CbmKFTrack.cxx 
Interface/CbmKFVertex.cxx 
Interface/CbmPVFinderKF.cxx 
#Interface/CbmStsFitPerformanceTask.cxx 
Interface/CbmStsKFTrackFitter.cxx 
#Interface/CbmStsKFSecondaryVertexFinder.cxx 
#Interface/CbmGlobalTrackFitterKF.cxx
Interface/BmnKFStsHit.cxx
Interface/BmnStsKFTrackFitter.cxx 

#KFQA/CbmKFTrErrMCPoints.cxx
#KFQA/CbmKFTrackFitQA.cxx
KFQA/KFParticleMatch.cxx
KFQA/KFMCParticle.cxx
KFQA/CbmKFPartEfficiencies.h
#KFQA/CbmKFParticlesFinderQA.cxx

#KFParticle/KFParticleBase.cxx
#KFParticle/KFParticle.cxx
#KFParticle/KFParticleBaseSIMD.cxx
#KFParticle/KFParticleSIMD.cxx
#KFParticle/KFParticleFinder.cxx

#KFParticlesFinder/CbmKFParticlesFinder.cxx
KFParticlesFinder/CbmKFParticleDatabase.cxx
)

ADD_DEFINITIONS(-Wall -Wsign-promo  -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel -Wno-non-template-friend )

IF (SSE_FOUND)
  Message(STATUS "KF will be compiled with SSE support")
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
ELSE (SSE_FOUND)
  MESSAGE(STATUS "KF will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_FOUND)

set(HEADERS)
Set(LINKDEF KFLinkDef.h)
Set(LIBRARY_NAME KF)
Set(DEPENDENCIES TOF Gem Silicon BmnData BmnField Passive Base Core)

GENERATE_LIBRARY()
