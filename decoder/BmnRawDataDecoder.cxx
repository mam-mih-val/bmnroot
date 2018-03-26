
#include "BmnRawDataDecoder.h"

//#include <pthread.h>
#include <sys/stat.h>
#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */

using namespace std;

class UniDbRun;

BmnRawDataDecoder::BmnRawDataDecoder() {
    fRunId = 0;
    fPeriodId = 0;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = 0;
    fLengthRawFile = 0;
    fCurentPositionRawFile = 0;
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
    tqdc_tdc = NULL;
    tqdc_adc = NULL;
    hrb = NULL;
    adc32 = NULL;
    adc128 = NULL;
    adc = NULL;
    tacquila = NULL;
    msc = NULL;
    dch = NULL;
    mwpc = NULL;
    tof400 = NULL;
    tof700 = NULL;
    zdc = NULL;
    ecal = NULL;
    gem = NULL;
    silicon = NULL;
    land = NULL;
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
    fTof700GeomFileName = "";
    fZDCCalibrationFileName = "";
    fZDCMapFileName = "";
    fECALCalibrationFileName = "";
    fECALMapFileName = "";
    fLANDMapFileName = "";
    fLANDClockFileName = "";
    fLANDTCalFileName = "";
    fLANDDiffSyncFileName = "";
    fLANDVScintFileName = "";
    fDat = 0;
    fGemMapper = NULL;
    fDchMapper = NULL;
    fTrigMapper = NULL;
    fTof400Mapper = NULL;
    fTof700Mapper = NULL;
    fZDCMapper = NULL;
    fECALMapper = NULL;
    fLANDMapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fPedoCounter = 0;
    fGemMap = NULL;
    fEvForPedestals = N_EV_FOR_PEDESTALS;
    fBmnSetup = kBMNSETUP;
    fT0Map = NULL;
}

BmnRawDataDecoder::BmnRawDataDecoder(TString file, ULong_t nEvents, ULong_t period) {

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
    hrb = NULL;
    tdc = NULL;
    tqdc_tdc = NULL;
    tqdc_adc = NULL;
    adc32 = NULL;
    adc128 = NULL;
    adc = NULL;
    tacquila = NULL;
    msc = NULL;
    dch = NULL;
    tof400 = NULL;
    tof700 = NULL;
    zdc = NULL;
    ecal = NULL;
    gem = NULL;
    silicon = NULL;
    land = NULL;
    fRawFileName = file;
    fTOF700ReferenceRun = 0;
    fTOF700ReferenceChamber = 0;
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
    fTof700GeomFileName = "";
    fZDCCalibrationFileName = "";
    fZDCMapFileName = "";
    fECALCalibrationFileName = "";
    fECALMapFileName = "";
    fLANDMapFileName = "";
    fLANDClockFileName = "";
    fLANDTCalFileName = "";
    fLANDDiffSyncFileName = "";
    fLANDVScintFileName = "";
    fDat = 0;
    fGemMapper = NULL;
    fDchMapper = NULL;
    fTrigMapper = NULL;
    fTof400Mapper = NULL;
    fTof700Mapper = NULL;
    fZDCMapper = NULL;
    fECALMapper = NULL;
    fLANDMapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fPedoCounter = 0;
    fGemMap = NULL;
    fEvForPedestals = N_EV_FOR_PEDESTALS;
    fBmnSetup = kBMNSETUP;
    fT0Map = NULL;
    //    InitMaps();
}

BmnRawDataDecoder::~BmnRawDataDecoder() {
}

BmnStatus BmnRawDataDecoder::ConvertRawToRoot() {
    if (InitConverter(fRawFileName) == kBMNERROR)
        return kBMNERROR;
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

    fRawTree->Branch("RunHeader", &runHeaderDAQ);
    runHeaderDAQ->SetRunId(fRunId);
    TTimeStamp startT = TTimeStamp(time_t(fTimeStart_s), fTimeStart_ns);
    TTimeStamp finishT = TTimeStamp(time_t(fTime_s), fTime_ns);
    fRunStartTime = TDatime(Int_t(startT.GetDate(kFALSE)), Int_t(startT.GetTime(kFALSE)));
    fRunEndTime = TDatime(Int_t(finishT.GetDate(kFALSE)), Int_t(finishT.GetTime(kFALSE)));
    runHeaderDAQ->SetStartTime(fRunStartTime);
    runHeaderDAQ->SetFinishTime(fRunEndTime);
    runHeaderDAQ->SetNEvents(fNevents);
    fRawTree->Fill();

    fCurentPositionRawFile = ftello64(fRawFileIn);
    printf("Read %d events; %lld bytes (%.3f Mb)\n\n", fNevents, fCurentPositionRawFile, fCurentPositionRawFile / 1024. / 1024.);

    fRawTree->Write();
    fRootFileOut->Close();
    fclose(fRawFileIn);

    delete sync;
    delete adc32;
    delete adc128;
    delete adc;
    delete tacquila;
    delete hrb;
    delete tdc;
    delete tqdc_tdc;
    delete tqdc_adc;
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
    //    fDigiFileName = Form("bmn_run%04d_digi.root", fRunId);

    InitConverter(fRawFileName);
    return kBMNSUCCESS;

}

BmnStatus BmnRawDataDecoder::InitConverter(TString FileName) {
    printf(ANSI_COLOR_RED "\n================ CONVERTING ================\n" ANSI_COLOR_RESET);
    fRawFileName = FileName;
    fRawFileIn = fopen(fRawFileName, "rb");
    if (fRawFileIn == NULL) {
        printf("\n!!!!!\ncannot open file %s\nConvertRawToRoot are stopped\n!!!!!\n\n", fRawFileName.Data());
        return kBMNERROR;
    }
    return InitConverter();
}

BmnStatus BmnRawDataDecoder::InitConverter() {
    fRawTree = new TTree("BMN_RAW", "BMN_RAW");
    sync = new TClonesArray("BmnSyncDigit");
    adc32 = new TClonesArray("BmnADCDigit");
    adc128 = new TClonesArray("BmnADCDigit");
    adc = new TClonesArray("BmnADCDigit");
    tacquila = new TClonesArray("BmnTacquilaDigit");
    tdc = new TClonesArray("BmnTDCDigit");
    tqdc_adc = new TClonesArray("BmnTQDCADCDigit");
    tqdc_tdc = new TClonesArray("BmnTDCDigit");
    hrb = new TClonesArray("BmnHRBDigit");
    msc = new TClonesArray("BmnMSCDigit");
    eventHeaderDAQ = new TClonesArray("BmnEventHeader");
    runHeaderDAQ = new BmnRunHeader();

    fRawTree->Branch("SYNC", &sync);
    fRawTree->Branch("ADC32", &adc32);
    fRawTree->Branch("ADC128", &adc128);
    fRawTree->Branch("ADC", &adc);
    fRawTree->Branch("Tacquila", &tacquila);
    fRawTree->Branch("TDC", &tdc);
    fRawTree->Branch("TQDC_ADC", &tqdc_adc);
    fRawTree->Branch("TQDC_TDC", &tqdc_tdc);
    fRawTree->Branch("HRB", &hrb);
    fRawTree->Branch("MSC", &msc);
    fRawTree->Branch("EventHeader", &eventHeaderDAQ);
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::wait_stream(deque<UInt_t> *que, Int_t len, UInt_t limit) {
    Int_t t;
    Int_t dt = 10000;
    while (que->size() < len) {
        if (t > limit)
            return kBMNERROR;
        usleep(dt);
        t += dt;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::wait_file(Int_t len, UInt_t limit) {
    Long_t pos = ftello64(fRawFileIn);
    Int_t t = 0;
    Int_t dt = 1000000;
    while (fLengthRawFile < pos + len) {
        //        gSystem->ProcessEvents();
        if (t > limit)
            return kBMNERROR;
        usleep(dt);
        fseeko64(fRawFileIn, 0, SEEK_END);
        fLengthRawFile = ftello64(fRawFileIn);
        fseeko64(fRawFileIn, pos - fLengthRawFile, SEEK_CUR);
        t += dt;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ConvertRawToRootIterate(UInt_t *buf, UInt_t len) {
    //        fRawTree->Clear();
    //    if (wait_stream(fDataQueue, 2) == kBMNERROR)
    //        return kBMNTIMEOUT;
    //    fDat = fDataQueue->front();
    //    fDataQueue->pop_front();
    //    if (fDat == kSYNC1) { //search for start of event
    //        // read number of bytes in event
    //        fDat = fDataQueue->front();
    //        fDataQueue->pop_front();
    //        if (wait_stream(fDataQueue, fDat) == kBMNERROR)
    //            return kBMNTIMEOUT;
    //        fDat = fDat / kNBYTESINWORD + 1; // bytes --> words
    //        if (fDat * kNBYTESINWORD >= 100000) { // what the constant?
    //            printf("Wrong data size: %d:  skip this event\n", fDat);
    //            fDataQueue->erase(fDataQueue->begin(), fDataQueue->begin() + fDat * kNBYTESINWORD);
    //            return kBMNERROR;
    //        } else {
    //            //read array of current event data and process them
    //            if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) return kBMNERROR;
    //            for (Int_t iByte = 0; iByte < fDat * kNBYTESINWORD; iByte++) {
    //                data[iByte] = fDataQueue->front();
    //                fDataQueue->pop_front();
    //            }
    fEventId = buf[0];
    //            printf("EventID = %d\n", fEventId);
    if (fEventId <= 0) return kBMNERROR; // continue; // skip bad events (it is possible, but what about 0?) 
    ProcessEvent(buf, len);
    fNevents++;
    //                fRawTree->Fill();
    //        }
    //    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ConvertRawToRootIterateFile(UInt_t limit) {
    //        if (fMaxEvent > 0 && fNevents == fMaxEvent) break;
    while (kTRUE) {
        if (wait_file(4 * kWORDSIZE, limit) == kBMNERROR) {
            return kBMNTIMEOUT;
            printf("file timeout\n");
        }
        fCurentPositionRawFile = ftello64(fRawFileIn);
        fread(&fDat, kWORDSIZE, 1, fRawFileIn);
        if (fDat == kRUNNUMBERSYNC) {
            printf("RunNumberSync\n");
            syncCounter++;
            if (syncCounter > 1) {
                cout << "Finish by SYNC" << endl;
                return kBMNFINISH;
            }
            fread(&fDat, kWORDSIZE, 1, fRawFileIn); //skip word
        }
        if (fDat == kSYNC1) { //search for start of event
            // read number of bytes in event
            //printf("kSYNC1\n");
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
            fDat = fDat / kNBYTESINWORD + 1; // bytes --> words
            if (fDat * kNBYTESINWORD >= 1000000) { // what the constant?
                printf("Wrong data size: %d:  skip this event\n", fDat);
                return kBMNFINISH;
            }
            //read array of current event data and process them
            if (wait_file(fDat * kNBYTESINWORD * kWORDSIZE, limit) == kBMNERROR) {
                return kBMNTIMEOUT;
                printf("file timeout\n");
            }
            if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) {
                printf("finish by length\n");
                return kBMNFINISH;
            }
            fEventId = data[0];
            if (fEventId <= 0) {
                printf("bad event #%d\n", fEventId);
                return kBMNERROR; // continue; // skip bad events (it is possible, but what about 0?) 
            }
            ProcessEvent(data, fDat);
            fNevents++;
            break;
            //        fRawTree->Fill();
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ProcessEvent(UInt_t *d, UInt_t len) {

    sync->Delete();
    tdc->Delete();
    tqdc_adc->Delete();
    tqdc_tdc->Delete();
    hrb->Delete();
    adc32->Delete();
    adc128->Delete();
    adc->Delete();
    tacquila->Delete();
    msc->Delete();
    eventHeaderDAQ->Delete();

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
        //        printf("id %x\n", id);
        UInt_t payload = (d[idx++] & 0xFFFFFF) / kNBYTESINWORD;
        if (payload > 2000000) {
            printf("[WARNING] Event %d:\n serial = 0x%06X\n id = Ox%02X\n payload = %d\n", fEventId, serial, id, payload);
            break;
        }
        switch (id) {
            case kADC64VE_XGE:
            case kADC64VE:
            {
                Bool_t isFound = kFALSE;
                for (Int_t iSer = 0; iSer < fNSiliconSerials; ++iSer)
                    if (serial == fSiliconSerials[iSer]) {
                        Process_ADC64VE(&d[idx], payload, serial, 128, adc128);
                        isFound = kTRUE;
                        break;
                    }
                if (isFound) break;
                for (Int_t iSer = 0; iSer < fNGemSerials; ++iSer)
                    if (serial == fGemSerials[iSer]) {
                        Process_ADC64VE(&d[idx], payload, serial, 32, adc32);
                        isFound = kTRUE;
                        break;
                    }
                if (isFound) break;
                for (Int_t iSer = 0; iSer < fNCSCSerials; ++iSer)
                    if (serial == fCSCSerials[iSer]) {
                        Process_ADC64VE(&d[idx], payload, serial, 32, adc32);
                        isFound = kTRUE;
                        break;
                    }
                break;
            }
            case kADC64WR:
            {
                Bool_t isZDC = kFALSE;
                for (Int_t iSer = 0; (iSer < fNZDCSerials); ++iSer) {
                    if (serial == fZDCSerials[iSer]) {
                        isZDC = kTRUE;
                        break;
                    }
                };
                if (isZDC)
                    Process_ADC64WR(&d[idx], payload, serial, adc);
                else {
                    Bool_t isECAL = kFALSE;
                    for (Int_t iSer = 0; (iSer < fNECALSerials); ++iSer) {
                        if (serial == fECALSerials[iSer]) {
                            isECAL = kTRUE;
                            break;
                        }
                    };
                    if (isECAL)
                        Process_ADC64WR(&d[idx], payload, serial, adc);
                }
                break;
            }
            case kFVME:
                Process_FVME(&d[idx], payload, serial, evType, trigType);
                break;
            case kHRB:
                Process_HRB(&d[idx], payload, serial);
                break;
            case kLAND:
                Process_Tacquila(&d[idx], payload);
                break;
        }
        idx += payload;
    }
    //printf("eventHeaderDAQ->GetEntriesFast() %d  eventID %d\n", eventHeaderDAQ->GetEntriesFast(), fEventId);
    new((*eventHeaderDAQ)[eventHeaderDAQ->GetEntriesFast()]) BmnEventHeader(fRunId, fEventId, TDatime(Int_t(TTimeStamp(time_t(fTime_s), fTime_ns).GetDate(kFALSE)), Int_t(TTimeStamp(time_t(fTime_s), fTime_ns).GetTime(kFALSE))), evType, trigType, kFALSE);
}

BmnStatus BmnRawDataDecoder::Process_ADC64VE(UInt_t *d, UInt_t len, UInt_t serial, UInt_t nSmpl, TClonesArray *arr) {
    const UChar_t kNCH = 64;
    const UChar_t kNSTAMPS = nSmpl;

    UShort_t valU[kNSTAMPS];
    Short_t valI[kNSTAMPS];
    for (Int_t i = 0; i < kNSTAMPS; ++i) {
        valU[i] = 0;
        valI[i] = 0;
    }

    UInt_t i = 0;
    while (i < len) {
        UInt_t subType = d[i] & 0x3;
        if (subType == 0) {
            i += 5; //skip unused words
            UInt_t iCh = 0;
            while (iCh < kNCH - 1 && i < len) {
                iCh = d[i] >> 24;
                if (iCh > 64) printf("serial = 0x%X     iCh = %d  nSmpl = %d\n", serial, iCh, nSmpl);
                i += 3; // skip two timestamp words (they are empty)
                TClonesArray& ar_adc = *arr;

                if (fRunId > GetBoundaryRun(kNSTAMPS)) {
                    TakeDataWordShort(kNSTAMPS, d, i, valI);
                    new(ar_adc[arr->GetEntriesFast()]) BmnADCDigit(serial, iCh, kNSTAMPS, valI);
                } else {
                    TakeDataWordUShort(kNSTAMPS, d, i, valU);
                    new(ar_adc[arr->GetEntriesFast()]) BmnADCDigit(serial, iCh, kNSTAMPS, valU);
                }
                i += (kNSTAMPS / 2); //skip words (we've processed them above)
            }
        } else break;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::TakeDataWordShort(UChar_t n, UInt_t *d, UInt_t i, Short_t* valI) {
    for (Int_t iWord = 0; iWord < n / 2; ++iWord) {
        valI[2 * iWord + 1] = d[i + iWord] & 0xFFFF; //take 16 lower bits and put them into corresponded cell of data-array
        valI[2 * iWord] = (d[i + iWord] >> 16) & 0xFFFF; //take 16 higher bits and put them into corresponded cell of data-array
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::TakeDataWordUShort(UChar_t n, UInt_t *d, UInt_t i, UShort_t* valU) {
    for (Int_t iWord = 0; iWord < n / 2; ++iWord) {
        valU[2 * iWord + 1] = d[i + iWord] & 0xFFFF; //take 16 lower bits and put them into corresponded cell of data-array
        valU[2 * iWord] = (d[i + iWord] >> 16) & 0xFFFF; //take 16 higher bits and put them into corresponded cell of data-array
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::Process_ADC64WR(UInt_t *d, UInt_t len, UInt_t serial, TClonesArray *arr) {
    const UChar_t kNCH = 64;
    const UChar_t kNSTAMPS = 128;

    UShort_t val[kNSTAMPS];
    for (Int_t i = 0; i < kNSTAMPS; ++i) val[i] = 0;

    UInt_t i = 0;
    UInt_t ns = 0;
    while (i < len) {
        UInt_t subType = d[i] & 0x3;
        if (subType == 0) {
            i += 5; //skip unused words
            UInt_t iCh = 0;
            while (iCh < kNCH - 1 && i < len) {
                iCh = d[i] >> 24;
                ns = (d[i] & 0xFFF) / 2 - 4;
                i += 3; // skip two timestamp words (they are empty)
                for (Int_t iWord = 0; iWord < ns / 2; ++iWord) {
                    val[2 * iWord + 1] = d[i + iWord] & 0xFFFF; //take 16 lower bits and put them into corresponded cell of data-array
                    val[2 * iWord] = (d[i + iWord] >> 16) & 0xFFFF; //take 16 higher bits and put them into corresponded cell of data-array
                }

                TClonesArray& ar_adc = *arr;
                if (iCh >= 0 && iCh < kNCH) {
                    //			printf("ns == %d, serial == 0x%0x, chan == %d\n", ns, serial, iCh);
                    new(ar_adc[arr->GetEntriesFast()]) BmnADCDigit(serial, iCh, ns, val);
                }
                i += (ns / 2); //skip words (we've processed them)
            }
        } else break;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::Process_FVME(UInt_t *d, UInt_t len, UInt_t serial, BmnEventType &evType, BmnTriggerType &trType) {
    UInt_t modId = 0;
    UInt_t slot = 0;
    UInt_t type = 0;
    for (UInt_t i = 0; i < len; i++) {
        type = d[i] >> 28;
        //        printf("type %x\n", type);
        //printf("modid 0x%X  serial 0x%X\n", modId, serial);
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
                    case kTQDC16VS:
                        FillTQDC(d, serial, slot, modId, i);
                        break;
                    case kMSC:
                        FillMSC(d, serial, i); //empty now
                        break;
                    case kTRIG:
                        FillSYNC(d, serial, i);
                        break;
                    case kU40VE_RC:
                            //printf("d[i] %x\n", d[i]);
                        if (fPeriodId > 4 && type == kGEMTRIGTYPE && slot == kEVENTTYPESLOT) {
                            trType = ((d[i] & 0x7) == kTRIGMINBIAS) ? kBMNMINBIAS : kBMNBEAM;
                            evType = ((d[i] & 0x8) >> 3) ? kBMNPEDESTAL : kBMNPAYLOAD;
                            if (evType == kBMNPEDESTAL)
                                fPedoCounter++;
                        }
                        FillU40VE(d, serial, slot, modId, i);
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
                if (word32 & BIT(iCh)) {
                    TClonesArray &ar_hrb = *hrb;
                    new(ar_hrb[hrb->GetEntriesFast()]) BmnHRBDigit(serial, iCh + 32 * iWord, iSmpl, tH, tL);
                }
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::Process_Tacquila(UInt_t *d, UInt_t len) {
    /* LAND Tacquila data in big endian. */
    uint32_t *p32 = d;

    /* 64-bit TRLO II timestamp. */
    p32 += 2;

    /*
     * 6 scalers:
     * 0: laniic3 m=1.
     * 1: laniic3 m=2.
     * 2: laniic6 m=1.
     * 3: laniic6 m=2.
     * 4: JINR DAQ trigger.
     * 5: ---
     */
    p32 += 6;

    /*
     * Tacquila data!
     * We have 2 chains of 10 Tacquila cards each.
     */
    for (unsigned chain = 0; chain < 2; ++chain) {
        uint32_t header = ntohl(*p32++);
#define TACQUILA_PRINT_HEADER << "(header=" << header << ")" <<
        unsigned count = header & 0x1ff;
        if (count & 1) {
            cerr << __FILE__ << ':' << __LINE__ << ": Odd data count forbidden "
                    TACQUILA_PRINT_HEADER ".\n";
            return kBMNFINISH;
        }
        unsigned gtb = (header >> 24) & 0xf;
        if (chain != gtb) {
            cerr << __FILE__ << ':' << __LINE__ << ": GTB=" << gtb << "!=" << chain
                    << " forbidden " TACQUILA_PRINT_HEADER ".\n";
            return kBMNFINISH;
        }
        unsigned sam = header >> 28;
        if (5 != sam) {
            cerr << __FILE__ << ':' << __LINE__ << ": SAM=" << sam << "!=5 "
                    "forbidden " TACQUILA_PRINT_HEADER ".\n";
            return kBMNFINISH;
        }
        unsigned tac, clock;
        for (unsigned i = 0; i < count; ++i) {
            uint32_t u32 = ntohl(*p32++);
#define TACQUILA_PRINT_DATA << "(data=" << std::hex << u32 << std::dec << ")" <<
            /*
             * Channels 0..15 are normal, 16 = common stop,
             * anything else is bogus.
             */
            unsigned channel = (u32 >> 22) & 0x1f;
            if (channel > 16) {
                cerr << __FILE__ << ':' << __LINE__ << ": Channel=" << channel <<
                        ">16 forbidden " TACQUILA_PRINT_DATA ".\n";
                return kBMNFINISH;
            }
            /* 10 + 10 Tacquila cards used for LAND. */
            unsigned module = u32 >> 27;
            if (module < 1 || module > 10) {
                cerr << __FILE__ << ':' << __LINE__ << ": Module=" << module <<
                        " forbidden " TACQUILA_PRINT_DATA ".\n";
                return kBMNFINISH;
            }
            unsigned be_qdc = 1 & i;
            unsigned is_qdc = 1 & (u32 >> 21);
            if (be_qdc != is_qdc) {
                cerr << __FILE__ << ':' << __LINE__ << ": TDC/QDC word mismatch "
                        TACQUILA_PRINT_DATA ".\n";
                return kBMNFINISH;
            }
            if (0 == (1 & i)) {
                /* Tacqcuila measures reverse time. */
                tac = 0xfff - (u32 & 0xfff);
                clock = 0x3f - ((u32 >> 12) & 0x3f);
            } else {
                /* QDC:s are not reversed :) */
                unsigned qdc = u32 & 0xfff;
                TClonesArray &ar_tacquila = *tacquila;
                new(ar_tacquila[tacquila->GetEntriesFast()])
                        BmnTacquilaDigit(sam, gtb, module - 1, channel, tac, clock, qdc);
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillU40VE(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t & idx) {
    UInt_t type = d[idx] >> 28;
    while (type == 2 || type == 3 || type == 4) {
        if (type == 4) {
            UInt_t trigCand = d[idx + 0] & 0x1FFFFFFF;
            UInt_t trigAcce = d[idx + 1] & 0x1FFFFFFF;
            UInt_t trigBefo = d[idx + 2] & 0x1FFFFFFF;
            UInt_t trigAfte = d[idx + 3] & 0x1FFFFFFF;
            UInt_t trigRjCt = d[idx + 4] & 0x1FFFFFFF;
            idx += 5;
            //            printf("cand %04d, acc %04d, bef %04d, after %04d, rjct %04d\n",
            //                    trigCand, trigAcce, trigBefo, trigAfte, trigRjCt);
            break;
        }
        idx++; //go to the next DATA-word
        type = d[idx] >> 28;
    }
    idx--;
}

BmnStatus BmnRawDataDecoder::FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t & idx) {
    UInt_t type = d[idx] >> 28;
    //    printf("fiiltdc\n");
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

BmnStatus BmnRawDataDecoder::FillTQDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t & idx) {
    //        printf("serial 0x%08X slot %d  modid 0x%X\n", serial, slot, modId);
    UInt_t type = d[idx] >> 28; // good
    UShort_t trigTimestamp = 0;
    UShort_t adcTimestamp = 0;
    UShort_t tdcTimestamp = 0;
    UInt_t iSampl = 0;
    UInt_t channel = 0;
    Short_t valI[ADC_SAMPLING_LIMIT];
    Bool_t inADC = kFALSE;
    if (type == 6) {
        fprintf(stderr, "TQDC Error: %d\n", d[idx++] & 0xF); // @TODO logging
        return kBMNSUCCESS;
    }
    while (type != kMODTRAILER) {
        UInt_t mode = (d[idx] >> 26) & 0x3; // good
        if (!inADC) {
            //            printf("type %d mode %d\n", type, mode);
            if ((mode == 0) && (type == 4 || type == 5)) { // good
                UInt_t rcdata = ((d[idx] >> 24) & 0x3) << 19; // fixed					
                channel = (d[idx] >> 19) & 0x1F; // i think ok...
                UInt_t time = 4 * (d[idx] & 0x7FFFF) + rcdata; // in 25 ps
                new((*tqdc_tdc)[tqdc_tdc->GetEntriesFast()]) BmnTDCDigit(serial, modId, slot, (type == 4), channel, 0, time, tdcTimestamp);
                //                printf("TDC: type %d channel %d time %d \n", type, channel, time);
            } else if ((type == 4) && (mode == 2)) {
                channel = (d[idx] >> 19) & 0x1F;
                trigTimestamp = d[idx++] & 0xFF;
                adcTimestamp = d[idx] & 0xFF;
                inADC = kTRUE;
                //                printf("ADC: channel %d trigTimestamp %d  adcTimestamp %d\n", channel, trigTimestamp, adcTimestamp);
            } else if ((type == 2) && (mode == 0)) {
                UInt_t iEv = (d[idx] >> 12) & 0x1FFF;
                tdcTimestamp = d[idx] & 0xFFF;
                //                printf("TDC ev header: %d\n", iEv);
            } else if ((type == 3) && (mode == 0)) {
                UInt_t iEv = (d[idx] >> 12) & 0x1FFF;
                //                printf("TDC ev trailer: %d\n", iEv);
            }
        } else {
            if ((type == 5) && ((mode == 2) || (mode == 1)) && (iSampl < ADC_SAMPLING_LIMIT)) {
                Short_t val = (d[idx] & ((1 << 14) - 1)) - (1 << (14 - 1));
                valI[iSampl++] = val;
            } else {
                new((*tqdc_adc)[tqdc_adc->GetEntriesFast()]) BmnTQDCADCDigit(serial, channel, slot, iSampl, valI, trigTimestamp, adcTimestamp);
                inADC = kFALSE;
                iSampl = 0;
            }
        }
        type = d[++idx] >> 28;
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
    tqdc_adc = new TClonesArray("BmnTQDCADCDigit");
    tqdc_tdc = new TClonesArray("BmnTDCDigit");
    hrb = new TClonesArray("BmnHRBDigit");
    sync = new TClonesArray("BmnSyncDigit");
    adc32 = new TClonesArray("BmnADCDigit");
    adc128 = new TClonesArray("BmnADCDigit");
    adc = new TClonesArray("BmnADCDigit");
    tacquila = new TClonesArray("BmnTacquilaDigit");
    eventHeaderDAQ = new TClonesArray("BmnEventHeader");
    runHeaderDAQ = new BmnRunHeader();
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("TQDC_ADC", &tqdc_adc);
    fRawTree->SetBranchAddress("TQDC_TDC", &tqdc_tdc);
    fRawTree->SetBranchAddress("HRB", &hrb);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("ADC32", &adc32);
    fRawTree->SetBranchAddress("ADC128", &adc128);
    fRawTree->SetBranchAddress("ADC", &adc);
    fRawTree->SetBranchAddress("Tacquila", &tacquila);
    fRawTree->SetBranchAddress("EventHeader", &eventHeaderDAQ);
    fRawTree->SetBranchAddress("RunHeader", &runHeaderDAQ);

    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    InitDecoder();
    BmnEventType curEventType = kBMNPAYLOAD;
    BmnEventType prevEventType = curEventType;

    if (fGemMapper || fSiliconMapper) {
        for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
            if (iEv % 100 == 0) {
                printf(ANSI_COLOR_BLUE "[%.2f%%]   " ANSI_COLOR_RESET, iEv * 100.0 / fNevents);
                printf("EVENT:%d   RUN:%d\n", iEv, fRunId);
            }

            fRawTree->GetEntry(iEv);

            BmnEventHeader* headDAQ = (BmnEventHeader*) eventHeaderDAQ->At(0);
            if (!headDAQ) continue;
            curEventType = headDAQ->GetType();

            if (curEventType != kBMNPEDESTAL) continue;
            if (fPedEvCntr != fEvForPedestals - 1) {
                printf("fPedEvCntr %d\n", fPedEvCntr);
                CopyDataToPedMap(adc32, adc128, fPedEvCntr);
                fPedEvCntr++;
            } else {
                if (fGemMapper) fGemMapper->RecalculatePedestals();
                if (fSiliconMapper) fSiliconMapper->RecalculatePedestals();
                fPedEvCntr = 0;
                break;
            }
        }
    }

    Int_t nEv = -1;
    Double_t fSize = 0.0;
    UInt_t runId = 0;
    vector<UInt_t> startTripEvent;
    vector<UInt_t> endTripEvent;

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

        if (iEv == 0) {

            nEv = (Int_t) runHeaderDAQ->GetNEvents();
            fSize = Double_t(fLengthRawFile / 1024. / 1024.);
            runId = runHeaderDAQ->GetRunId();
            fRunStartTime = runHeaderDAQ->GetStartTime();
            fRunEndTime = runHeaderDAQ->GetFinishTime();

            if (!UniDbRun::GetRun(fPeriodId, runId))
                UniDbRun::CreateRun(fPeriodId, runId, TString::Format("/nica/data4mpd1/dataBMN/bmndata2/run6/raw/mpd_run_Glob_%d.data", runId), "", NULL, NULL, fRunStartTime, &fRunEndTime, &nEv, NULL, &fSize, NULL);

            //check for trip information
            UniDbTangoData db_tango;
            enumConditions condition = conditionEqual;
            bool condition_value = 1;
            int map_channel[] = {1, 3, 0, 5, 2, 6, 4};
            TString date_start = fRunStartTime.AsSQLString(); // 1252 run
            TString date_end = fRunEndTime.AsSQLString();

            UInt_t runLength = fRunEndTime.Convert() - fRunStartTime.Convert(); //in seconds
            Double_t timeStep = runLength * 1.0 / fNevents; //time for one event
            printf("Run duration = %d sec.\t TimeStep = %f sec./event\n", runLength, timeStep);

            TObjArray* tango_data_gem = db_tango.SearchTangoIntervals((char*) "gem", (char*) "trip", (char*) date_start.Data(), (char*) date_end.Data(), condition, condition_value, map_channel);
            if (tango_data_gem) {
                for (Int_t i = 0; i < tango_data_gem->GetEntriesFast(); ++i) {
                    TObjArray* currGemTripInfo = (TObjArray*) tango_data_gem->At(i);
                    if (currGemTripInfo->GetEntriesFast() != 0)
                        for (Int_t j = 0; j < currGemTripInfo->GetEntriesFast(); ++j) {
                            TangoTimeInterval* ti = (TangoTimeInterval*) currGemTripInfo->At(j);
                            startTripEvent.push_back(UInt_t((ti->start_time.Convert() - fRunStartTime.Convert()) / timeStep));
                            endTripEvent.push_back(UInt_t((ti->end_time.Convert() - fRunStartTime.Convert()) / timeStep));
                        }
                }
            }
        }

        curEventType = headDAQ->GetType();

        Bool_t isTripEvent = kFALSE;
        for (Int_t iTrip = 0; iTrip < startTripEvent.size(); ++iTrip) {
            if (headDAQ->GetEventId() > startTripEvent[iTrip] && headDAQ->GetEventId() < endTripEvent[iTrip]) {
                isTripEvent = kTRUE;
                break;
            }
        }

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
            fTrigMapper->FillEvent(tdc);
        }
        GetT0Info(fT0Time, fT0Width);
        new((*eventHeader)[eventHeader->GetEntriesFast()]) BmnEventHeader(headDAQ->GetRunId(), headDAQ->GetEventId(), headDAQ->GetEventTime(), curEventType, headDAQ->GetTrig(), isTripEvent, fTimeShifts);
        BmnEventHeader* evHdr = (BmnEventHeader*) eventHeader->At(eventHeader->GetEntriesFast() - 1);
        evHdr->SetStartSignalInfo(fT0Time, fT0Width);

        //if (t0->GetEntriesFast() != 1 || bc2->GetEntriesFast() != 1) continue;
        if (curEventType == kBMNPEDESTAL) {
            if (fPedEvCntr == fEvForPedestals - 1) continue;
            CopyDataToPedMap(adc32, adc128, fPedEvCntr);
            fPedEvCntr++;
        } else { // payload
            if (prevEventType == kBMNPEDESTAL && fPedEvCntr == fEvForPedestals - 1) {
                if (fGemMapper) fGemMapper->RecalculatePedestals();
                if (fSiliconMapper) fSiliconMapper->RecalculatePedestals();
                fPedEvCntr = 0;
            }
            if (fGemMapper) fGemMapper->FillEvent(adc32, gem);
            if (fSiliconMapper) fSiliconMapper->FillEvent(adc128, silicon);
            if (fDchMapper) fDchMapper->FillEvent(tdc, &fTimeShifts, dch, fT0Time);
            if (fMwpcMapper) fMwpcMapper->FillEvent(hrb, mwpc);
            if (fTof400Mapper) fTof400Mapper->FillEvent(tdc, &fTimeShifts, tof400);
            if (fTof700Mapper) fTof700Mapper->fillEvent(tdc, &fTimeShifts, fT0Time, fT0Width, tof700);
            if (fZDCMapper) fZDCMapper->fillEvent(adc, zdc);
            if (fECALMapper) fECALMapper->fillEvent(adc, ecal);
            if (fLANDMapper) fLANDMapper->fillEvent(tacquila, land);
        }

        fDigiTree->Fill();
        prevEventType = curEventType;
    }

    printf(ANSI_COLOR_RED "\n=============== RUN" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BLUE " %04d " ANSI_COLOR_RESET, runId);
    printf(ANSI_COLOR_RED "SUMMARY ===============\n" ANSI_COLOR_RESET);
    printf("START (event 1):\t%s\n", fRunStartTime.AsSQLString());
    printf("FINISH (event %d):\t%s\n", fNevents, fRunEndTime.AsSQLString());
    printf(ANSI_COLOR_RED "================================================\n" ANSI_COLOR_RESET);

    fDigiTree->Branch("RunHeader", &runHeader);
    runHeader->SetRunId(runId);
    runHeader->SetStartTime(fRunStartTime);
    runHeader->SetFinishTime(fRunEndTime);
    runHeader->SetNEvents(nEv);

    fDigiTree->Write();
    DisposeDecoder();
    fDigiFileOut->Close();
    fRootFileIn->Close();

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::InitDecoder() {

    //    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    fDigiTree = new TTree("cbmsim", "bmn_digit");

    eventHeader = new TClonesArray("BmnEventHeader");
    runHeader = new BmnRunHeader();
    fDigiTree->Branch("EventHeader", &eventHeader);
    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    if (fDetectorSetup[0]) {
        fTrigMapper = new BmnTrigRaw2Digit(fTrigMapFileName, fTrigINLFileName, fDigiTree);
        if (fT0Map == NULL) {
            BmnTrigMapping tm = fTrigMapper->GetT0Map();
            printf("T0 serial 0x%X got from trig mapping\n", tm.serial);
            if (tm.serial > 0) {
                fT0Map = new TriggerMapStructure();
                fT0Map->channel = tm.channel;
                fT0Map->serial = tm.serial;
                fT0Map->slot = tm.slot;
            }
        }
        fTrigMapper->SetSetup(fBmnSetup);
    }

    if (fDetectorSetup[1]) {
        mwpc = new TClonesArray("BmnMwpcDigit");
        fDigiTree->Branch("MWPC", &mwpc);
        fMwpcMapper = new BmnMwpcRaw2Digit(fMwpcMapFileName);
    }

    if (fDetectorSetup[2]) {
        silicon = new TClonesArray("BmnSiliconDigit");
        fDigiTree->Branch("SILICON", &silicon);
        fSiliconMapper = new BmnSiliconRaw2Digit(fPeriodId, fRunId, fSiliconSerials);
    }

    if (fDetectorSetup[3]) {
        gem = new TClonesArray("BmnGemStripDigit");
        fDigiTree->Branch("GEM", &gem);
        fGemMapper = new BmnGemRaw2Digit(fPeriodId, fRunId, fGemSerials, fGemMapFileName);
    }

    if (fDetectorSetup[4]) {
        tof400 = new TClonesArray("BmnTof1Digit");
        fDigiTree->Branch("TOF400", &tof400);
        fTof400Mapper = new BmnTof1Raw2Digit();
        Bool_t FlagTemp = fTof400Mapper->setRun(fPeriodId, fRunId);
        if (FlagTemp == kFALSE) {
            if (fTof400PlaceMapFileName.Sizeof() > 1 && fTof400StripMapFileName.Sizeof() > 1) {
                TString dir = Form("%s%s", getenv("VMCWORKDIR"), "/input/");
                fTof400Mapper->setMapFromFile(dir + fTof400PlaceMapFileName.Data(), dir + fTof400StripMapFileName.Data());
            } else
                cout << "Map for TOF400 are not loaded" << endl;
        }
    }

    if (fDetectorSetup[5]) {
        tof700 = new TClonesArray("BmnTof2Digit");
        fDigiTree->Branch("TOF700", &tof700);
        fTof700Mapper = new BmnTof2Raw2DigitNew(fTof700MapFileName, fRootFileName, fTOF700ReferenceRun, fTOF700ReferenceChamber, fTof700GeomFileName);
        //        fTof700Mapper->print();
        fTof700Mapper->readSlewingT0();
        fTof700Mapper->readSlewing();
        fTof700Mapper->BookSlewingResults();
    }

    if (fDetectorSetup[6]) {
        dch = new TClonesArray("BmnDchDigit");
        fDigiTree->Branch("DCH", &dch);
        fDchMapper = new BmnDchRaw2Digit(fPeriodId, fRunId);
    }

    if (fDetectorSetup[7]) {
        zdc = new TClonesArray("BmnZDCDigit");
        fDigiTree->Branch("ZDC", &zdc);
        fZDCMapper = new BmnZDCRaw2Digit(fZDCMapFileName, fRootFileName, fZDCCalibrationFileName);
        //        fZDCMapper->print();
    }

    if (fDetectorSetup[8]) {
        ecal = new TClonesArray("BmnECALDigit");
        fDigiTree->Branch("ECAL", &ecal);
        fECALMapper = new BmnECALRaw2Digit(fECALMapFileName, fRootFileName, fECALCalibrationFileName);
        //        fECALMapper->print();
    }

    if (fDetectorSetup[9]) {
        land = new TClonesArray("BmnLANDDigit");
        fDigiTree->Branch("LAND", &land);
        fLANDMapper = new BmnLANDRaw2Digit(fLANDMapFileName,
                fLANDClockFileName, fLANDTCalFileName, fLANDDiffSyncFileName,
                fLANDVScintFileName);
    }

    fPedEvCntr = 0; // counter for pedestal events between two spills
    fPedEnough = kFALSE;
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ClearArrays() {
    if (dch) dch->Delete();
    if (mwpc) mwpc->Delete();
    if (gem) gem->Delete();
    if (silicon) silicon->Delete();
    if (tof400) tof400->Delete();
    if (tof700) tof700->Delete();
    if (zdc) zdc->Delete();
    if (ecal) ecal->Delete();
    if (land) land->Delete();
    if (fTrigMapper)
        fTrigMapper->ClearArrays();
    eventHeader->Delete();
    //runHeader->Delete();
    fTimeShifts.clear();
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::DecodeDataToDigiIterate() {
    ClearArrays();
    //            Int_t iEv = fRawTree->GetEntries();
    //            fRawTree->GetEntry(iEv);

    FillTimeShiftsMap();
    BmnEventHeader* headDAQ = (BmnEventHeader*) eventHeaderDAQ->At(0);
    fCurEventType = headDAQ->GetType();

    if (fTrigMapper) {
        fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
        fTrigMapper->FillEvent(tdc);
    }
    GetT0Info(fT0Time, fT0Width);

    if (fCurEventType == kBMNPEDESTAL) {
        if (fPedEvCntr == fEvForPedestals - 1) return kBMNERROR; //FIX return!
        CopyDataToPedMap(adc32, adc128, fPedEvCntr);
        fPedEvCntr++;
    } else { // payload
        if (fPrevEventType == kBMNPEDESTAL) {
            if (fPedEvCntr >= fEvForPedestals - 1) {
                fGemMapper->RecalculatePedestals();
                fSiliconMapper->RecalculatePedestals();
                fPedEvCntr = 0;
                fPedEnough = kTRUE;
            }
        }
        if ((fGemMapper) && (fPedEnough)) fGemMapper->FillEvent(adc32, gem);
        if ((fSiliconMapper) && (fPedEnough)) fSiliconMapper->FillEvent(adc128, silicon);
        if (fDchMapper) fDchMapper->FillEvent(tdc, &fTimeShifts, dch, fT0Time);
        if (fMwpcMapper) fMwpcMapper->FillEvent(hrb, mwpc);
        if (fTof400Mapper) fTof400Mapper->FillEvent(tdc, &fTimeShifts, tof400);
        if (fTof700Mapper) fTof700Mapper->fillEvent(tdc, &fTimeShifts, fT0Time, fT0Width, tof700);
        if (fZDCMapper) fZDCMapper->fillEvent(adc, zdc);
        if (fECALMapper) fECALMapper->fillEvent(adc, ecal);
        if (fLANDMapper) fLANDMapper->fillEvent(tacquila, land);
    }
    new((*eventHeader)[eventHeader->GetEntriesFast()]) BmnEventHeader(headDAQ->GetRunId(), headDAQ->GetEventId(), headDAQ->GetEventTime(), fCurEventType, headDAQ->GetTrig(), kFALSE);
    //        fDigiTree->Fill();
    fPrevEventType = fCurEventType;

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FinishRun() {
    Double_t fSize = Double_t(fLengthRawFile / 1024. / 1024.);
    fRunStartTime = runHeaderDAQ->GetStartTime();
    fRunEndTime = runHeaderDAQ->GetFinishTime();
    Int_t nEv = fNevents;

    if (!UniDbRun::GetRun(fPeriodId, fRunId))
        UniDbRun::CreateRun(fPeriodId, fRunId, fRawFileName, "", NULL, NULL, fRunStartTime, &fRunEndTime, &nEv, NULL, &fSize, NULL);

}

void BmnRawDataDecoder::ResetDecoder(TString file) {
    fNevents = 0;
    syncCounter = 0;
    fRawFileName = file;
    if (fRawFileIn) {
        fclose(fRawFileIn);
        fRawFileIn = NULL;
    }
    fRunId = GetRunIdFromFile(fRawFileName);
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
    fRawTree->Reset();
    fDigiTree->Reset();
    //    fDigiTree->Branch("EventHeader", &eventHeader);
    //    //fDigiTree->Branch("RunHeader", &runHeader);
    //    fDigiTree->Branch("T0", &t0);
    //    fDigiTree->Branch("BC1", &bc1);
    //    fDigiTree->Branch("BC2", &bc2);
    //    fDigiTree->Branch("VETO", &veto);
    //    fDigiTree->Branch("FD", &fd);
    //    fDigiTree->Branch("BD", &bd);
    //    fDigiTree->Branch("DCH", &dch);
    //    fDigiTree->Branch("GEM", &gem);
    //    fDigiTree->Branch("TOF400", &tof400);
    //    fDigiTree->Branch("TOF700", &tof700);
    //    fDigiTree->Branch("ZDC", &zdc);
    //    fDigiTree->Branch("ECAL", &ecal);
    //    fRunId = GetRunIdFromFile(fRawFileName);
    //    fRootFileName = Form("bmn_run%04d_raw.root", fRunId);
    //    fDigiFileName = Form("bmn_run%04d_digi.root", fRunId);
}

BmnStatus BmnRawDataDecoder::DisposeDecoder() {
    if (fGemMap) delete[] fGemMap;
    if (fT0Map) delete[] fT0Map;
    if (fGemMapper) delete fGemMapper;
    if (fSiliconMapper) delete fSiliconMapper;
    if (fDchMapper) delete fDchMapper;
    if (fMwpcMapper) delete fMwpcMapper;
    if (fTrigMapper) delete fTrigMapper;
    if (fTof400Mapper) delete fTof400Mapper;
    if (fTof700Mapper) delete fTof700Mapper;
    if (fZDCMapper) delete fZDCMapper;
    if (fECALMapper) delete fECALMapper;
    if (fLANDMapper) delete fLANDMapper;

    delete sync;
    delete adc32;
    delete adc128;
    delete adc;
    delete tacquila;
    delete tdc;
    delete tqdc_adc;
    delete tqdc_tdc;

    if (gem) delete gem;
    if (dch) delete dch;
    if (mwpc) delete mwpc;
    if (silicon) delete silicon;
    if (tof400) delete tof400;
    if (tof700) delete tof700;
    if (zdc) delete zdc;
    if (ecal) delete ecal;
    if (land) delete land;

    delete eventHeader;
    delete runHeader;
    if (runHeaderDAQ) delete runHeaderDAQ;
    if (eventHeaderDAQ) delete eventHeaderDAQ;
    if (fRawTree) fRawTree->Delete();
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

BmnStatus BmnRawDataDecoder::CopyDataToPedMap(TClonesArray* adcGem, TClonesArray* adcSil, UInt_t ev) {
    if (fGemMapper) {
        Double_t**** pedData = fGemMapper->GetPedData();
        for (UInt_t iAdc = 0; iAdc < adcGem->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adcGem->At(iAdc);

            for (Int_t iSer = 0; iSer < fNGemSerials; ++iSer) {
                if (adcDig->GetSerial() != fGemSerials[iSer]) continue;
                for (UInt_t iSmpl = 0; iSmpl < adcDig->GetNSamples(); ++iSmpl)
                    if (fRunId > GetBoundaryRun(ADC32_N_SAMPLES)) {
                        //                        printf("ser = 0x%x, iSer = %d, ev= %d, ch = %d, iSmpl = %d, sig = %f\n", adcDig->GetSerial(), iSer, ev, adcDig->GetChannel(), iSmpl, (Double_t) (adcDig->GetShortValue())[iSmpl] / 16);
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetShortValue())[iSmpl] / 16;
                    } else
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetUShortValue())[iSmpl] / 16;
                break;
            }
        }
    }
    if (fSiliconMapper) {
        Double_t**** pedData = fSiliconMapper->GetPedData();
        for (UInt_t iAdc = 0; iAdc < adcSil->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adcSil->At(iAdc);

            for (Int_t iSer = 0; iSer < fNSiliconSerials; ++iSer) {
                if (adcDig->GetSerial() != fSiliconSerials[iSer]) continue;
                for (UInt_t iSmpl = 0; iSmpl < adcDig->GetNSamples(); ++iSmpl) {
                    if (fRunId > GetBoundaryRun(ADC128_N_SAMPLES))
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetShortValue())[iSmpl] / 16;
                    else
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetUShortValue())[iSmpl] / 16;
                }
                break;
            }
        }
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
    tqdc_adc = new TClonesArray("BmnTQDCADCDigit");
    tqdc_tdc = new TClonesArray("BmnTDCDigit");
    sync = new TClonesArray("BmnSyncDigit");
    eventHeaderDAQ = new TClonesArray("BmnEventHeader");
    runHeaderDAQ = new BmnRunHeader();
    hrb = new TClonesArray("BmnHRBDigit");
    adc32 = new TClonesArray("BmnADCDigit");
    adc128 = new TClonesArray("BmnADCDigit");
    adc = new TClonesArray("BmnADCDigit");
    tacquila = new TClonesArray("BmnTacquilaDigit");
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("TQDC_ADC", &tqdc_adc);
    fRawTree->SetBranchAddress("TQDC_TDC", &tqdc_tdc);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("EventHeader", &eventHeaderDAQ);
    fRawTree->SetBranchAddress("RunHeader", &runHeaderDAQ);
    fRawTree->SetBranchAddress("HRB", &hrb);
    fRawTree->SetBranchAddress("ADC32", &adc32);
    fRawTree->SetBranchAddress("ADC128", &adc128);
    fRawTree->SetBranchAddress("ADC", &adc);
    fRawTree->SetBranchAddress("Tacquila", &tacquila);

    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    fDigiTree = new TTree("cbmsim", "bmn_digit");

    eventHeader = new TClonesArray("BmnEventHeader");
    runHeader = new BmnRunHeader();
    fDigiTree->Branch("EventHeader", &eventHeader);
    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    fTrigMapper = new BmnTrigRaw2Digit(fTrigMapFileName, fTrigINLFileName, fDigiTree);
    if (fT0Map == NULL) {
        BmnTrigMapping tm = fTrigMapper->GetT0Map();
        printf("T0 serial 0x%X got from trig mapping\n", tm.serial);
        if (tm.serial > 0) {
            fT0Map = new TriggerMapStructure();
            fT0Map->channel = tm.channel;
            fT0Map->serial = tm.serial;
            fT0Map->slot = tm.slot;
        }
    }
    fTrigMapper->SetSetup(fBmnSetup);


    fTof700Mapper = new BmnTof2Raw2DigitNew(fTof700MapFileName, fRootFileName);
    //fTof700Mapper->print();

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::SlewingTOF700() {

    fTof700Mapper->readSlewingLimits();

    fTof700Mapper->BookSlewing();

    for (Int_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0) cout << "Slewing T0 event #" << iEv << endl;

        fTrigMapper->ClearArrays();

        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        FillTimeShiftsMap();

        //if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
        //                cout << "No TimeShiftMap created" << endl;
        //continue;
        //}

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tdc);
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        if (fT0Time == 0.) continue;

        fTof700Mapper->fillSlewingT0(tdc, &fTimeShifts, fT0Time, fT0Width);
    }
    cout << "Slewing T0 event #" << fNevents << endl;

    fTof700Mapper->SlewingT0();

    fTof700Mapper->readSlewingT0();


    for (Int_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0) cout << "Slewing RPC event #" << iEv << endl;

        fTrigMapper->ClearArrays();

        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        FillTimeShiftsMap();

        //if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
        //                cout << "No TimeShiftMap created" << endl;
        //continue;
        //}

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tdc);
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        if (fT0Time == 0.) continue;

        fTof700Mapper->fillSlewing(tdc, &fTimeShifts, fT0Time, fT0Width);
    }
    cout << "Slewing RPC event #" << fNevents << endl;

    fTof700Mapper->Slewing();

    fTof700Mapper->readSlewing();

    fTof700Mapper->InitEqualization();

    for (Int_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0) cout << "Equalization RPC strips event #" << iEv << endl;

        fTrigMapper->ClearArrays();

        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        FillTimeShiftsMap();

        //if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
        //                cout << "No TimeShiftMap created" << endl;
        //continue;
        //}

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tdc);
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        if (fT0Time == 0.) continue;

        fTof700Mapper->fillEqualization(tdc, &fTimeShifts, fT0Time, fT0Width);
    }
    cout << "Equalization RPC strips event #" << fNevents << endl;

    fTof700Mapper->Equalization();

    //    fRootFileIn->Close();

    //    delete trigMapper;
    //    delete tof700Mapper;

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::PreparationTOF700() {

    for (Int_t iEv = 0; iEv < fNevents; ++iEv) {
        if (iEv % 5000 == 0) cout << "Preparation stage event #" << iEv << endl;

        fTrigMapper->ClearArrays();

        fTimeShifts.clear();

        fRawTree->GetEntry(iEv);

        FillTimeShiftsMap();

        //if (FillTimeShiftsMapNoDB(0x6EA9711) == kBMNERROR) {
        //                cout << "No TimeShiftMap created" << endl;
        //continue;
        //}

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tdc);
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        if (fT0Time == 0.) continue;

        fTof700Mapper->fillPreparation(tdc, &fTimeShifts, fT0Time, fT0Width);
    }

    fTof700Mapper->writeSlewingLimits();

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
    fclose(file);
    if (runId <= 0) {
        Int_t run = 0;
        //sscanf(&(((char *)name.Data())[strlen(name.Data())-9]), "%d", &run);
        run = ((TString) name(name.Length() - 9, name.Length() - 5)).Atoi();
        return run;
    } else return runId;
}

BmnStatus BmnRawDataDecoder::InitMaps() {
    //    Int_t fEntriesInGlobMap = 0;
    //    UniDbDetectorParameter* mapPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_global_mapping", fPeriodId, fRunId);
    //    if (mapPar != NULL) mapPar->GetGemMapArray(fGemMap, fEntriesInGlobMap);
    //
    //    for (Int_t i = 0; i < fEntriesInGlobMap; ++i)
    //        if (find(fGemSerials.begin(), fGemSerials.end(), fGemMap[i].serial) == fGemSerials.end())
    //            fGemSerials.push_back(fGemMap[i].serial);
    //    fNGemSerials = fGemSerials.size();
    string dummy;
    UInt_t ser = 0;
    set<UInt_t> seials;
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fGemMapFileName;
    ifstream inFile(name.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 5; ++i) getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> dummy >> dummy >> dummy >> dummy >> dummy;
        if (!inFile.good()) break;
        seials.insert(ser);
    }
    for (auto s : seials) fGemSerials.push_back(s);
    fNGemSerials = fGemSerials.size();

    seials.clear();
    name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fSiliconMapFileName;
    ifstream inFileSil(name.Data());
    if (!inFileSil.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 4; ++i) getline(inFileSil, dummy); //comment line in input file

    while (!inFileSil.eof()) {
        inFileSil >> std::hex >> ser >> std::dec >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
        if (!inFileSil.good()) break;
        seials.insert(ser);
    }
    for (auto s : seials) fSiliconSerials.push_back(s);
    fNSiliconSerials = fSiliconSerials.size();

    seials.clear();
    name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fCSCMapFileName;
    ifstream inFileCSC(name.Data());
    if (!inFileCSC.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 2; ++i) getline(inFileCSC, dummy); //comment line in input file

    while (!inFileCSC.eof()) {
        inFileCSC >> std::hex >> ser >> std::dec >> dummy >> dummy >> dummy >> dummy >> dummy;
        if (!inFileCSC.good()) break;
        seials.insert(ser);
    }
    for (auto s : seials) fCSCSerials.push_back(s);
    fNCSCSerials = fCSCSerials.size();

    fZDCSerials.push_back(0x046f4083);
    fZDCSerials.push_back(0x046f4bb2);
    fNZDCSerials = fZDCSerials.size();

    seials.clear();
    name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fECALMapFileName;
    ifstream inFileECAL(name.Data());
    printf("ECal name = %s\n", name.Data());
    if (!inFileECAL.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 2; ++i) getline(inFileECAL, dummy); //comment line in input file

    while (!inFileECAL.eof()) {
        inFileECAL >> std::hex >> ser >> std::dec >> dummy >> dummy >> dummy >> dummy;
        if (!inFileECAL.good()) break;
        seials.insert(ser);
    }
    for (auto s : seials) fECALSerials.push_back(s);
    fNECALSerials = fECALSerials.size();

    //    Int_t nEntries = 1;
    //    if (mapPar != NULL) delete mapPar;
    //    mapPar = UniDbDetectorParameter::GetDetectorParameter("T0", "T0_global_mapping", fPeriodId, fRunId);
    //    if (mapPar != NULL) {
    //        mapPar->GetTriggerMapArray(fT0Map, nEntries);
    //        delete mapPar;
    //        return kBMNSUCCESS;
    //    } else {
    //        cerr << "No TO map found in DB" << endl;
    //        return kBMNERROR;
    //    }
}

BmnStatus BmnRawDataDecoder::GetT0Info(Double_t& t0time, Double_t &t0width) {
    vector<TClonesArray*>* trigArr = fTrigMapper->GetTrigArrays();
    for (auto ar : *trigArr) {
        BmnTrigDigit* dig = (BmnTrigDigit*) ar->At(0);
        if (fPeriodId > 6) {
            if (!strcmp(ar->GetName(), "BC2") && ar->GetEntriesFast() && (dig->GetMod() == 0)) {
                t0time = dig->GetTime();
                t0width = dig->GetAmp();
                //		printf(" t0 %f t0w %f n %d\n", t0time, t0width, ar->GetEntriesFast());
                return kBMNSUCCESS;
            }
        } else {
            if (!strcmp(ar->GetName(), "T0") && ar->GetEntriesFast()) {
                t0time = dig->GetTime();
                t0width = dig->GetAmp();
                return kBMNSUCCESS;
            }
        }
    }
    return kBMNERROR;
}
