# Create a library called "libCat" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${Boost_INCLUDE_DIRS}
${ROOT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/sts
${CMAKE_SOURCE_DIR}/tof
${CMAKE_SOURCE_DIR}/KF
${CMAKE_SOURCE_DIR}/KF/KFQA
${CMAKE_SOURCE_DIR}/KF/Interface
${CMAKE_SOURCE_DIR}/KF/KFParticlesFinder
${CMAKE_SOURCE_DIR}/cat
${CMAKE_SOURCE_DIR}/cat/L1Algo
${CMAKE_SOURCE_DIR}/cat/OffLineInterface
${CMAKE_SOURCE_DIR}/cat/ParticleFinder
)

include_directories(${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})


set(CAT_SRCS
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

# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h CAT_HEADERS "${CAT_SRCS}")

set(CAT_HEADERS
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

ADD_DEFINITIONS(-Wall -Wabi -Wsign-promo  -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel -Wno-non-template-friend) 

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${CAT_SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
  Message(STATUS "CAT will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "CAT will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${CAT_SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_FOUND)

set(CAT_LINKDEF CatLinkDef.h)
set(CAT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnCatDict.cxx) 

ROOT_GENERATE_DICTIONARY("${CAT_HEADERS}" "${CAT_LINKDEF}" "${CAT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(CAT_SRCS ${CAT_SRCS} ${CAT_DICTIONARY})


add_library(Cat SHARED ${CAT_SRCS})
target_link_libraries(Cat ${ROOT_LIBRARIES})
set_target_properties(Cat PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Cat DESTINATION ${CMAKE_BINARY_DIR}/lib)

