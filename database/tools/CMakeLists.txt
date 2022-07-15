# Create a library called "libDbTools"

set(INCLUDE_DIRECTORIES
 ${CMAKE_CURRENT_SOURCE_DIR}/../uni_db
 ${CMAKE_CURRENT_SOURCE_DIR}/../elog_db
 ${CMAKE_SOURCE_DIR}/services
 ${CMAKE_CURRENT_SOURCE_DIR}
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${Boost_INCLUDE_DIRS}
 ${XML2_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
 ${XML2_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 UniGenerateClasses.cxx
 UniParser.cxx
)

set(HEADERS)
Set(LINKDEF DbToolsLinkDef.h)
Set(LIBRARY_NAME DbTools)
Set(DEPENDENCIES ElogDb UniDb Core xml2)

GENERATE_LIBRARY()
