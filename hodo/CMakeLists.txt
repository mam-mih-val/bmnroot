# Create a library called "libHodo"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/hodo
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
BmnHodo.cxx 
BmnHodoPoint.cxx  
BmnHodoContFact.cxx  
BmnHodoGeo.cxx 
BmnHodoGeoPar.cxx
)

Set(HEADERS)
Set(LINKDEF BmnHodoLinkDef.h)
Set(LIBRARY_NAME Hodo)
Set(DEPENDENCIES BmnData Base Core)

GENERATE_LIBRARY()
