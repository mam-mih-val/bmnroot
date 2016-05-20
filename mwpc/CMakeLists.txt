
# Create a library called "libMWPC" which includes the source files given in
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
${CMAKE_SOURCE_DIR}/bmnbase
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/mwpc
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(MWPC1_SRCS
BmnMWPC1.cxx 
BmnMWPC1Point.cxx  
BmnMWPC1ContFact.cxx  
BmnMWPC1Geo.cxx 
BmnMWPC1GeoPar.cxx
BmnMwpcHitProducer.cxx
BmnMwpcDigitizer.cxx
BmnMwpcHitFinder.cxx
)

set(MWPC2_SRCS
BmnMWPC2.cxx 
BmnMWPC2Point.cxx  
BmnMWPC2ContFact.cxx  
BmnMWPC2Geo.cxx 
BmnMWPC2GeoPar.cxx
)

set(MWPC3_SRCS
BmnMWPC3.cxx 
BmnMWPC3Point.cxx  
BmnMWPC3ContFact.cxx  
BmnMWPC3Geo.cxx 
BmnMWPC3GeoPar.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MWPC1_HEADERS "${MWPC1_SRCS}")
CHANGE_FILE_EXTENSION(*.cxx *.h MWPC2_HEADERS "${MWPC2_SRCS}")
CHANGE_FILE_EXTENSION(*.cxx *.h MWPC3_HEADERS "${MWPC3_SRCS}")

set(MWPC1_LINKDEF BmnMWPC1LinkDef.h)
set(MWPC1_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnMWPC1Dict.cxx) 

set(MWPC2_LINKDEF BmnMWPC2LinkDef.h)
set(MWPC2_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnMWPC2Dict.cxx) 

set(MWPC3_LINKDEF BmnMWPC3LinkDef.h)
set(MWPC3_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/BmnMWPC3Dict.cxx) 

ROOT_GENERATE_DICTIONARY("${MWPC1_HEADERS}" "${MWPC1_LINKDEF}" "${MWPC1_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
ROOT_GENERATE_DICTIONARY("${MWPC2_HEADERS}" "${MWPC2_LINKDEF}" "${MWPC2_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
ROOT_GENERATE_DICTIONARY("${MWPC3_HEADERS}" "${MWPC3_LINKDEF}" "${MWPC3_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(MWPC1_SRCS ${MWPC1_SRCS} ${MWPC1_DICTIONARY})
set(MWPC2_SRCS ${MWPC2_SRCS} ${MWPC2_DICTIONARY})
set(MWPC3_SRCS ${MWPC3_SRCS} ${MWPC3_DICTIONARY})

add_library(MWPC1 SHARED ${MWPC1_SRCS})
target_link_libraries(MWPC1 ${ROOT_LIBRARIES})
set_target_properties(MWPC1 PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

add_library(MWPC2 SHARED ${MWPC2_SRCS})
target_link_libraries(MWPC2 ${ROOT_LIBRARIES})
set_target_properties(MWPC2 PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

add_library(MWPC3 SHARED ${MWPC3_SRCS})
target_link_libraries(MWPC3 ${ROOT_LIBRARIES})
set_target_properties(MWPC3 PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS MWPC1 DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS MWPC2 DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS MWPC3 DESTINATION ${CMAKE_BINARY_DIR}/lib)

