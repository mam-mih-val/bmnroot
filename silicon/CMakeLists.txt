# Create a library called "libSilicon"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/alignment
 ${CMAKE_SOURCE_DIR}/silicon
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
 BmnSilicon.cxx
 BmnSiliconPoint.cxx
 BmnSiliconLayer.cxx
 BmnSiliconModule.cxx
 BmnSiliconStation.cxx
 BmnSiliconStationSet.cxx
 BmnSiliconDigitizer.cxx
 BmnSiliconHitMaker.cxx
 BmnSiliconDigit.cxx
 BmnSiliconTrack.cxx
 BmnSiliconHitProducer.cxx
 BmnSiliconHitProducerSRC.cxx
 BmnSiliconTrackFinder.cxx
 BmnSiliconConfiguration.h
)

Set(HEADERS)
Set(LINKDEF SiliconLinkDef.h)
Set(LIBRARY_NAME Silicon)
Set(DEPENDENCIES BmnData Base XMLParser Core)

GENERATE_LIBRARY()
