# Create a library called "libElogDb"

set(INCLUDE_DIRECTORIES
 ${CMAKE_SOURCE_DIR}/database/elog_db
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
 ElogDbPerson.cxx
 ElogDbType.cxx
 ElogDbTrigger.cxx
 ElogDbBeam.cxx
 ElogDbTarget.cxx
 ElogDbRecord.cxx
 ElogDbAttachment.cxx 
 
 ElogConnection.cxx
 ElogSearchCondition.cxx

 elog_db_settings.h
)

set(HEADERS)
Set(LINKDEF ElogDbLinkDef.h)
Set(LIBRARY_NAME ElogDb)
Set(DEPENDENCIES Core Geom Gpad Net)

GENERATE_LIBRARY()

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/elog_db/examples)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/elog_db/macros)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scheme/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/elog_db/scheme)
