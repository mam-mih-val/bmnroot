# Create a library called "libTOF1" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/mcstack
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/tof1
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
BmnTOF1.cxx 
BmnTOF1Point.cxx  
BmnTOF1ContFact.cxx  
BmnTOF1Geo.cxx 
BmnTOF1GeoPar.cxx
BmnTof1HitProducer.cxx
BmnTof1HitProducerIdeal.cxx
BmnTof1GeoUtils.cxx
BmnTOF1TDC.cxx
BmnTOF1Detector.cxx
BmnTOF1Conteiner.cxx
)

Set(HEADERS)
Set(LINKDEF BmnTOF1LinkDef.h)
Set(LIBRARY_NAME TOF1)
Set(DEPENDENCIES BmnData Core Base)

GENERATE_LIBRARY()
