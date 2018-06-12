# Create a library called "libBmnAlignment" which includes the source files given in
# the array.
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/cbm
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmnfield
 ${CMAKE_SOURCE_DIR}/globaltracking
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/mwpc
 ${CMAKE_SOURCE_DIR}/dch
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/alignment
 ${CMAKE_SOURCE_DIR}/gem
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
 BmnGlobalAlignment.cxx
 BmnGemAlignCorrections.cxx
 BmnDchAlignCorrections.cxx
 BmnMwpcAlignCorrections.cxx
 BmnSiliconAlignCorrections.cxx
 BmnMille.cxx
 BmnAlignCorrections.cxx
 BmnGemResiduals.cxx
 BmnSiResiduals.cxx
 BmnResiduals.cxx
)

Set(HEADERS)
Set(LINKDEF AlignLinkDef.h)
Set(LIBRARY_NAME BmnAlignment)
Set(DEPENDENCIES Core Base Mwpc)

GENERATE_LIBRARY()
