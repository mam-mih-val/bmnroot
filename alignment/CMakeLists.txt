# Create a library called "libBmnAlignment"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/mwpc
 ${CMAKE_SOURCE_DIR}/dch
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/alignment
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
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
 BmnMilleContainer.cxx 
 BmnGlobalAlignment.cxx
 BmnGemAlignCorrections.cxx
 BmnSiliconAlignCorrections.cxx
 BmnMille.cxx
 BmnAlignCorrections.cxx
 BmnResiduals.cxx
 BmnInnTrackerAlign.cxx
)

Set(HEADERS)
Set(LINKDEF AlignLinkDef.h)
Set(LIBRARY_NAME BmnAlignment)
Set(DEPENDENCIES Gem Silicon Dch Mwpc BmnData BmnField UniDb Base Core)

GENERATE_LIBRARY()
