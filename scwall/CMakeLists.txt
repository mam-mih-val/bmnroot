# Create a library called "libScWall"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/scwall
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
BmnScWall.cxx 
BmnScWallPoint.cxx  
BmnScWallContFact.cxx  
BmnScWallGeo.cxx 
BmnScWallGeoPar.cxx
)

Set(HEADERS)
Set(LINKDEF BmnScWallLinkDef.h)
Set(LIBRARY_NAME ScWall)
Set(DEPENDENCIES BmnData Base Core)

GENERATE_LIBRARY()
