# Create a library called "libBmnParticles" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${Boost_INCLUDE_DIRS}
${ROOT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/globaltracking
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/bmnbase
#${CMAKE_SOURCE_DIR}/mwpc
#${CMAKE_SOURCE_DIR}/dch
${CMAKE_SOURCE_DIR}/physics/particles
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}

)
 
link_directories( ${LINK_DIRECTORIES})

set(BMNPARTICLES_SRCS
BmnLambdaInvMass.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h BMNPARTICLES_HEADERS "${BMNPARTICLES_SRCS}")

set(BMNPARTICLES_LINKDEF LinkDef.h)
set(BMNPARTICLES_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnParticlesDict.cxx) 

ROOT_GENERATE_DICTIONARY("${BMNPARTICLES_HEADERS}" "${BMNPARTICLES_LINKDEF}" "${BMNPARTICLES_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(BMNPARTICLES_SRCS ${BMNPARTICLES_SRCS} ${BMNPARTICLES_DICTIONARY})


add_library(BmnParticles SHARED ${BMNPARTICLES_SRCS})
target_link_libraries(BmnParticles ${ROOT_LIBRARIES})
set_target_properties(BmnParticles PROPERTIES VERSION 0.0.1 SOVERSION 0)

################ install ###################
install(TARGETS BmnParticles DESTINATION ${CMAKE_BINARY_DIR}/lib)
