# Create a library called "libTOF"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/decoder
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
 ${FAIRROOT_LIBRARY_DIR}
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
Set(DEPENDENCIES Decoder BmnData Base Core) 

GENERATE_LIBRARY()
