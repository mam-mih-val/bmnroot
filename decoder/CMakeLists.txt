# Create a library called "libDecoder" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/math
 ${CMAKE_SOURCE_DIR}/gem
 ${CMAKE_SOURCE_DIR}/silicon
 ${CMAKE_SOURCE_DIR}/globaltracking
 ${CMAKE_SOURCE_DIR}/uni_db
 ${CMAKE_SOURCE_DIR}/uni_db/db_classes
 ${CMAKE_SOURCE_DIR}/decoder
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
 BmnAdcProcessor.cxx
 BmnDchRaw2Digit.cxx
 BmnMwpcRaw2Digit.cxx
 BmnZDCRaw2Digit.cxx
 BmnECALRaw2Digit.cxx
 BmnLANDRaw2Digit.cxx
 BmnGemRaw2Digit.cxx
 BmnTof2Raw2Digit.cxx
 BmnTof2Raw2DigitNew.cxx
 BmnTof1Raw2Digit.cxx
 BmnSiliconRaw2Digit.cxx
 BmnRawDataDecoder.cxx
 BmnTrigRaw2Digit.cxx
 BmnCscRaw2Digit.cxx
)

Set(HEADERS)

Set(LINKDEF DecoderLinkDef.h)
Set(LIBRARY_NAME Decoder)
Set(DEPENDENCIES Core Base BmnData UniDb)

GENERATE_LIBRARY()
