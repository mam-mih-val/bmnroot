# Create a library called "libBmnDst" which includes the source files given in the array

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/bmndst
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR} 
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 DstRunHeader.cxx
 DstEventHeader.cxx
 BmnFillDstTask.cxx
)

Set(HEADERS)
Set(LINKDEF BmnDstLinkDef.h)
Set(LIBRARY_NAME BmnDst)
Set(DEPENDENCIES Core Base BmnData)

GENERATE_LIBRARY()
