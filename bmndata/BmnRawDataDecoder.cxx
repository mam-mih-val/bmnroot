#include "BmnRawDataDecoder.h"
#include <bitset>

/***************** SET OF DAQ CONSTANTS *****************/
const UInt_t kSYNC1 = 0x2A502A50;
const UInt_t kSYNC2 = 0x4A624A62;
const size_t kWORDSIZE = sizeof (UInt_t);
const Short_t kNBYTESINWORD = 4;

//VFME data types
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
const UInt_t kTDC64V = 0x10;
const UInt_t kTDC64VHLE = 0x53;
const UInt_t kTDC72VHL = 0x12;
const UInt_t kTRIG = 0xA;
const UInt_t kMSC = 0xF;
const UInt_t kUT24VE = 0x49;
const UInt_t kADC64VE = 0xD4;
const UInt_t kFVME = 0xD1;
/********************************************************/

using namespace std;

BmnRawDataDecoder::BmnRawDataDecoder() {
    fRunId = 0;
    fEventId = 0;
    fNevents = 0;
    fOutTree = NULL;
    fRootFile = NULL;
    fRawFile = NULL;
    sync = NULL;
    tdc = NULL;
    adc = NULL;
    msc = NULL;
    fRootFileName = "";
    fRawFileName = "";
}

BmnRawDataDecoder::BmnRawDataDecoder(TString file) {

    fRunId = TString(file(file.Length() - 8, 3)).Atoi();
    fEventId = 0;
    fNevents = 0;
    fOutTree = new TTree("BMN_RAW", "BMN_RAW");
    fRawFileName = file;
    fRawFile = fopen(fRawFileName, "rb");

    fRootFileName = Form("bmn_run%04d.root", fRunId);
    fRootFile = new TFile(fRootFileName, "recreate");

    sync = new TClonesArray("BmnSyncDigit");
    adc = new TClonesArray("BmnADC32Digit");
    tdc = new TClonesArray("BmnTDCDigit");
    msc = new TClonesArray("BmnMSCDigit");

    fOutTree->Branch("SYNC", &sync);
    fOutTree->Branch("ADC", &adc);
    fOutTree->Branch("TDC", &tdc);
    fOutTree->Branch("MSC", &msc);
}

BmnRawDataDecoder::~BmnRawDataDecoder() {
    //    fOutTree->Print();
    fOutTree->Write();
    fRootFile->Close();
    fclose(fRawFile);
}

BmnStatus BmnRawDataDecoder::ConvertRawToRoot() {
    UInt_t dat = 0;
    for (;;) {
        //if (fNevents == 4) break;
        if (fread(&dat, kWORDSIZE, 1, fRawFile) != 1) return kBMNERROR;
        if (dat == kSYNC1) { //search for start of event

            // read number of bytes in event
            if (fread(&dat, kWORDSIZE, 1, fRawFile) != 1) return kBMNERROR;
            dat = dat / kNBYTESINWORD + 1; // bytes --> words
            if (dat >= 100000) { // what the constant?
                printf("Wrong data size: %d\n", dat);
                return kBMNERROR;
            }

            //read array of current event data and process them
            if (fread(data, kWORDSIZE, dat, fRawFile) != dat) return kBMNERROR;
            fEventId = data[0];
            if (fEventId <= 0) continue; // skip bad events (it is possible, but what about 0?) 
            ProcessEvent(data, dat);
            fNevents++;
            fOutTree->Fill();
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ProcessEvent(UInt_t *d, UInt_t len) {

    sync->Clear();
    tdc->Clear();
    adc->Clear();
    msc->Clear();

    if (fEventId % 1000 == 0) cout << "Event #" << d[0] << " contains " << len << " words" << endl;

    UInt_t idx = 1;
    while (idx < len) {
        UInt_t serial = d[idx++];
        UInt_t id = (d[idx] >> 24);
        UInt_t payload = (d[idx++] & 0xFFFFFF) / kNBYTESINWORD;

        if (payload > 20000) {
            printf("[WARNING] Event %d:\n serial = 0x%06X\n id = Ox%02X\n payload = %d\n", fEventId, serial, id, payload);
            break;
        }
        switch (id) {
            case kADC64VE:
                Process_ADC64VE(&data[idx], payload, serial);
                break;
            case kFVME:
                Process_FVME(&data[idx], payload, serial);
                break;
        }
        idx += payload;
    }
}

BmnStatus BmnRawDataDecoder::Process_ADC64VE(UInt_t *d, UInt_t len, UInt_t serial) {
    const UChar_t kNCH = 64;
    const UChar_t kNSTAMPS = 32;

    UShort_t val[kNSTAMPS];
    for (Int_t i = 0; i < kNSTAMPS; ++i) val[i] = 0;

    UInt_t i = 0;
    while (i < len) {
        UInt_t subType = d[i] & 0x3;
        if (subType == 0) {
            i += 5; //skip unused words
            UInt_t iCh = 0;
            while (iCh < kNCH - 1) {
                iCh = d[i] >> 24;

                i += 3; // skip two timestamp words (they are empty)
                for (Int_t iWord = 0; iWord < 16; ++iWord) {
                    val[2 * iWord] = d[i + iWord] & 0xFFFF; //take 16 lower bits and put them into corresponded cell of data-array
                    val[2 * iWord + 1] = (d[i + iWord] >> 16) & 0xFFFF; //take 16 higher bits and put them into corresponded cell of data-array
                }

                TClonesArray& ar_adc = *adc;
                new(ar_adc[adc->GetEntriesFast()]) BmnADC32Digit(serial, iCh, val);
                i += 16; //skip 16 words (we've processed them)
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::Process_FVME(UInt_t *d, UInt_t len, UInt_t serial) {
    UInt_t modId = 0;
    UInt_t slot = 0;
    UInt_t type = 0;
    for (UInt_t i = 0; i < len; i++) {
        type = d[i] >> 28;
        switch (type) {
            case kEVHEADER:
            case kEVTRAILER:
            case kMODTRAILER:
            case kSPILLHEADER:
            case kSPILLTRAILER:
            case kSTATUS:
            case kPADDING:
                break;
            case kMODHEADER:
                modId = (d[i] >> 16) & 0x7F;
                slot = (d[i] >> 23) & 0x1F;
                break;
            default: //data
            {
                switch (modId) {
                    case kTDC64V:
                    case kTDC64VHLE:
                    case kTDC72VHL:
                        FillTDC(d, serial, slot, modId, i);
                        break;
                    case kMSC:
                        FillMSC(d, serial, i); //empty now
                        break;
                    case kTRIG:
                        FillTRIG(d, serial, i);
                        break;
                }
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t &idx) {
    UInt_t type = d[idx] >> 28;
    while (type != kMODTRAILER) { //data will be finished when module trailer appears 
        if (type == 4 || type == 5) { // 4 - leading, 5 - trailing
            UInt_t tdcId = (d[idx] >> 24) & 0xF;
            UInt_t time = (modId == kTDC64V) ? (d[idx] & 0x7FFFF) : ((d[idx] & 0x7FFFF) << 2) | ((d[idx] & 0x180000) >> 19);
            UInt_t channel = (modId == kTDC64V) ? (d[idx] >> 19) & 0x1F : (d[idx] >> 21) & 0x7;
            TClonesArray &ar_tdc = *tdc;
            new(ar_tdc[tdc->GetEntriesFast()]) BmnTDCDigit(serial, modId, slot, (type == 4), tdcId, channel, time);
        }
        idx++; //go to the next DATA-word
        type = d[idx] >> 28;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTRIG(UInt_t *d, UInt_t serial, UInt_t &idx) {
    UInt_t d0 = d[idx + 0];
    UInt_t d1 = d[idx + 1];
    UInt_t d2 = d[idx + 2];
    UInt_t d3 = d[idx + 3];
    if ((d0 >> 28) != 2 || (d1 >> 28) != 2 || (d2 >> 28) != 2 || (d3 >> 28) != 2) return kBMNERROR; //check TAI code 
    Long64_t ts_t0_s = -1;
    Long64_t ts_t0_ns = -1;
    Long64_t GlobalEvent = -1;

    if (((d1 >> 2) & 0x3) == 2) {
        ts_t0_ns = d0 & 0x0FFFFFFF | ((d1 & 0x3) << 28);
        ts_t0_s = ((d1 >> 4) & 0xFFFFFF) | ((d2 & 0xFFFF) << 24);
        GlobalEvent = ((d3 & 0x0FFFFFFF) << 12) | ((d2 >> 16) & 0xFFF);
    }

    TClonesArray &ar_sync = *sync;
    new(ar_sync[sync->GetEntriesFast()]) BmnSyncDigit(serial, GlobalEvent, ts_t0_s, ts_t0_ns);

    idx += 3; //skip next 3 words (we've processed them)
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillMSC(UInt_t *d, UInt_t serial, UInt_t &idx) {
    //                    UInt_t groupId = d[i] >> 28;
    //                    cout << groupId << endl;
    //                    UInt_t time = 0;
    //                    UInt_t cnt[16];
    //                    if (groupId < 4)
    //                        for (Int_t iCnt = 0; iCnt < 4; ++iCnt)
    //                            cnt[groupId * 4 + iCnt] = d[i] >> (iCnt * 7) & 0x7F;
    //                    else
    //                        time = d[i] & 28;
    //                    //                    TClonesArray &ar_msc = *msc;
    //                    //                    new(ar_msc[msc->GetEntriesFast()]) BmnTDCDigit(serial, 0, slot, (dataType == 4), channel, time);
}
