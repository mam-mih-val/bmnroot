# Create a library called "libEventDisplay" 

Set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/config
 ${CMAKE_SOURCE_DIR}/eventdisplay
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${ROOT_INCLUDE_DIR} 
  ${Geant3_INCLUDE_DIRS}
  ${XML2_INCLUDE_DIR}
)

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${XML2_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
 ${Boost_LIBRARY_DIRS}
)

set(SRCS
 MpdEventManager.cxx  
 MpdMCTracks.cxx
 MpdMCStack.cxx
 MpdEventManagerEditor.cxx
 MpdMCTracksEditor.cxx
 MpdBoxSet.cxx
 MpdBoxSetDraw.cxx
 MpdBoxSetEditor.cxx
 MpdPointSetDraw.cxx
 MpdMCPointDraw.cxx
 MpdHitDraw.cxx
 MpdXMLNode.cxx
 MpdHitPointSetDraw.cxx
 MpdWebScreenshots.cxx
)

if (BMNROOT)
  Set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_SOURCE_DIR}/base
    ${CMAKE_SOURCE_DIR}/base/data
    ${CMAKE_SOURCE_DIR}/reconstruction/globaltracking
    ${CMAKE_SOURCE_DIR}/detectors/silicon
    ${CMAKE_SOURCE_DIR}/detectors/gem
    ${CMAKE_SOURCE_DIR}/detectors/dch
    ${CMAKE_SOURCE_DIR}/detectors/zdc
    ${CMAKE_SOURCE_DIR}/services
    ${CMAKE_SOURCE_DIR}/eventdisplay/bmn
  )
  Set(SRCS
    ${SRCS}
    bmn/BmnTrackDrawH.cxx
    bmn/BmnTrackDrawP.cxx
    bmn/BmnGlobalTrackDraw.cxx
    bmn/BmnCaloTowerDraw.cxx
    bmn/FairMCModuleDraw.cxx
  )
endif (BMNROOT)

if (MPDROOT)
  Set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_SOURCE_DIR}/mcstack
    ${CMAKE_SOURCE_DIR}/mpdbase
    ${CMAKE_SOURCE_DIR}/mpddst
    ${CMAKE_SOURCE_DIR}/kalman
    ${CMAKE_SOURCE_DIR}/tpc
    ${CMAKE_SOURCE_DIR}/lhetrack
    ${CMAKE_SOURCE_DIR}/emc
    ${CMAKE_SOURCE_DIR}/zdc
    ${CMAKE_SOURCE_DIR}/eventdisplay/mpd
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
  if (BMNROOT)
   Set(SRCS
    ${SRCS}   
    tdaq/BmnEventPull.cxx
    #tdaq/BmnEventMonitor.cxx
    tdaq/BmnTdaqSource.cxx
   )
  endif (BMNROOT)
endif (TDAQ_FOUND)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

Set(HEADERS)
if (TDAQ_FOUND)
 Set(LINKDEF tdaq/EventDLinkDef.h)
else (TDAQ_FOUND)
 Set(LINKDEF EventDisplayLinkDef.h)
endif (TDAQ_FOUND)
if (BMNROOT)
 if (TDAQ_FOUND)
  Set(LINKDEF bmn/EventBDLinkDef.h)
 else (TDAQ_FOUND)
  Set(LINKDEF bmn/EventBLinkDef.h)
 endif (TDAQ_FOUND)
endif (BMNROOT)
if (MPDROOT)
 if (TDAQ_FOUND)
  Set(LINKDEF mpd/EventMDLinkDef.h)
 else (TDAQ_FOUND)
  Set(LINKDEF mpd/EventMLinkDef.h)
 endif (TDAQ_FOUND)
endif (MPDROOT)

if (TDAQ_FOUND)
  Set(DEPENDENCIES Base XMLParser Eve Geom EG Physics Ged Gui Graf3d Tree Core xml2 emon emon-dal cmdline ipc owl omniORB4 omnithread ers)
else (TDAQ_FOUND)
  Set(DEPENDENCIES Base XMLParser Eve Geom EG Physics Ged Gui Graf3d Tree Core xml2)
endif (TDAQ_FOUND)
if (BMNROOT)
 Set(DEPENDENCIES BmnGlobalTracking Gem Silicon Dch BmnBase Geane ${DEPENDENCIES})
endif (BMNROOT)
if (MPDROOT)
 Set(DEPENDENCIES LHETrack Kalman MpdDst Zdc Emc tpc Geane ${DEPENDENCIES})
endif (MPDROOT)

Set(LIBRARY_NAME EventDisplay)

If ((${FairRoot_VERSION_MAJOR} MATCHES "18") AND (${FairRoot_VERSION_MINOR} MATCHES "6") AND (${FairRoot_VERSION_PATCH} VERSION_GREATER "6"))
 add_definitions(-DFairRoot_18_6_7)
EndIf()

GENERATE_LIBRARY()
