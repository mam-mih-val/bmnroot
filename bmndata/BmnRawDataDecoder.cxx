
#include "BmnRawDataDecoder.h"

//#include <pthread.h>
#include <sys/stat.h>

/***************** SET OF DAQ CONSTANTS *****************/
const UInt_t kSYNC1 = 0x2A502A50;
const UInt_t kSYNC2 = 0x4A624A62;
const UInt_t kRUNNUMBERSYNC = 0x236E7552;
const size_t kWORDSIZE = sizeof (UInt_t);
const Short_t kNBYTESINWORD = 4;

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
const UInt_t kTRIG = 0xA;
const UInt_t kMSC = 0xF;
const UInt_t kUT24VE = 0x49;
const UInt_t kADC64VE = 0xD4;
const UInt_t kHRB = 0xC2;
const UInt_t kFVME = 0xD1;
const UInt_t kU40VE_RC = 0x4C;

//event type trigger
const UInt_t kEVENTTYPESLOT = 12;
const UInt_t kGEMTRIGTYPE = 3;
const UInt_t kTRIGBEAM = 6;
const UInt_t kTRIGMINBIAS = 1;

#define ANSI_COLOR_RED   "\x1b[91m"
#define ANSI_COLOR_BLUE  "\x1b[94m"
#define ANSI_COLOR_RESET "\x1b[0m"
/********************************************************/

using namespace std;

BmnRawDataDecoder::BmnRawDataDecoder() {
    fRunId = 0;
    fPeriodId = 0;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = 0;
    fLengthRawFile = 0;
    fCurentPositionRawFile = 0;
    t0 = NULL;
    bc1 = NULL;
    bc2 = NULL;
    veto = NULL;
    runHeaderDAQ = NULL;
    eventHeaderDAQ = NULL;
    runHeader = NULL;
    eventHeader = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fT0Time = 0.0;
    fRawTree = NULL;
    fDigiTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
    fDigiFileOut = NULL;
    sync = NULL;
    tdc = NULL;
    hrb = NULL;
    adc32 = NULL;
    adc128 = NULL;
    msc = NULL;
    dch = NULL;
    mwpc = NULL;
    tof400 = NULL;
    tof700 = NULL;
    gem = NULL;
    silicon = NULL;
    fRootFileName = "";
    fRawFileName = "";
    fDigiFileName = "";
    fDchMapFileName = "";
    fMwpcMapFileName = "";
    fTrigMapFileName = "";
    fTrigINLFileName = "";
    fGemMapFileName = "";
    fTof400StripMapFileName = "";
    fTof400PlaceMapFileName = "";
    fTof700MapFileName = "";
    fDat = 0;
    fGemMapper = NULL;
    fDchMapper = NULL;
    fTrigMapper = NULL;
    fTof400Mapper = NULL;
    fTof700Mapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fPedoCounter = 0;
    fGemMap = NULL;
}

BmnRawDataDecoder::BmnRawDataDecoder(TString file, ULong_t nEvents, ULong_t period) {

    t0 = NULL;
    runHeaderDAQ = NULL;
    eventHeaderDAQ = NULL;
    runHeader = NULL;
    eventHeader = NULL;
    bc2 = NULL;
    bc1 = NULL;
    veto = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fT0Time = 0.0;
    fRawTree = NULL;
    fDigiTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
    fDigiFileOut = NULL;
    sync = NULL;
    hrb = NULL;
    tdc = NULL;
    adc32 = NULL;
    adc128 = NULL;
    msc = NULL;
    dch = NULL;
    tof400 = NULL;
    tof700 = NULL;
    gem = NULL;
    silicon = NULL;
    fRawFileName = file;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = nEvents;
    fPeriodId = period;
    fRunId = GetRunIdFromFile(fRawFileName);
    fRootFileName = Form("bmn_run%04d_raw.root", fRunId);
    fDigiFileName = Form("bmn_run%04d_digi.root", fRunId);
    fDchMapFileName = "";
    fMwpcMapFileName = "";
    fTrigMapFileName = "";
    fTrigINLFileName = "";
    fGemMapFileName = "";
    fTof400StripMapFileName = "";
    fTof400PlaceMapFileName = "";
    fTof700MapFileName = "";
    fDat = 0;
    fGemMapper = NULL;
    fDchMapper = NULL;
    fTrigMapper = NULL;
    fTof400Mapper = NULL;
    fTof700Mapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fPedoCounter = 0;
    fGemMap = NULL;
    Int_t fEntriesInGlobMap = 0;
    UniDbDetectorParameter* mapPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_global_mapping", fPeriodId, fRunId);
    if (mapPar != NULL) mapPar->GetGemMapArray(fGemMap, fEntriesInGlobMap);

    for (Int_t i = 0; i < fEntriesInGlobMap; ++i)
        if (find(fGemSerials.begin(), fGemSerials.end(), fGemMap[i].serial) == fGemSerials.end())
            fGemSerials.push_back(fGemMap[i].serial);
    fNGemSerials = fGemSerials.size();

    Int_t nEntries = 1;
    mapPar = UniDbDetectorParameter::GetDetectorParameter("T0", "T0_global_mapping", fPeriodId, fRunId);
    if (mapPar != NULL) mapPar->GetTriggerMapArray(fT0Map, nEntries);
    else cerr << "No TO map found in DB" << endl;
    delete mapPar;

}

BmnRawDataDecoder::~BmnRawDataDecoder() {
}

BmnStatus BmnRawDataDecoder::ConvertRawToRoot() {

    printf(ANSI_COLOR_RED "\n================ CONVERTING ================\n" ANSI_COLOR_RESET);

    fRawFileIn = fopen(fRawFileName, "rb");
    if (fRawFileIn == NULL) {
        printf("\n!!!!!\ncannot open file %s\nConvertRawToRoot are stopped\n!!!!!\n\n", fRawFileName.Data());
        return kBMNERROR;
    }

    InitConverter();

    fseeko64(fRawFileIn, 0, SEEK_END);
    fLengthRawFile = ftello64(fRawFileIn);
    rewind(fRawFileIn);
    printf("\nRAW FILE: ");
    printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET, fRawFileName.Data());
    printf("\nRAW FILE LENGTH: ");
    printf(ANSI_COLOR_BLUE "%.3f MB\n" ANSI_COLOR_RESET, fLengthRawFile / 1024. / 1024.);
    fRootFileOut = new TFile(fRootFileName, "recreate");

    for (;;) {
        if (fMaxEvent > 0 && fNevents == fMaxEvent) break;
        //if (fread(&dat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
        fread(&fDat, kWORDSIZE, 1, fRawFileIn);
        fCurentPositionRawFile = ftello64(fRawFileIn);
        if (fCurentPositionRawFile >= fLengthRawFile) break;
        if (fDat == kSYNC1) { //search for start of event
            // read number of bytes in event
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
            fDat = fDat / kNBYTESINWORD + 1; // bytes --> words
            if (fDat >= 100000) { // what the constant?
                printf("Wrong data size: %d:  skip this event\n", fDat);
                fread(data, kWORDSIZE, fDat, fRawFileIn);
            } else {

                //read array of current event data and process them
                if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) return kBMNERROR;
                fEventId = data[0];
                if (fEventId <= 0) continue; // skip bad events
                ProcessEvent(data, fDat);
                fNevents++;
                fRawTree->Fill();
            }
        }
    }

    printf("fPedoCounter = %d\n", fPedoCounter);

    fRawTree->Branch("RunHeader", &runHeaderDAQ);
    runHeaderDAQ->SetRunId(fRunId);
    runHeaderDAQ->SetStartTime(TTimeStamp(time_t(fTimeStart_s), fTimeStart_ns));
    runHeaderDAQ->SetFinishTime(TTimeStamp(time_t(fTime_s), fTime_ns));
    fRawTree->Fill();

    fCurentPositionRawFile = ftello64(fRawFileIn);
    printf("Readed %d events; %lld bytes (%.3f Mb)\n\n", fNevents, fCurentPositionRawFile, fCurentPositionRawFile / 1024. / 1024.);

    fRawTree->Write();
    fRootFileOut->Close();
    fclose(fRawFileIn);

    delete sync;
    delete adc32;
    delete adc128;
    delete hrb;
    delete tdc;
    delete msc;

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::InitConverter(deque<UInt_t> *dq) {
    fDataQueue = dq;
    fRawTree = new TTree("BMN_RAW", "BMN_RAW");
    //    fRawFileIn = fopen(fRawFileName, "rb");
    //    if (fDataQueue == NULL) {
    //        printf("\n!!!!!\ncannot open stream\nConvertRawToRoot are stopped\n!!!!!\n\n");
    //        return kBMNERROR;
    //    }
    //    fRootFileOut = new TFile(fRootFileName, "recreate");
    //    fseeko64(fRawFileIn, 0, SEEK_END);
    //    fLengthRawFile = ftello64(fRawFileIn);
    //    rewind(fRawFileIn);
    fLengthRawFile = fDataQueue->size();
    //    printf("\nRawData File %s;\nLength RawData - %lld bytes (%.3f Mb)\n", fRawFileName.Data(), fLengthRawFile, fLengthRawFile / 1024. / 1024.);
    //    printf("RawRoot File %s\n\n", fRootFileName.Data());

    //    fRunId = TString(file(fRawFileName.Length() - 8, 3)).Atoi();
    fDigiFileName = Form("bmn_run%04d_digi.root", fRunId);

    InitConverter();
    return kBMNSUCCESS;

}

BmnStatus BmnRawDataDecoder::InitConverter() {
    fRawTree = new TTree("BMN_RAW", "BMN_RAW");
    sync = new TClonesArray("BmnSyncDigit");
    adc32 = new TClonesArray("BmnADC32Digit");
    adc128 = new TClonesArray("BmnADC128Digit");
    tdc = new TClonesArray("BmnTDCDigit");
    hrb = new TClonesArray("BmnHRBDigit");
    msc = new TClonesArray("BmnMSCDigit");
    eventHeaderDAQ = new TClonesArray("BmnEventHeader");
    runHeaderDAQ = new BmnRunHeader();

    fRawTree->Branch("SYNC", &sync);
    fRawTree->Branch("ADC32", &adc32);
    fRawTree->Branch("ADC128", &adc128);
    fRawTree->Branch("TDC", &tdc);
    fRawTree->Branch("HRB", &hrb);
    fRawTree->Branch("MSC", &msc);
    fRawTree->Branch("EventHeader", &eventHeaderDAQ);
    //    fRawTree->Branch("RunHeader", &runHeaderDAQ);
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::wait_stream(deque<UInt_t> *que, Int_t len) {
    Int_t t;
    Int_t dt = 10000;
    while (que->size() < len) {
        usleep(dt);
        t += dt;
        if (t >= WAIT_LIMIT)
            return kBMNERROR;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::wait_file(Int_t len) {
    Long_t pos = ftello64(fRawFileIn);
    Int_t t = 0;
    Int_t dt = 10000;
    while (fLengthRawFile < pos + len) {
        gSystem->ProcessEvents();
        usleep(dt);
        fseeko64(fRawFileIn, 0, SEEK_END);
        fLengthRawFile = ftello64(fRawFileIn);
        fseeko64(fRawFileIn, pos - fLengthRawFile, SEEK_CUR);
        t += dt;
        if (t >= WAIT_LIMIT)
            return kBMNERROR;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ConvertRawToRootIterate() {
    //    fRawTree->Clear();
    //    if (wait_stream(fDataQueue, 2) == kBMNERROR)
    //        return kBMNTIMEOUT;
    //    fDat = fDataQueue->front();
    //    ((mutex*) fDataMutex)->lock();
    //    fDataQueue->pop_front();
    //    ((mutex*) fDataMutex)->unlock();
    //    if (fDat == kSYNC1) { //search for start of event
    //        // read number of bytes in event
    //        fDat = fDataQueue->front();
    //        ((mutex*) fDataMutex)->lock();
    //        fDataQueue->pop_front();
    //        ((mutex*) fDataMutex)->unlock();
    //        if (wait_stream(fDataQueue, fDat) == kBMNERROR)
    //            return kBMNTIMEOUT;
    //        fDat = fDat / kNBYTESINWORD + 1; // bytes --> words
    //        if (fDat * kNBYTESINWORD >= 100000) { // what the constant?
    //            printf("Wrong data size: %d:  skip this event\n", fDat);
    //            //                fread(data, kWORDSIZE, dat, fRawFileIn);
    //            ((mutex*) fDataMutex)->lock();
    //            fDataQueue->erase(fDataQueue->begin(), fDataQueue->begin() + fDat * kNBYTESINWORD);
    //            ((mutex*) fDataMutex)->unlock();
    //            return kBMNERROR;
    //        } else {
    //            //read array of current event data and process them
    //            if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) return kBMNERROR;
    //            ((mutex*) fDataMutex)->lock();
    //            for (Int_t iByte = 0; iByte < fDat * kNBYTESINWORD; iByte++) {
    //                data[iByte] = fDataQueue->front();
    //                fDataQueue->pop_front();
    //            }
    //            ((mutex*) fDataMutex)->unlock();
    //            fEventId = data[0];
    //            if (fEventId <= 0) return kBMNERROR; // continue; // skip bad events (it is possible, but what about 0?) 
    //            ProcessEvent(data, fDat);
    //            fNevents++;
    //            fRawTree->Fill();
    //        }
    //    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ConvertRawToRootIterateFile() {
    //        if (fMaxEvent > 0 && fNevents == fMaxEvent) break;
    if (wait_file(4 * kWORDSIZE) == kBMNERROR)
        return kBMNTIMEOUT;
    fCurentPositionRawFile = ftello64(fRawFileIn);
    fread(&fDat, kWORDSIZE, 1, fRawFileIn);
    if (fDat)
        //printf("dat %d\n", fDat);
        if (fDat == kRUNNUMBERSYNC) {
            printf("RunNumberSync\n");
            syncCounter++;
            if (syncCounter > 1) {
                cout << "Finish by SYNC" << endl;
                return kBMNFINISH;
            }
            fread(&fDat, kWORDSIZE, 1, fRawFileIn); //skip word
            //            fread(&fRunId, kWORDSIZE, 1, fRawFileIn);
        }
    if (fDat == kSYNC1) { //search for start of event
        // read number of bytes in event
        //printf("kSYNC1\n");
        if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
        fDat = fDat / kNBYTESINWORD + 1; // bytes --> words
        //        if (fDat * kNBYTESINWORD >= 100000) { // what the constant?
        //            printf("Wrong data size: %d:  skip this event\n", fDat);
        //            fread(data, kWORDSIZE, fDat, fRawFileIn);
        //        } else {
        //read array of current event data and process them
        if (wait_file(fDat * kNBYTESINWORD * kWORDSIZE) == kBMNERROR)
            return kBMNTIMEOUT;
        if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) {
            printf("finish by length\n");
            return kBMNFINISH;
        }
        fEventId = data[0];
        if (fEventId <= 0) {
            printf("bad event #%d\n", fEventId);
            return kBMNERROR; // continue; // skip bad events (it is possible, but what about 0?) 
        }
        //            printf("process event #%d\n", fEventId);
        ProcessEvent(data, fDat);
        fNevents++;
        fRawTree->Fill();
        //        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::DisposeConverter() {
    fCurentPositionRawFile = ftello64(fRawFileIn);
    printf("Readed %d events; %lld bytes (%.3f Mb)\n\n", fNevents, fCurentPositionRawFile, fCurentPositionRawFile / 1024. / 1024.);
    fRawTree->Branch("RunHeader", &runHeaderDAQ);
    runHeaderDAQ->SetRunId(fRunId);
    runHeaderDAQ->SetStartTime(TTimeStamp(time_t(fTimeStart_s), fTimeStart_ns));
    runHeaderDAQ->SetFinishTime(TTimeStamp(time_t(fTime_s), fTime_ns));

    //    fRawTree->Write();
    //    fRootFileOut->Close();
    //    fclose(fRawFileIn);

    //    delete sync;
    //    delete adc;
    //    delete tdc;
    //    delete msc;

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ProcessEvent(UInt_t *d, UInt_t len) {

    sync->Clear();
    tdc->Clear();
    hrb->Clear();
    adc32->Clear();
    adc128->Clear();
    msc->Clear();
    eventHeaderDAQ->Clear();

    if (fEventId % 100 == 0) {
        printf(ANSI_COLOR_BLUE "[%.2f%%]   " ANSI_COLOR_RESET, fCurentPositionRawFile * 100.0 / fLengthRawFile);
        printf("EVENT:%d   RUN:%d\n", d[0], fRunId);
    }

    Long64_t idx = 1;
    BmnEventType evType = kBMNPAYLOAD;
    BmnTriggerType trigType = kBMNBEAM;

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
            {
                Bool_t isGem = kFALSE;
                for (Int_t iSer = 0; iSer < fNGemSerials; ++iSer)
                    if (serial == fGemSerials[iSer]) {
                        isGem = kTRUE;
                        break;
                    }
                if (isGem)
                    Process_ADC64VE(&data[idx], payload, serial, 32, adc32);
                //else
                //Process_ADC64VE(&data[idx], payload, serial, 128, adc128);
                break;
            }
            case kFVME:
                Process_FVME(&data[idx], payload, serial, evType, trigType);
                break;
            case kHRB:
                Process_HRB(&data[idx], payload, serial);
                break;
        }
        idx += payload;
    }
    new((*eventHeaderDAQ)[eventHeaderDAQ->GetEntriesFast()]) BmnEventHeader(fRunId, fEventId, TTimeStamp(time_t(fTime_s), fTime_ns), evType, trigType);
}

BmnStatus BmnRawDataDecoder::Process_ADC64VE(UInt_t *d, UInt_t len, UInt_t serial, UInt_t nSmpl, TClonesArray *arr) {
    const UChar_t kNCH = 64;
    const UChar_t kNSTAMPS = nSmpl;

    UInt_t val[kNSTAMPS];
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
                for (Int_t iWord = 0; iWord < kNSTAMPS / 2; ++iWord) {
                    val[2 * iWord] = d[i + iWord] & 0xFFFF; //take 16 lower bits and put them into corresponded cell of data-array
                    val[2 * iWord + 1] = (d[i + iWord] >> 16) & 0xFFFF; //take 16 higher bits and put them into corresponded cell of data-array
                }

                TClonesArray& ar_adc = *arr;
                if (iCh >= 0 && iCh < kNCH) {
                    if (kNSTAMPS == ADC128_N_SAMPLES)
                        new(ar_adc[arr->GetEntriesFast()]) BmnADC128Digit(serial, iCh, val);
                    else if (kNSTAMPS == ADC32_N_SAMPLES)
                        new(ar_adc[arr->GetEntriesFast()]) BmnADC32Digit(serial, iCh, val);
                }
                i += (kNSTAMPS / 2); //skip words (we've processed them)
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::Process_FVME(UInt_t *d, UInt_t len, UInt_t serial, BmnEventType &evType, BmnTriggerType &trType) {
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
                    case kTDC32VL:
                        FillTDC(d, serial, slot, modId, i);
                        break;
                    case kMSC:
                        FillMSC(d, serial, i); //empty now
                        break;
                    case kTRIG:
                        FillSYNC(d, serial, i);
                        break;
                    case kU40VE_RC:
                        if (fPeriodId > 4 && type == kGEMTRIGTYPE && slot == kEVENTTYPESLOT) {
                            trType = ((d[i] & 0x7) == kTRIGMINBIAS) ? kBMNMINBIAS : kBMNBEAM;
                            evType = ((d[i] & 0x8) >> 3) ? kBMNPEDESTAL : kBMNPAYLOAD;
                            if (evType == kBMNPEDESTAL)
                                fPedoCounter++;
                        }
                        break;
                }
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::Process_HRB(UInt_t *d, UInt_t len, UInt_t serial) {
    UInt_t evId = d[0];
    UInt_t tH = d[1];
    UInt_t tL = d[2];
    UInt_t nWords = 4; //4 words per plane (per 96 channels, why 4 words - 3 is enough???)
    UInt_t nSmpl = (len - 3) / nWords; //3 words are read now. Why divide by 4 (To ask Vadim)

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        for (Int_t iWord = 0; iWord < nWords; ++iWord) {
            UInt_t word32 = d[3 + iWord + iSmpl * nWords];
            for (Int_t iCh = 0; iCh < 32; ++iCh) {
                if ((bitset<32>(word32))[iCh]) {
                    TClonesArray &ar_hrb = *hrb;
                    new(ar_hrb[hrb->GetEntriesFast()]) BmnHRBDigit(serial, iCh + 32 * iWord, iSmpl, tH, tL);
                }
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t & idx) {
    UInt_t type = d[idx] >> 28;
    while (type != kMODTRAILER) { //data will be finished when module trailer appears 
        if (type == 4 || type == 5) { // 4 - leading, 5 - trailing
            UInt_t tdcId = (d[idx] >> 24) & 0xF;
            UInt_t time = (modId == kTDC64V) ? (d[idx] & 0x7FFFF) : ((d[idx] & 0x7FFFF) << 2) | ((d[idx] & 0x180000) >> 19);
            UInt_t channel = (modId == kTDC64V) ? (d[idx] >> 19) & 0x1F : (d[idx] >> 21) & 0x7;
            //if (modId == kTDC64V && tdcId == 2) channel += 32;
            TClonesArray &ar_tdc = *tdc;
            new(ar_tdc[tdc->GetEntriesFast()]) BmnTDCDigit(serial, modId, slot, (type == 4), channel, tdcId, time);
        }
        idx++; //go to the next DATA-word
        type = d[idx] >> 28;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillSYNC(UInt_t *d, UInt_t serial, UInt_t & idx) {
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

    fTime_ns = ts_t0_ns;
    fTime_s = ts_t0_s;

    if (fEventId == 1) {
        fTimeStart_s = ts_t0_s;
        fTimeStart_ns = ts_t0_ns;
    }

    TClonesArray &ar_sync = *sync;
    new(ar_sync[sync->GetEntriesFast()]) BmnSyncDigit(serial, GlobalEvent, ts_t0_s, ts_t0_ns);

    idx += 3; //skip next 3 words (we've processed them)
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::DecodeDataToDigi() {

    printf(ANSI_COLOR_RED "================= DECODING =================\n" ANSI_COLOR_RESET);

    fRootFileIn = new TFile(fRootFileName, "READ");
    if (fRootFileIn->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s \nDecodeDataToDigi are stopped\n!!!!\n", fRootFileName.Data());
        return kBMNERROR;
    } else {
        printf("\nINPUT ROOT FILE: ");
        printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET, fRootFileName.Data());
        printf("\nOUTPUT DIGI FILE: ");
        printf(ANSI_COLOR_BLUE "%s\n\n" ANSI_COLOR_RESET, fDigiFileName.Data());
    }
    fRawTree = (TTree *) fRootFileIn->Get("BMN_RAW");
    tdc = new TClonesArray("BmnTDCDigit");
    hrb = new TClonesArray("BmnHRBDigit");
    sync = new TClonesArray("BmnSyncDigit");
    adc32 = new TClonesArray("BmnADC32Digit");
    adc128 = new TClonesArray("BmnADC128Digit");
    eventHeaderDAQ = new TClonesArray("BmnEventHeader");
    //runHeaderDAQ = new TClonesArray("BmnRunHeader");
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("HRB", &hrb);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("ADC32", &adc32);
    fRawTree->SetBranchAddress("ADC128", &adc128);
    fRawTree->SetBranchAddress("EventHeader", &eventHeaderDAQ);

    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    InitDecoder();
    BmnEventType curEventType = kBMNPAYLOAD;
    BmnEventType prevEventType = curEventType;

    if (fTof700Mapper) fTof700Mapper->BookSlewing();

    for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 100 == 0) {
            printf(ANSI_COLOR_BLUE "[%.2f%%]   " ANSI_COLOR_RESET, iEv * 100.0 / fNevents);
            printf("EVENT:%d   RUN:%d\n", iEv, fRunId);
        }
        ClearArrays();

        fRawTree->GetEntry(iEv);
        FillTimeShiftsMap();

        BmnEventHeader* headDAQ = (BmnEventHeader*) eventHeaderDAQ->At(0);
        if (!headDAQ) continue;
        curEventType = headDAQ->GetType();
        new((*eventHeader)[eventHeader->GetEntriesFast()]) BmnEventHeader(headDAQ->GetRunId(), headDAQ->GetEventId(), headDAQ->GetEventTime(), curEventType, headDAQ->GetTrig());

        fTrigMapper->FillEvent(tdc, t0, bc1, bc2, veto, fd, bd, fT0Time, &fT0Width);

        if (curEventType == kBMNPEDESTAL) {
            if (fPedEvCntr == N_EV_FOR_PEDESTALS - 1) continue;
            CopyDataToPedMap(adc32, fPedEvCntr);
            fPedEvCntr++;
        } else { // payload
            if (prevEventType == kBMNPEDESTAL) {
                if (fPedEvCntr == N_EV_FOR_PEDESTALS - 1) {
                    fGemMapper->RecalculatePedestals();
                    fPedEvCntr = 0;
                }
            }
            fGemMapper->FillEvent(adc32, gem);
            fSiliconMapper->FillEvent(adc128, silicon);
            fDchMapper->FillEvent(tdc, &fTimeShifts, dch, fT0Time);
            fMwpcMapper->FillEvent(hrb, mwpc);
            fTof400Mapper->FillEvent(tdc, tof400);
            fTof700Mapper->fillEvent(tdc, &fTimeShifts, fT0Time, fT0Width, tof700);
            if (iEv == fNevents - 1) {
                fDigiTree->Branch("RunHeader", &runHeader);
                if (runHeaderDAQ) {
                    UInt_t sT = runHeaderDAQ->GetStartTime().GetTime();
                    UInt_t fT = runHeaderDAQ->GetFinishTime().GetTime();
                    UInt_t sD = runHeaderDAQ->GetStartTime().GetDate();
                    UInt_t fD = runHeaderDAQ->GetFinishTime().GetDate();

                    runHeader->SetRunId(runHeaderDAQ->GetRunId());
                    runHeader->SetStartTime(runHeaderDAQ->GetStartTime());
                    runHeader->SetFinishTime(runHeaderDAQ->GetFinishTime());

                    printf(ANSI_COLOR_RED "\n=============== RUN" ANSI_COLOR_RESET);
                    printf(ANSI_COLOR_BLUE " %04d " ANSI_COLOR_RESET, runHeader->GetRunId());
                    printf(ANSI_COLOR_RED "SUMMARY ===============\n" ANSI_COLOR_RESET);
                    printf("START (event 1):\t%d/%02d/%02d\t", sD / 10000, sD % 10000 / 100, sD % 100);
                    printf("%02d:%02d:%02d\n", sT / 10000, sT % 10000 / 100, sT % 100);
                    printf("FINISH (event %d):\t%d/%02d/%02d\t", fEventId, fD / 10000, fD % 10000 / 100, fD % 100);
                    printf("%02d:%02d:%02d\n", fT / 10000, fT % 10000 / 100, fT % 100);
                    printf(ANSI_COLOR_RED "================================================\n" ANSI_COLOR_RESET);
                }
            }
            fDigiTree->Fill();
        }
        prevEventType = curEventType;

    }
    fDigiTree->Write();
    fDigiFileOut->Close();
    fRootFileIn->Close();

    DisposeDecoder();

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::InitDecoder() {
    //    fRootFileIn = new TFile(fRootFileName, "READ");
    //    if (fRootFileIn->IsOpen() == false) {
    //        printf("\n!!!!\ncannot open file %s \nDecodeDataToDigi are stopped\n!!!!\n", fRootFileName.Data());
    //        return kBMNERROR;
    //    } else {
    //        printf("\nInput root file: %s;\nOutput digi file: %s;\n", fRootFileName.Data(), fDigiFileName.Data());
    //    }
    //    fRawTree = (TTree *) fRootFileIn->Get("BMN_RAW");
    //    tdc = new TClonesArray("BmnTDCDigit");
    //    sync = new TClonesArray("BmnSyncDigit");
    //    adc = new TClonesArray("BmnADC32Digit");
    //    headerDAQ = new TClonesArray("BmnEventHeader");
    //    fRawTree->SetBranchAddress("TDC", &tdc);
    //    fRawTree->SetBranchAddress("SYNC", &sync);
    //    fRawTree->SetBranchAddress("ADC", &adc);
    //    fRawTree->SetBranchAddress("EventHeader", &headerDAQ);

    //    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    fDigiTree = new TTree("cbmsim", "bmn_digit");

    gem = new TClonesArray("BmnGemStripDigit");
    silicon = new TClonesArray("BmnSiliconDigit");
    tof400 = new TClonesArray("BmnTof1Digit");
    tof700 = new TClonesArray("BmnTof2Digit");
    dch = new TClonesArray("BmnDchDigit");
    mwpc = new TClonesArray("BmnMwpcDigit");
    t0 = new TClonesArray("BmnTrigDigit");
    bc1 = new TClonesArray("BmnTrigDigit");
    bc2 = new TClonesArray("BmnTrigDigit");
    bd = new TClonesArray("BmnTrigDigit");
    fd = new TClonesArray("BmnTrigDigit");
    veto = new TClonesArray("BmnTrigDigit");
    eventHeader = new TClonesArray("BmnEventHeader");
    runHeader = new BmnRunHeader();
    //    runHeader = new TClonesArray("BmnRunHeader");

    fDigiTree->Branch("EventHeader", &eventHeader);
    //    fDigiTree->Branch("RunHeader", &runHeader);
    fDigiTree->Branch("T0", &t0);
    fDigiTree->Branch("BC1", &bc1);
    fDigiTree->Branch("BC2", &bc2);
    fDigiTree->Branch("VETO", &veto);
    fDigiTree->Branch("FD", &fd);
    fDigiTree->Branch("BD", &bd);
    fDigiTree->Branch("DCH", &dch);
    fDigiTree->Branch("MWPC", &mwpc);
    fDigiTree->Branch("GEM", &gem);
    fDigiTree->Branch("SILICON", &silicon);
    fDigiTree->Branch("TOF400", &tof400);
    fDigiTree->Branch("TOF700", &tof700);

    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    fDchMapper = new BmnDchRaw2Digit(fPeriodId, fRunId);
    fMwpcMapper = new BmnMwpcRaw2Digit(fMwpcMapFileName);
    fTrigMapper = new BmnTrigRaw2Digit(fTrigMapFileName, fTrigINLFileName);
    if (fTof400PlaceMapFileName.Sizeof() > 1 && fTof400StripMapFileName.Sizeof() > 1) {
        fTof400Mapper = new BmnTof1Raw2Digit();
        TString dir = Form("%s%s", getenv("VMCWORKDIR"), "/input/");
        fTof400Mapper->setMapFromFile(dir + fTof400PlaceMapFileName.Data(), dir + fTof400StripMapFileName.Data());
    } else
        fTof400Mapper = new BmnTof1Raw2Digit(fPeriodId, fRunId); //Pass period and run index here or by BmnTof1Raw2Digit->setRun(...)
    fTof700Mapper = new BmnTof2Raw2DigitNew(fTof700MapFileName, fRootFileName);
    //    fTof700Mapper->readSlewingT0();
    //    fTof700Mapper->readSlewing();

    fSiliconMapper = new BmnSiliconRaw2Digit(fPeriodId, fRunId);
    fGemMapper = new BmnGemRaw2Digit(fPeriodId, fRunId);
    fPedEvCntr = 0; // counter for pedestal events between two spills
    if (fTof700Mapper) fTof700Mapper->BookSlewing();
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ClearArrays() {
    dch->Clear();
    mwpc->Clear();
    gem->Clear();
    silicon->Clear();
    tof400->Clear();
    tof700->Clear();
    t0->Clear();
    bc1->Clear();
    bc2->Clear();
    veto->Clear();
    fd->Clear();
    bd->Clear();
    eventHeader->Clear();
    //runHeader->Clear();
    if (fTimeShifts.size() > 1e5)
        fTimeShifts = map<UInt_t, Long64_t>();
    else
        fTimeShifts.clear();
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::DecodeDataToDigiIterate() {
    fCurEventType = kBMNPAYLOAD;
    fPrevEventType = fCurEventType;

    ClearArrays();
    //            Int_t iEv = fRawTree->GetEntries();
    //            fRawTree->GetEntry(iEv);

    FillTimeShiftsMap();

    //            printf("decode event #%d\n", fEventId);
    BmnEventHeader* headDAQ = (BmnEventHeader*) eventHeaderDAQ->At(0);
    fCurEventType = headDAQ->GetType();
    new((*eventHeader)[eventHeader->GetEntriesFast()]) BmnEventHeader(headDAQ->GetRunId(), headDAQ->GetEventId(), headDAQ->GetEventTime(), fCurEventType, headDAQ->GetTrig());

    fTrigMapper->FillEvent(tdc, t0, bc1, bc2, veto, fd, bd, fT0Time, &fT0Width);

    if (fCurEventType == kBMNPEDESTAL) {
        if (fPedEvCntr == N_EV_FOR_PEDESTALS - 1) return kBMNERROR; //FIX return!
        CopyDataToPedMap(adc32, fPedEvCntr);
        fPedEvCntr++;
    } else { // payload
        if (fPrevEventType == kBMNPEDESTAL) {
            if (fPedEvCntr == N_EV_FOR_PEDESTALS - 1) {
                fGemMapper->RecalculatePedestals();
                fPedEvCntr = 0;
            }
        }
        fDchMapper->FillEvent(tdc, &fTimeShifts, dch, fT0Time);
        fMwpcMapper->FillEvent(hrb, mwpc);
        fGemMapper->FillEvent(adc32, gem);
        fSiliconMapper->FillEvent(adc128, silicon);
        fTof400Mapper->FillEvent(tdc, tof400);
        fTof700Mapper->fillEvent(tdc, &fTimeShifts, fT0Time, fT0Width, tof700);

        fDigiTree->Fill();
    }
    fPrevEventType = fCurEventType;

    return kBMNSUCCESS;
}

void BmnRawDataDecoder::ResetDecoder(TString file) {
    fNevents = 0;
    syncCounter = 0;
    fRawFileName = file;
    fRawFileIn = fopen(fRawFileName, "rb");
    if (fRawFileIn == NULL) {
        printf("\n!!!!!\ncannot open file %s\nConvertRawToRoot are stopped\n!!!!!\n\n", fRawFileName.Data());
        return;
    }
    //    fRootFileOut = new TFile(fRootFileName, "recreate");
    fseeko64(fRawFileIn, 0, SEEK_END);
    fLengthRawFile = ftello64(fRawFileIn);
    rewind(fRawFileIn);
    printf("\nRawData File %s;\nLength RawData - %lld bytes (%.3f Mb)\n", fRawFileName.Data(), fLengthRawFile, fLengthRawFile / 1024. / 1024.);
    //    printf("RawRoot File %s\n\n", fRootFileName.Data());
    fDigiTree->Reset();
    fDigiTree->Branch("EventHeader", &eventHeader);
    //fDigiTree->Branch("RunHeader", &runHeader);
    fDigiTree->Branch("T0", &t0);
    fDigiTree->Branch("BC1", &bc1);
    fDigiTree->Branch("BC2", &bc2);
    fDigiTree->Branch("VETO", &veto);
    fDigiTree->Branch("FD", &fd);
    fDigiTree->Branch("BD", &bd);
    fDigiTree->Branch("DCH", &dch);
    fDigiTree->Branch("GEM", &gem);
    fDigiTree->Branch("TOF400", &tof400);
    fDigiTree->Branch("TOF700", &tof700);
    fRunId = GetRunIdFromFile(fRawFileName);
    //    fRootFileName = Form("bmn_run%04d_raw.root", fRunId);
    fDigiFileName = Form("bmn_run%04d_digi.root", fRunId);
}

BmnStatus BmnRawDataDecoder::DisposeDecoder() {
    //    fDigiTree->Write();
    //    fDigiFileOut->Close();

    delete fGemMap;
    delete fGemMapper;
    delete fSiliconMapper;
    delete fDchMapper;
    delete fMwpcMapper;
    delete fTrigMapper;
    delete fTof400Mapper;
    delete fTof700Mapper;

    delete sync;
    delete adc32;
    delete adc128;
    delete tdc;
    delete gem;
    delete dch;
    delete mwpc;
    delete t0;
    delete bc1;
    delete bc2;
    delete veto;
    delete fd;
    delete bd;
    delete tof400;
    delete tof700;
    delete eventHeader;
    delete runHeader;
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTimeShiftsMap() {
    if (fT0Map == NULL) return kBMNERROR;
    Long64_t t0time = 0;
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        if (syncDig->GetSerial() == fT0Map->serial) {
            t0time = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
            break;
        }
    }
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        Long64_t syncTime = (t0time == 0.0) ? 0 : syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
        fTimeShifts.insert(pair<UInt_t, Long64_t>(syncDig->GetSerial(), syncTime - t0time));
    }

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::CopyDataToPedMap(TClonesArray* adc, UInt_t ev) {
    if (!fGemMapper) return kBMNERROR;
    UInt_t**** pedData = fGemMapper->GetPedData();
    for (UInt_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);

        Int_t iSer = -1;
        for (iSer = 0; iSer < fNGemSerials; ++iSer)
            if (adcDig->GetSerial() == fGemSerials[iSer]) break;
        if (iSer == -1) return kBMNERROR;

        for (UInt_t iSmpl = 0; iSmpl < ADC32_N_SAMPLES; ++iSmpl)
            pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (adcDig->GetValue())[iSmpl] / 16;
    }

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTimeShiftsMapNoDB(UInt_t t0serial) {

    Long64_t t0time = -1;
    //    printf(" sync size %d, t0serial 0x%0x\n", sync->GetEntriesFast(), t0serial);
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        //	printf(" have 0x%0x requred 0x%0x\n", syncDig->GetSerial(), t0serial);
        if (syncDig->GetSerial() == t0serial) {
            t0time = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
            break;
        }
    }

    if (t0time == -1) {
        //        cerr << "No T0 digit found in tree" << endl;
        return kBMNERROR;
    }

    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        Long64_t syncTime = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
        fTimeShifts.insert(pair<UInt_t, Long64_t>(syncDig->GetSerial(), syncTime - t0time));
    }

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::SlewingTOF700Init() {

    fRootFileIn = new TFile(fRootFileName, "READ");
    if (fRootFileIn->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s \nSlewingTOF700 are stopped\n!!!!\n", fRootFileName.Data());
        return kBMNERROR;
    } else {
        printf("\nInput root file: %s;\n", fRootFileName.Data());
    }
    fRawTree = (TTree *) fRootFileIn->Get("BMN_RAW");
    tdc = new TClonesArray("BmnTDCDigit");
    sync = new TClonesArray("BmnSyncDigit");
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("SYNC", &sync);

    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    fTrigMapper = new BmnTrigRaw2Digit(fTrigMapFileName, fTrigINLFileName);
    fTof700Mapper = new BmnTof2Raw2DigitNew(fTof700MapFileName, fRootFileName);
    fTof700Mapper->print();

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::SlewingTOF700() {

    fTof700Mapper->BookSlewing();

    for (Int_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0) cout << "Slewing T0 event #" << iEv << endl;
        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
            //                cout << "No TimeShiftMap created" << endl;
            continue;
        }

        fTrigMapper->FillEvent(tdc, NULL, NULL, NULL, NULL, NULL, NULL, fT0Time, &fT0Width);

        fTof700Mapper->fillSlewingT0(tdc, &fTimeShifts, fT0Time, fT0Width);
    }
    cout << "Slewing T0 event #" << fNevents << endl;

    fTof700Mapper->SlewingT0();

    fTof700Mapper->readSlewingT0();


    for (Int_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0) cout << "Slewing RPC event #" << iEv << endl;
        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
            //                cout << "No TimeShiftMap created" << endl;
            continue;
        }

        fTrigMapper->FillEvent(tdc, NULL, NULL, NULL, NULL, NULL, NULL, fT0Time, &fT0Width);

        fTof700Mapper->fillSlewing(tdc, &fTimeShifts, fT0Time, fT0Width);
    }
    cout << "Slewing RPC event #" << fNevents << endl;

    fTof700Mapper->Slewing();

    //    fRootFileIn->Close();

    //    delete trigMapper;
    //    delete tof700Mapper;

    return kBMNSUCCESS;
}

Int_t BmnRawDataDecoder::GetRunIdFromFile(TString name) {
    Int_t runId = -1;
    FILE * file = fopen(name.Data(), "rb");
    if (file == NULL) {
        printf("File %s is not open!!!\n", name.Data());
        return -1;
    }
    UInt_t word;
    while (fread(&word, kWORDSIZE, 1, file)) {
        if (word == kRUNNUMBERSYNC) {
            fread(&word, kWORDSIZE, 1, file); //skip word
            fread(&runId, kWORDSIZE, 1, file);
            return runId;
        }
    }
    fclose(fRawFileIn);
}