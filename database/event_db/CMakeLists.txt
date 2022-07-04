# Create a library called "libEventDb"

set(INCLUDE_DIRECTORIES
 ${CMAKE_CURRENT_SOURCE_DIR}
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
 EventCatalogue.cxx
 
 event_db_settings.h
)

set(HEADERS)
Set(LINKDEF EventDbLinkDef.h)
Set(LIBRARY_NAME EventDb)
Set(DEPENDENCIES Core Geom Gpad Net) #xml2)

GENERATE_LIBRARY()

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scheme/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/event_db/scheme)
