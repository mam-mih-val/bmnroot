# Create a library called "libKF" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/bmnfield
${CMAKE_SOURCE_DIR}/mpdfield
${CMAKE_SOURCE_DIR}/tof
${CMAKE_SOURCE_DIR}/sts
${CMAKE_SOURCE_DIR}/cat
${CMAKE_SOURCE_DIR}/KF
${CMAKE_SOURCE_DIR}/KF/Interface
${CMAKE_SOURCE_DIR}/KF/KFQA
${CMAKE_SOURCE_DIR}/KF/KFParticle
${CMAKE_SOURCE_DIR}/KF/KFParticlesFinder
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
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

#KFQA/CbmKFTrErrMCPoints.cxx
#KFQA/CbmKFTrackFitQA.cxx
KFQA/KFParticleMatch.cxx
KFQA/KFMCParticle.cxx
#KFQA/CbmKFParticlesFinderQA.cxx

#KFParticle/KFParticleBase.cxx
#KFParticle/KFParticle.cxx
#KFParticle/KFParticleBaseSIMD.cxx
#KFParticle/KFParticleSIMD.cxx
#KFParticle/KFParticleFinder.cxx

#KFParticlesFinder/CbmKFParticlesFinder.cxx
KFParticlesFinder/CbmKFParticleDatabase.cxx
)

set(HEADERS
CbmKF.h 
CbmKFFieldMath.h
CbmKFHit.h
CbmKFMaterial.h
CbmKFMath.h
CbmKFPixelMeasurement.h
CbmKFPrimaryVertexFinder.h
#CbmKFSecondaryVertexFinder.h
CbmKFTrackInterface.h
CbmKFUMeasurement.h 
CbmKFVertexInterface.h
CbmKFParticle.h

#Interface/CbmEcalTrackExtrapolationKF.h
Interface/CbmKFStsHit.h
Interface/CbmKFTrack.h 
Interface/CbmKFVertex.h 
Interface/CbmPVFinderKF.h
#Interface/CbmStsFitPerformanceTask.h
Interface/CbmStsKFTrackFitter.h
#Interface/CbmStsKFSecondaryVertexFinder.h
#Interface/CbmGlobalTrackFitterKF.h

#KFQA/CbmKFTrErrMCPoints.h
#KFQA/CbmKFTrackFitQA.h
KFQA/KFParticleMatch.h
KFQA/KFMCParticle.h
#KFQA/CbmKFParticlesFinderQA.h
KFQA/CbmKFPartEfficiencies.h

#KFParticle/KFParticleBase.h
#KFParticle/KFParticle.h

#KFParticlesFinder/CbmKFParticlesFinder.h
KFParticlesFinder/CbmKFParticleDatabase.h
)

ADD_DEFINITIONS(-Wall -Wabi -Wsign-promo  -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel -Wno-non-template-friend )

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

Set(LINKDEF KFLinkDef.h)
Set(LIBRARY_NAME KF)
Set(DEPENDENCIES Core Base)

GENERATE_LIBRARY()
