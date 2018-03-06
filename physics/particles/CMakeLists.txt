# Create a library called "libBmnParticles" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/bmnfield
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/bmndata/cbm
${CMAKE_SOURCE_DIR}/bmndata/math
${CMAKE_SOURCE_DIR}/globaltracking
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/physics/particles
${CMAKE_SOURCE_DIR}/uni_db
${BMNROOT_SOURCE_DIR}/uni_db/db_classes
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

# List of source files
set(SRCS
BmnTwoParticleDecay.cxx
BmnParticlePair.cxx
)

Set(HEADERS)
Set(LINKDEF LinkDef.h)
Set(LIBRARY_NAME BmnParticles)
Set(DEPENDENCIES Core Base)

GENERATE_LIBRARY()
