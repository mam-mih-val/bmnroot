# Create a library called "libTOF"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/detectors/tof
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
 BmnTOF.cxx
 BmnTOFPoint.cxx
 BmnTof2Digit.cxx
 BmnTOFContFact.cxx
 BmnTOFGeo.cxx
 BmnTOFGeoPar.cxx
 BmnTofHit.cxx
 BmnTofHitProducerIdeal.cxx
 BmnTofGeoUtils.cxx
)

Set(HEADERS)
Set(LINKDEF TOFLinkDef.h)
Set(LIBRARY_NAME TOF)
Set(DEPENDENCIES BmnBase Base Core)

GENERATE_LIBRARY()
