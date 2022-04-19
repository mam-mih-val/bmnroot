# Create a library called "libBmnRecoFHCal"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/raw
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/detectors/fhcal
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/reconstruction/fhcal
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
 BmnFHCalReconstructor.cxx
)

Set(HEADERS)
Set(LINKDEF BmnRecoFHCalLinkDef.h)
Set(LIBRARY_NAME BmnRecoFHCal)
Set(DEPENDENCIES Decoder FHCal UniDb BmnData Base Core)

GENERATE_LIBRARY()
