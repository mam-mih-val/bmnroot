# Create a library called "libTOF" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/tof
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
BmnTOF.cxx
BmnTOFPoint.cxx
BmnTOFContFact.cxx
BmnTOFGeo.cxx
BmnTOFGeoPar.cxx
BmnTofHitProducer.cxx
BmnTofHitProducerIdeal.cxx
BmnTofGeoUtils.cxx
)

Set(HEADERS)
Set(LINKDEF BmnTOFLinkDef.h)
Set(LIBRARY_NAME TOF)
Set(DEPENDENCIES BmnData Core Base)

GENERATE_LIBRARY()
