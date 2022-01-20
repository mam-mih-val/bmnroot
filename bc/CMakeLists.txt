# Create a library called "libBC"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/mcstack
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bc
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
Set(LINKDEF BmnBCLinkDef.h)
Set(LIBRARY_NAME BC)
Set(DEPENDENCIES BmnData Core Base)

GENERATE_LIBRARY()
