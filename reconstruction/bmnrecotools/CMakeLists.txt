# Create a library called "libBmnRecoTools"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/field
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/generators
 ${CMAKE_SOURCE_DIR}/detectors/bd
 ${CMAKE_SOURCE_DIR}/detectors/tof1
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/sts
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/tof
 ${CMAKE_SOURCE_DIR}/detectors/zdc
 ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
 ${CMAKE_SOURCE_DIR}/reconstruction/zdc
 ${CMAKE_SOURCE_DIR}/reconstruction/bmnrecotools
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
 BmnFairRunSim.cxx
 BmnRecoTools.cxx
 BmnCheckMCTask.cxx
 BmnPVGetter.cxx
 MpdLAQGSMGeneratorExt.cxx
 BmnEmbedding.cxx
 BmnRescale.cxx
 BmnTrackConv.cxx
)

Set(HEADERS)
Set(LINKDEF RecoToolsLinkDef.h)
Set(LIBRARY_NAME BmnRecoTools)
Set(DEPENDENCIES BmnGlobalTracking BmnRecoZdc Zdc CSC Gem Sts Silicon BmnBase MpdGen BmnField Base Core Bd TOF1 Dch)

GENERATE_LIBRARY()
