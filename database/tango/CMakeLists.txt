# Create a library called "libTangoData"

set(INCLUDE_DIRECTORIES
 ${CMAKE_CURRENT_SOURCE_DIR}
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${XML2_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 TangoData.cxx
 TangoConnection.cxx
 
 tango_db_settings.h
)

set(HEADERS)
Set(LINKDEF TangoDataLinkDef.h)
Set(LIBRARY_NAME TangoData)
Set(DEPENDENCIES Core Geom Gpad Net)

GENERATE_LIBRARY()

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/tango/examples)
