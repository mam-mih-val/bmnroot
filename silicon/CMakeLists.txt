# Create a library called "libSilicon" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/field
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/base/source
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/silicon
${CMAKE_SOURCE_DIR}/uni_db
${CMAKE_SOURCE_DIR}/uni_db/db_classes
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
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
BmnSiliconHit.cxx
BmnSiliconRaw2Digit.cxx
)

Set(HEADERS)
Set(LINKDEF SiliconLinkDef.h)
Set(LIBRARY_NAME Silicon)
Set(DEPENDENCIES Core Base BmnData)

GENERATE_LIBRARY()
