# Create a library called "libEventDb"

set(INCLUDE_DIRECTORIES
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/event_db
)

Set(SYSTEM_INCLUDE_DIRECTORIES
# ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
# ${XML2_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
# ${XML2_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 EventCatalogue.cxx
)

set(HEADERS)
Set(LINKDEF EventDbLinkDef.h)
Set(LIBRARY_NAME EventDb)
Set(DEPENDENCIES UniCommon Core Geom Gpad Net) #xml2)

GENERATE_LIBRARY()

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scheme/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/event_db/scheme)
