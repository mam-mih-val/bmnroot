# Create a library called "libBC"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/detectors/bc
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
 BmnBC.cxx
 BmnBCPoint.cxx
 BmnBCContFact.cxx
 BmnBCGeo.cxx
 BmnBCGeoPar.cxx
 BmnBCHit.cxx
 BmnBCHitProducer.cxx
)

Set(HEADERS)
Set(LINKDEF BCLinkDef.h)
Set(LIBRARY_NAME BC)
Set(DEPENDENCIES BmnBase Core Base)

GENERATE_LIBRARY()
