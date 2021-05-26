# Create a library called "libUniCommon"

set(INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
 ${CMAKE_SOURCE_DIR}/database/unicom
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
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 UniConnection.cxx
 UniValue.cxx
 UniSearchCondition.cxx
 UniGenerateClasses.cxx
 UniGeoConverter.cxx

 db_settings.h
 # add definitions of the possible database structures and enums
 db_structures.h
)

set(HEADERS)
Set(LINKDEF UniCommonLinkDef.h)
Set(LIBRARY_NAME UniCommon)
Set(DEPENDENCIES Core Geom Gpad Net xml2)

GENERATE_LIBRARY()

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros/ DESTINATION ${CMAKE_INSTALL_PREFIX}/database/macros)
