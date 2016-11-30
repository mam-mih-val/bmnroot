# Create a library called "libEventDisplay" 
set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/config
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/eventdisplay
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR} 
  ${Boost_INCLUDE_DIRS}
  ${GEANT3_INCLUDE_DIR}
  ${XML2_INCLUDE_DIR}
)

set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${XML2_LIBRARY_DIR}
)

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
FairWebScreenshots.cxx
)

if (BMNROOT)
  Set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES} 
    ${CMAKE_SOURCE_DIR}/eventdisplay/raw
    ${CMAKE_SOURCE_DIR}/bmndata
    ${CMAKE_SOURCE_DIR}/globaltracking
    ${CMAKE_SOURCE_DIR}/gem
  )
  Set(SRCS
    ${SRCS}
    raw/RawDataParser.cxx
    raw/RawDataConverter.cxx
    raw/RawMWPCDigitDraw.cxx
    bmn/BmnDigitDraw.cxx
    bmn/BmnTrackDrawH.cxx
    bmn/BmnTrackDrawP.cxx
    bmn/BmnGlobalTrackDraw.cxx
    bmn/FairMCModuleDraw.cxx
  )
endif (BMNROOT)

if (MPDROOT)
  Set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_SOURCE_DIR}/mcstack
    ${CMAKE_SOURCE_DIR}/mpddata
    ${CMAKE_SOURCE_DIR}/kalman
    ${CMAKE_SOURCE_DIR}/tpc
    ${CMAKE_SOURCE_DIR}/lhetrack
    ${CMAKE_SOURCE_DIR}/emc
    ${CMAKE_SOURCE_DIR}/zdc
  )
  Set(SRCS
    ${SRCS}
    mpd/MpdGlobalTrackDraw.cxx
    mpd/MpdEmcTowerDraw.cxx
    mpd/MpdZdcTowerDraw.cxx
  )
endif (MPDROOT)

if (TDAQ_FOUND)
  Set(SYSTEM_INCLUDE_DIRECTORIES
    ${SYSTEM_INCLUDE_DIRECTORIES} 
    ${TDAQ_INCLUDE_PATH}
  )
  Set(LINK_DIRECTORIES
    ${LINK_DIRECTORIES} 
    ${TDAQ_LIBRARY_PATH}
  )
  Set(SRCS
    ${SRCS} 
    #tdaq/BmnEventMonitor.cxx
  )
endif (TDAQ_FOUND)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

Set(HEADERS)
if (BMNROOT)
 if (TDAQ_FOUND)
  Set(LINKDEF bmn/EventBDLinkDef.h)
 else (TDAQ_FOUND)
  Set(LINKDEF bmn/EventBLinkDef.h)
 endif (TDAQ_FOUND)
else (BMNROOT)
 if (MPDROOT)
  if (TDAQ_FOUND)
   Set(LINKDEF mpd/EventMDLinkDef.h)
  else (TDAQ_FOUND)
   Set(LINKDEF mpd/EventMLinkDef.h)
  endif (TDAQ_FOUND)
 else (MPDROOT)
  if (TDAQ_FOUND)
   Set(LINKDEF tdaq/EventDLinkDef.h)
  else (TDAQ_FOUND)
   Set(LINKDEF EventDisplayLinkDef.h)
  endif (TDAQ_FOUND)
 endif (MPDROOT)
endif (BMNROOT)
Set(LIBRARY_NAME EventDisplay)

#if (TDAQ_FOUND)
#  Set(DEPENDENCIES Base Eve Geom EG Physics Ged Gui Graf3d Tree Cint Core xml2 emon emon-dal cmdline ipc owl omniORB4 omnithread ers)
#else (TDAQ_FOUND)
  Set(DEPENDENCIES Base Eve Geom EG Physics Ged Gui Graf3d Tree Cint Core xml2)
#endif (TDAQ_FOUND)

GENERATE_LIBRARY()
