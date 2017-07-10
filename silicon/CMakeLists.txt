
# Create a library called "libSilicon" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${Boost_INCLUDE_DIRS}
${ROOT_INCLUDE_DIR}
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

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SILICON_SRCS
BmnSilicon.cxx
BmnSiliconPoint.cxx
BmnSiliconLayer.cxx
BmnSiliconModule.cxx
BmnSiliconStation.cxx
BmnSiliconStationSet.cxx
BmnSiliconDigit.cxx
BmnSiliconRaw2Digit.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h SILICON_HEADERS "${SILICON_SRCS}")

set(SILICON_LINKDEF SiliconLinkDef.h)
set(SILICON_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/SiliconDict.cxx)

ROOT_GENERATE_DICTIONARY("${SILICON_HEADERS}" "${SILICON_LINKDEF}" "${SILICON_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(SILICON_SRCS ${SILICON_SRCS} ${SILICON_DICTIONARY})


add_library(Silicon SHARED ${SILICON_SRCS})
target_link_libraries(Silicon ${ROOT_LIBRARIES})
set_target_properties(Silicon PROPERTIES VERSION 0.0.1 SOVERSION 0)

################ install ###################
install(TARGETS Silicon DESTINATION ${CMAKE_BINARY_DIR}/lib)
