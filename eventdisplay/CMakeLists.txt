# Create a library called "libEventDisplay" 
# 06.12.07  M. Al-Turany

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/gconfig
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/mpddata
${CMAKE_SOURCE_DIR}/kalman
${CMAKE_SOURCE_DIR}/globaltracking
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/mwpc
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/eventdisplay
${CMAKE_SOURCE_DIR}/eventdisplay/raw
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR} 
  ${Boost_INCLUDE_DIRS}
  ${GEANT3_INCLUDE_DIR}
  ${XML2_INCLUDE_DIR}
)

Include_Directories(${INCLUDE_DIRECTORIES})

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${XML2_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

set(SRCS
FairEventManager.cxx  
FairMCTracks.cxx
FairMCStack.cxx
FairEventManagerEditor.cxx
FairMCTracksEditor.cxx
FairBoxSet.cxx
FairBoxSetDraw.cxx
FairBoxSetEditor.cxx
FairPointSetDraw.cxx
FairMCPointDraw.cxx
FairHitDraw.cxx
FairHitPointSetDraw.cxx
FairTimeStampPointDraw.cxx
FairMCModuleDraw.cxx
BmnDigitDraw.cxx
BmnGlobalTrackDraw.cxx
BmnExpTrackDraw.cxx
BmnTrackDraw.cxx
FairWebScreenshots.cxx
raw/RawDataParser.cxx
raw/RawDataConverter.cxx
raw/RawMWPCDigitDraw.cxx
)

Set(HEADERS)
Set(LINKDEF EventDisplayLinkDef.h)
Set(LIBRARY_NAME EventDisplay)
Set(DEPENDENCIES Base Eve Geom EG Physics Ged Gui Graf3d Tree Cint Core xml2)

GENERATE_LIBRARY()
