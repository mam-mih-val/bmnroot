# Create a library called "BmnPidSRC"

Set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndst
 ${CMAKE_SOURCE_DIR}/dch
 ${CMAKE_SOURCE_DIR}/identification
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
Set(SRCS
BmnPidSRC.cxx
BmnPid.cxx
)

Set(HEADERS)
Set(LINKDEF IdentLinkDef.h)
Set(LIBRARY_NAME BmnPidSRC)
Set(DEPENDENCIES BmnDst Dch BmnData Base)

GENERATE_LIBRARY()
