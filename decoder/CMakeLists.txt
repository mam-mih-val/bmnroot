# Create a library called "libDecoder"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/unicom
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/database/elog_db
 ${CMAKE_SOURCE_DIR}/database/tango
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/common
 ${CMAKE_SOURCE_DIR}/cat
 ${CMAKE_SOURCE_DIR}/cat/vectors
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/gem
  ${CMAKE_SOURCE_DIR}/decoder
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR} 
 ${Boost_INCLUDE_DIRS}
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
 BmnAdcProcessor.cxx
 BmnDchRaw2Digit.cxx
 BmnMwpcRaw2Digit.cxx
 BmnZDCRaw2Digit.cxx
 BmnScWallRaw2Digit.cxx
 BmnFHCalRaw2Digit.cxx
 BmnHodoRaw2Digit.cxx
 BmnECALRaw2Digit.cxx
 BmnLANDRaw2Digit.cxx
 BmnTofCalRaw2Digit.cxx
 BmnGemRaw2Digit.cxx
 BmnTof2Raw2DigitNew.cxx
 BmnTof1Raw2Digit.cxx
 BmnSiliconRaw2Digit.cxx
 BmnRawDataDecoder.cxx
 BmnSlewingTOF700.cxx
 BmnTrigRaw2Digit.cxx
 BmnCscRaw2Digit.cxx
 BmnMscRaw2Digit.cxx
 WfmProcessor.cxx
 PronyFitter.cxx
)

if (CMAKE_BUILD_TYPE STREQUAL Release)
    ADD_DEFINITIONS(-DHAVE_SSE)
    SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS
            "-msse -O3")
    Message(STATUS "Decoder will be compiled in -O3 SSE mode")
else()
    Message(STATUS "Decoder will be compiled in debug SSE mode")
    SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS
            "-msse  ")
endif()

Set(HEADERS)
Set(LINKDEF DecoderLinkDef.h)
Set(LIBRARY_NAME Decoder)
Set(DEPENDENCIES CSC Gem Silicon BmnData UniDb ElogDb TangoData Base Minuit Core boost_program_options)

GENERATE_LIBRARY()
