# Create a library called "libBmnDecoder"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/database/uni_db
 ${CMAKE_SOURCE_DIR}/database/elog_db
 ${CMAKE_SOURCE_DIR}/database/tango
 ${CMAKE_SOURCE_DIR}/database/tools
 ${CMAKE_SOURCE_DIR}/base
 ${CMAKE_SOURCE_DIR}/base/data
 ${CMAKE_SOURCE_DIR}/base/raw
 ${CMAKE_SOURCE_DIR}/base/header
 ${CMAKE_SOURCE_DIR}/base/math
 ${CMAKE_SOURCE_DIR}/detectors/mwpc
 ${CMAKE_SOURCE_DIR}/detectors/csc
 ${CMAKE_SOURCE_DIR}/detectors/silicon
 ${CMAKE_SOURCE_DIR}/detectors/SiBT
 ${CMAKE_SOURCE_DIR}/detectors/gem
 ${CMAKE_SOURCE_DIR}/detectors/dch
 ${CMAKE_SOURCE_DIR}/detectors/tof1
 ${CMAKE_SOURCE_DIR}/detectors/tof
 ${CMAKE_SOURCE_DIR}/detectors/tofcal
 ${CMAKE_SOURCE_DIR}/detectors/ecal
 ${CMAKE_SOURCE_DIR}/detectors/zdc
 ${CMAKE_SOURCE_DIR}/detectors/scwall
 ${CMAKE_SOURCE_DIR}/detectors/hodo
 ${CMAKE_SOURCE_DIR}/detectors/fhcal
 ${CMAKE_SOURCE_DIR}/detectors/ndet
 ${CMAKE_SOURCE_DIR}/detectors/land
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/reconstruction/cat
 ${CMAKE_SOURCE_DIR}/reconstruction/cat/vectors
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
 BmnNdetRaw2Digit.cxx
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
 BmnProfRaw2Digit.cxx
 BmnProfAsic2Raw.cxx
 BmnMscRaw2Digit.cxx
 WfmProcessor.cxx
 PronyFitter.cxx
)

If (${CMAKE_BUILD_TYPE} STREQUAL Debug)
  add_compile_definitions(BUILD_DEBUG=${CMAKE_BUILD_TYPE})
EndIf(${CMAKE_BUILD_TYPE} STREQUAL Debug)

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
Set(LIBRARY_NAME BmnDecoder)
Set(DEPENDENCIES LAND FHCal Hodo ScWall Ndet Zdc Ecal TofCal TOF TOF1 CSC Gem Silicon
SiBT Dch Mwpc BmnBase DbTools UniDb ElogDb TangoData Base Minuit Core boost_program_options)

GENERATE_LIBRARY()
