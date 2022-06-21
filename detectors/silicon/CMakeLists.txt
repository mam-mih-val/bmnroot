# Create a library called "libSilicon"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/cbm
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/detectors/silicon
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
 BmnSiliconHit.cxx
 BmnSiliconHitProducer.cxx
 BmnSiliconHitProducerSRC.cxx
 BmnSiliconTrackFinder.cxx
 BmnSiliconTransform.cxx
 BmnSiliconConfiguration.h
)

Set(HEADERS)
Set(LINKDEF SiliconLinkDef.h)
Set(LIBRARY_NAME Silicon)
Set(DEPENDENCIES BmnBase UniDb Base XMLParser Gdml Core)

GENERATE_LIBRARY()
