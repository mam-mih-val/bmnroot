# Create a library called "libDecoder"

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/bmndata/common
 ${CMAKE_SOURCE_DIR}/csc
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/gem
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
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 BmnAdcProcessor.cxx
 BmnDchRaw2Digit.cxx
 BmnMwpcRaw2Digit.cxx
 BmnZDCRaw2Digit.cxx
 BmnECALRaw2Digit.cxx
 BmnLANDRaw2Digit.cxx
 BmnGemRaw2Digit.cxx
 BmnTof2Raw2DigitNew.cxx
 BmnTof1Raw2Digit.cxx
 BmnSiliconRaw2Digit.cxx
 BmnRawDataDecoder.cxx
 BmnSlewingTOF700.cxx
 BmnTrigRaw2Digit.cxx
 BmnCscRaw2Digit.cxx
 BmnMscRaw2Digit.cxx
)

Set(HEADERS)

Set(LINKDEF DecoderLinkDef.h)
Set(LIBRARY_NAME Decoder)
Set(DEPENDENCIES CSC Gem Silicon BmnData UniDb Base Minuit Core)

GENERATE_LIBRARY()
