# Create a library called "libBmnAlignment" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/globaltracking
${CMAKE_SOURCE_DIR}/alignment
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/mwpc
${CMAKE_SOURCE_DIR}/dch
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}

)
 
link_directories( ${LINK_DIRECTORIES})

set(BMNALIGNMENT_SRCS
BmnGemAlignment.cxx
BmnGemAlignCorrections.cxx
BmnGemAlignmentCorrections.cxx
BmnDchAlignCorrections.cxx
BmnMwpcAlignCorrections.cxx
BmnGlobalAlignment.cxx
BmnMille.cxx
BmnAlignCorrections.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h BMNALIGNMENT_HEADERS "${BMNALIGNMENT_SRCS}")

set(BMNALIGNMENT_LINKDEF LinkDef.h)
set(BMNALIGNMENT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnAlignmentDict.cxx) 

ROOT_GENERATE_DICTIONARY("${BMNALIGNMENT_HEADERS}" "${BMNALIGNMENT_LINKDEF}" "${BMNALIGNMENT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(BMNALIGNMENT_SRCS ${BMNALIGNMENT_SRCS} ${BMNALIGNMENT_DICTIONARY})


add_library(BmnAlignment SHARED ${BMNALIGNMENT_SRCS})
target_link_libraries(BmnAlignment ${ROOT_LIBRARIES})
set_target_properties(BmnAlignment PROPERTIES VERSION 0.0.1 SOVERSION 0)

################ install ###################
install(TARGETS BmnAlignment DESTINATION ${CMAKE_BINARY_DIR}/lib)
