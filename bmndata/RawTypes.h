#ifndef RAWTYPES_H
#define RAWTYPES_H


/***************** SET OF DAQ CONSTANTS *****************/
const UInt_t SYNC_EVENT = 0x2A50D5AF;
const UInt_t SYNC_EVENT_OLD = 0x2A502A50;
const UInt_t SYNC_STAT = 0x4A62B59D; // Statistics
const UInt_t SYNC_EOS = 0x4A624A62; // End Of Spill
const UInt_t SYNC_RUN_START = 0x72617453;
const UInt_t SYNC_RUN_STOP = 0x706F7453;
const UInt_t SYNC_RUN_NUMBER = 0x236E7552; // Run ID
const UInt_t SYNC_RUN_INDEX = 0x78646E49; // Event Builder Name
const UInt_t SYNC_JSON = 0x4E4F534A;
const size_t kWORDSIZE = sizeof (UInt_t);
const UShort_t kNBYTESINWORD = 4;

//FVME data types
const UInt_t kMODDATAMAX = 0x7;
const UInt_t kMODHEADER = 0x8;
const UInt_t kMODTRAILER = 0x9;
const UInt_t kEVHEADER = 0xA;
const UInt_t kEVTRAILER = 0xB;
const UInt_t kSPILLHEADER = 0xC;
const UInt_t kSPILLTRAILER = 0xD;
const UInt_t kSTATUS = 0xE;
const UInt_t kPADDING = 0xF;

//module ID
const UInt_t kTDC64V = 0x10; //DCH
const UInt_t kTDC64VHLE = 0x53;
const UInt_t kTDC72VHL = 0x12;
const UInt_t kTDC32VL = 0x11;
const UInt_t kTQDC16 = 0x09;
const UInt_t kTQDC16VS = 0x56;
const UInt_t kTQDC16VS_E = 0xD6;
const UInt_t kTRIG = 0xA;
const UInt_t kMSC16V = 0xF;
const UInt_t kMSC16VE_E = 0xD8;
const UInt_t kUT24VE_TRC = 0xE3;
const UInt_t kUT24VE = 0xC9;
const UInt_t kUT24VE_ = 0x49;
const UInt_t kADC64VE = 0xD4;
const UInt_t kADC64VE_XGE = 0xD9;
const UInt_t kADC64WR = 0xCA;
const UInt_t kHRB = 0xC2;
const UInt_t kTDC72VXS = 0xD0;
const UInt_t kFVME = 0xD1;
const UInt_t kTACQUILADAQ = 0xDA;
const UInt_t kU40VE_RC = 0x4C;

//event type trigger
const UInt_t kEVENTTYPESLOT = 12;
const UInt_t kWORDTAI = 2;
const UInt_t kWORDTRIG = 3;
const UInt_t kWORDAUX = 4;
const UInt_t kTRIGBEAM = 6;
const UInt_t kTRIGMINBIAS = 1;

const UInt_t TDC_EV_HEADER = 2;
const UInt_t TDC_EV_TRAILER = 3;
const UInt_t TDC_LEADING = 4;
const UInt_t TDC_TRAILING = 5;
const UInt_t TDC_ERROR = 6;

#pragma pack(push,1)

struct __attribute__ ((packed)) DeviceHeader {
    void Print() {
        printf("Serial   : %08X\n", Serial);
        printf("Length   : %8u\n", Len);
        printf("DeviceId : %8X\n", DeviceId);
    }
    uint32_t Serial;
    uint32_t Len : 24;
    uint8_t DeviceId : 8;
};

/**
 * M-Stream Header
 * https://afi.jinr.ru/MpdDeviceRawDataFormat
 */
struct __attribute__ ((packed)) MStreamHeader {
    void Print() {
        printf("Subtype   :   %8u\n", Subtype);
        printf("Length    :   %8u\n", Len);
        printf("CustomBits:   %8u\n", CustomBits);
    }
    uint8_t Subtype : 2;
    uint32_t Len : 22;
    uint8_t CustomBits : 8;
};

struct __attribute__ ((packed)) MStreamTAI {
    uint32_t TaiSec;
    uint8_t TaiFlags : 2;
    uint32_t TaiNSec : 30;
};

/**
 *  https://afi.jinr.ru/DataFormatMSC_ETH
 */
struct __attribute__ ((packed)) MSC16VE_EHeader {
    MStreamHeader Hdr;
    MStreamTAI Tai;
    uint8_t NCntrBits : 4;
    uint32_t : 24;
    uint8_t Version : 4;
    uint32_t SliceInt; // Slice interval [ns]
};

/**
 * TQDC-E Data Block header
 *  https://afi.jinr.ru/DataFormatTQDC16VSE
 */
struct __attribute__ ((packed)) TqdcDataHeader {
    uint16_t Len : 16;
    uint8_t AdcBits : 3;
    uint8_t : 5;
    uint8_t Chan : 4;
    uint8_t DataType : 4;
};


#pragma pack(pop)

#endif

