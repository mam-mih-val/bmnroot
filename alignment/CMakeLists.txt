# Create a library called "libBmnAlignment"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
 ${CMAKE_SOURCE_DIR}/alignment
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR} 
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
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
Set(DEPENDENCIES BmnGlobalTracking Gem Silicon Dch Mwpc BmnBase BmnField UniDb Base Core)

GENERATE_LIBRARY()
