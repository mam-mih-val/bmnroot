# Create a library called "libTangoData"

set(INCLUDE_DIRECTORIES
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/tango
)

Set(SYSTEM_INCLUDE_DIRECTORIES
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
 TangoData.cxx
)

set(HEADERS)
Set(LINKDEF TangoDataLinkDef.h)
Set(LIBRARY_NAME TangoData)
Set(DEPENDENCIES UniCommon Core Geom Gpad Net)

GENERATE_LIBRARY()

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/tango/examples)
