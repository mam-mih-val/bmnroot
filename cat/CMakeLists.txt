# Create a library called "libCat"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/sts
 ${CMAKE_SOURCE_DIR}/KF
 ${CMAKE_SOURCE_DIR}/KF/KFQA
 ${CMAKE_SOURCE_DIR}/KF/Interface
 ${CMAKE_SOURCE_DIR}/KF/KFParticlesFinder
 ${CMAKE_SOURCE_DIR}/cat
 ${CMAKE_SOURCE_DIR}/cat/L1Algo
 ${CMAKE_SOURCE_DIR}/cat/OffLineInterface
 ${CMAKE_SOURCE_DIR}/cat/ParticleFinder
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${Boost_LIBRARY_DIRS}
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
CbmL1.cxx 
CbmL1Performance.cxx 
CbmL1ReadEvent.cxx
CbmL1MCTrack.cxx
CbmL1ParticlesFinder.cxx

L1Algo/L1Algo.cxx
L1Algo/L1CATrackFinder.cxx
L1Algo/L1TrackExtender.cxx
L1Algo/L1TrackFitter.cxx
L1Algo/L1CAMergeClones.cxx
L1Algo/L1HitsSortHelper.cxx

OffLineInterface/CbmL1StsTrackFinder.cxx 

ParticleFinder/CbmL1PFFitter.cxx
ParticleFinder/CbmL1PFMCParticle.cxx
)

set(HEADERS
CbmL1.h
CbmL1MCTrack.h
CbmL1MCPoint.h
CbmL1StsHit.h
CbmL1Track.h
CbmL1TrackPar.h
CbmL1Vtx.h
OffLineInterface/CbmL1StsTrackFinder.h
CbmL1PartEfficiencies.h
)

ADD_DEFINITIONS(-Wall -Wsign-promo  -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel -Wno-non-template-friend)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
  Message(STATUS "CAT will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "CAT will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_FOUND)

Set(LINKDEF CatLinkDef.h)
Set(LIBRARY_NAME Cat)
Set(DEPENDENCIES KF Sts Gem Silicon BmnData BmnField Base Core)

GENERATE_LIBRARY()
