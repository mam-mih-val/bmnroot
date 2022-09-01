# Create a library called "libMpdGen"

EXEC_PROGRAM( gcc ARGS "-dumpversion" OUTPUT_VARIABLE GCC_VERSION )
STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]" "\\1" req_gcc_major_vers "${GCC_VERSION}")
STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]" "\\1" req_gcc_minor_vers "${GCC_VERSION}")
IF(${req_gcc_major_vers} MATCHES "4" AND NOT ${req_gcc_minor_vers} MATCHES "0")
ENDIF(${req_gcc_major_vers} MATCHES "4" AND NOT ${req_gcc_minor_vers} MATCHES "0")

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/config
 ${CMAKE_SOURCE_DIR}/generators
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
)
if(PLUTO_FOUND)
set(SYSTEM_INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRECTORIES} ${PLUTO_INCLUDE_DIR})
EndIf(PLUTO_FOUND)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)
if(PLUTO_FOUND)
 set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${PLUTO_LIBRARY_DIR})
EndIf(PLUTO_FOUND)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
MpdMCEventHeader.cxx
MpdLAQGSMGenerator.cxx
MpdHypYPtGenerator.cxx
MpdRainGenerator.cxx
MpdPHSDGenerator.cxx
MpdUrqmdGenerator.cxx
MpdGetNumEvents.cxx
MpdHistoGenerator.cxx
Mpd3fdGenerator.cxx
MpdVHLLEGenerator.cxx
MpdDCMSMMGenerator.cxx
)
if(PLUTO_FOUND)
#AZ set(SRCS ${SRCS} FairPlutoReactionGenerator.cxx MpdPlutoGenerator.cxx)
set(SRCS ${SRCS} MpdPlutoGenerator.cxx)
EndIf(PLUTO_FOUND)

Set(HEADERS)
if(PLUTO_FOUND)
 Set(LINKDEF GenLinkDefPluto.h)
else(PLUTO_FOUND)
 Set(LINKDEF GenLinkDef.h)
EndIf(PLUTO_FOUND)
Set(LIBRARY_NAME MpdGen)
Set(DEPENDENCIES Base EG MathCore Physics RIO Core BmnBase)

GENERATE_LIBRARY()

add_executable (show_event_count show_event_count.cxx)
target_link_libraries(show_event_count MpdGen ${ROOT_LIBRARIES})

INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/show_event_count DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
