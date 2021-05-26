# Create a library called "libElogDb"

set(INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/elog_db
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
 ElogDbPerson.cxx
 ElogDbType.cxx
 ElogDbTrigger.cxx
 ElogDbBeam.cxx
 ElogDbTarget.cxx
 ElogDbRecord.cxx
 ElogDbAttachment.cxx
)

set(HEADERS)
Set(LINKDEF ElogDbLinkDef.h)
Set(LIBRARY_NAME ElogDb)
Set(DEPENDENCIES UniCommon Core Geom Gpad Net)

GENERATE_LIBRARY()

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/elog_db/examples)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/elog_db/macros)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scheme/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/elog_db/scheme)
