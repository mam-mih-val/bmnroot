
#include "BmnRawDataDecoder.h"

#include <BmnMath.h>
#include <sys/stat.h>
#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */

using namespace std;

class UniDbRun;

BmnRawDataDecoder::BmnRawDataDecoder(TString file, TString outfile, ULong_t nEvents, ULong_t period) {
    string confFileName = string(getenv("VMCWORKDIR")) + "/config/bmnconf.json";
    //    conf.put<string>("Decoder.AdcDecoMode", "MK");
    //    conf.put<string>("Decoder.DigiTreeName", "bmndata");
    //    conf.put<string>("Decoder.DigiTreeTitle", "bmndigit");
    //    pt::write_json(confFileName, conf);
    pt::read_json(confFileName, conf);
    string decoMode = conf.get<string>("Decoder.AdcDecoMode", "");
    if (decoMode.find("SM") != string::npos)
        SetAdcDecoMode(kBMNADCSM);
    else
        SetAdcDecoMode(kBMNADCMK);
    eventHeaderDAQ = NULL;
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
    if (fRawFileName != "") {
        fRunId = GetRunIdFromFile(fRawFileName);
        fDigiFileName = (outfile == "") ? Form("bmn_run%04d_digi.root", fRunId) : outfile;
        Int_t lastSlash = fDigiFileName.Last('/');
        TString digiPath = (lastSlash == TString::kNPOS) ? "" : TString(fDigiFileName(0, lastSlash + 1));
        fRootFileName = Form("%sbmn_run%04d_raw.root", digiPath.Data(), fRunId);
    }
    fDchMapFileName = "";
    fMwpcMapFileName = "";
    fTrigPlaceMapFileName = "";
    fTrigChannelMapFileName = "";
    fGemMapFileName = "";
    fCscMapFileName = "";
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
    fDigiRunHdrName = "DigiRunHeader";
    fDat = 0;
    fGemMapper = NULL;
    fCscMapper = NULL;
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
    tai_utc_dif = 0;
    fVerbose = 0;
    isSpillStart = kFALSE;
    fSpillCntr = 0;
    InitUTCShift();
    //InitMaps();
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
    isSpillStart = kTRUE;

    for (;;) {
        if (fMaxEvent > 0 && fNevents == fMaxEvent) break;
        //if (fread(&dat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
        fread(&fDat, kWORDSIZE, 1, fRawFileIn);
        fCurentPositionRawFile = ftello64(fRawFileIn);
        if (fCurentPositionRawFile >= fLengthRawFile) break;
        if (fDat == kENDOFSPILL) {
            //                printf("EOS!!\n");
            isSpillStart = kTRUE;
        }
        if (fDat == kSYNC1) { //search for start of event
            // read number of bytes in event
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) continue;
            fDat = fDat / kNBYTESINWORD + 1; // bytes --> words
            if (fDat >= 100000) { // what the constant?
                printf("Wrong data size: %d:  skip this event\n", fDat);
                fread(data, kWORDSIZE, fDat, fRawFileIn);
            } else {
                //read array of current event data and process them
                if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) continue;
                fEventId = data[0];
                if (fEventId <= 0) continue; // skip bad events
                ProcessEvent(data, fDat);
                if (data[0] != (fNevents + 1)) // Just a check to see if somehow ProcessEvent messed up our counting
                    printf(ANSI_COLOR_RED "***Extreme warning, events are not synced: %i, %i***\n" ANSI_COLOR_RESET, fEventId, fNevents + 1);
                fNevents++;
                fRawTree->Fill();
            }
        }
    }

    fRunEndTime = TTimeStamp(time_t(fTime_s), fTime_ns);
    Int_t shift = GetUTCShift(fRunEndTime);
    if (shift != tai_utc_dif)
        fprintf(stderr, ANSI_COLOR_RED "Critical Warning! Leap second added during the %i run!\n\n" ANSI_COLOR_RESET, fRunId);
    //    fRunEndTime = TTimeStamp(time_t(fTime_s - shift), fTime_ns);
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
    eventHeaderDAQ = new BmnEventHeader();

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
    fRawTree->Branch("BmnEventHeader.", &eventHeaderDAQ);
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
    fEventId = buf[0];
    //            printf("EventID = %d\n", fEventId);
    if (fEventId <= 0) return kBMNERROR;
    ProcessEvent(buf, len);
    fNevents++;
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
    //    eventHeaderDAQ->Delete();
    BmnTrigInfo* trigInfo = new BmnTrigInfo();
    if (fVerbose == 0)
        DrawBar(fCurentPositionRawFile, fLengthRawFile);

    Long64_t idx = 1;
    BmnEventType evType = kBMNPAYLOAD;

    while (idx < len) {
        UInt_t serial = d[idx++];
        UInt_t id = (d[idx] >> 24);
        UInt_t payload = (d[idx++] & 0xFFFFFF) / kNBYTESINWORD;
        if (payload > 2000000) {
            printf("[WARNING] Event %d:\n serial = 0x%06X\n id = Ox%02X\n payload = %d\n", fEventId, serial, id, payload);
            break;
        }
        switch (id) {
            case kADC64VE_XGE:
            case kADC64VE:
            {
                //                printf("\n\npayload %d id 0x%02X serial 0%08X\n", payload, id, serial);
                //                Bool_t isFound = kFALSE;
                //                for (Int_t iSer = 0; iSer < fNSiliconSerials; ++iSer)
                //                    if (serial == fSiliconSerials[iSer]) {
                //                        Process_ADC64VE(&d[idx], payload, serial, 128, adc128);
                //                        isFound = kTRUE;
                //                        break;
                //                    }
                //                if (isFound) break;
                //                for (Int_t iSer = 0; iSer < fNGemSerials; ++iSer)
                //                    if (serial == fGemSerials[iSer]) {
                //                        Process_ADC64VE(&d[idx], payload, serial, 32, adc32);
                //                        isFound = kTRUE;
                //                        break;
                //                    }
                //                if (isFound) break;
                //                for (Int_t iSer = 0; iSer < fNCscSerials; ++iSer)
                //                    if (serial == fCscSerials[iSer]) {
                //                        Process_ADC64VE(&d[idx], payload, serial, 32, adc32);
                //                        isFound = kTRUE;
                //                        break;
                //                    }
                bitset<kWORDSIZE * 8> chMaskLo(d[idx + 3]);
                bitset<kWORDSIZE * 8> chMaskHi(d[idx + 4]);
                Int_t nCh = chMaskHi.count() + chMaskLo.count(); //64;
                payload -= 5;
                idx += 5;
                Double_t nSmpl = 2 * (payload / (Double_t) nCh - 3); // MStream ADC payload count
                //                printf("nCh %d nSmpl %f\n", nCh, nSmpl);
                if (nSmpl > 100.0)
                    Process_ADC64VE(&d[idx], payload, serial, 128, adc128);
                else
                    Process_ADC64VE(&d[idx], payload, serial, 32, adc32);
                break;
            }
            case kADC64WR:
            {
                Bool_t isZDC = kFALSE;
                Bool_t isECAL = kFALSE;
                for (Int_t iSer = 0; (iSer < fNZDCSerials); ++iSer) {
                    if (serial == fZDCSerials[iSer]) {
                        isZDC = kTRUE;
                        break;
                    }
                };
                if (isZDC)
                    Process_ADC64WR(&d[idx], payload, serial, adc);
                else {
                    for (Int_t iSer = 0; (iSer < fNECALSerials); ++iSer) {
                        if (serial == fECALSerials[iSer]) {
                            isECAL = kTRUE;
                            break;
                        }
                    };
                    if (isECAL)
                        Process_ADC64WR(&d[idx], payload, serial, adc);
                }
                //if (isECAL) printf("Serial 0x%08x %d %d\n",serial,isZDC,isECAL);
                break;
            }
            case kFVME:
                Process_FVME(&d[idx], payload, serial, evType, trigInfo);
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
    //    new((*eventHeaderDAQ)[eventHeaderDAQ->GetEntriesFast()]) BmnEventHeader(fRunId, fEventId, TTimeStamp(time_t(fTime_s), fTime_ns), evType, kFALSE, trigInfo);
    eventHeaderDAQ->SetRunId(fRunId);
    eventHeaderDAQ->SetPeriodId(fPeriodId);
    eventHeaderDAQ->SetEventId(fEventId);
    eventHeaderDAQ->SetEventTimeTS(TTimeStamp(time_t(fTime_s), fTime_ns));
    eventHeaderDAQ->SetEventTime(TTimeStamp(time_t(fTime_s), fTime_ns).AsDouble());
    eventHeaderDAQ->SetEventType(evType);
    eventHeaderDAQ->SetTripWord(kFALSE);
    eventHeaderDAQ->SetTrigInfo(trigInfo);
    eventHeaderDAQ->SetTimeShift(fTimeShifts);
    eventHeaderDAQ->SetStartSignalInfo(fT0Time, fT0Width);
    eventHeaderDAQ->SetSpillStart(isSpillStart);
    if (isSpillStart == kTRUE)
        isSpillStart = kFALSE;
}

/**
 * Parse ADC64VE 
 * format Mstream Waveform V2 from https://afi.jinr.ru/MStreamWaveformDigitizer
 * @param d Data array ptr
 * @param len payload length
 * @param serial 
 * @param nSmpl
 * @param arr ADC digits storage
 * @return kBMNSUCCESS
 */
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
    //    while (i < len) {
    //        UInt_t subType = d[i] & 0x3;
    ////        printf("serial = 0x%X   subType = %d  nSmpl = %d i  =%d\n", serial, subType, nSmpl, i);
    ////        printf(" other word[0] part = %08X\n", (d[i] & (((1UL << 32) - 1) - 0x3)));
    ////        printf(" TAI      s = %d  ns = %lu TAI flag = %d\n", d[i + 1], (d[i + 2] & (((1UL << 32) - 1) - 0x3)), (d[i + 2] & 0x3));
    ////        printf(" readout channels  (0-31) = %08X  (32-63) = %08X\n", d[i + 3], d[i + 4]);
    //        if (subType == 0) {
    //            i += 5; //skip unused words
    UInt_t iCh = 0;
    while (iCh < kNCH - 1 && i < len) {
        iCh = d[i] >> 24;
        if (iCh > 64) {
            printf("serial = 0x%X   iCh = %d  nSmpl = %d\n", serial, iCh, nSmpl);
            break;
        }
        //                UInt_t subType = d[i];
        //                printf("            subType word = %d  nSmpl = %d\n", subType, nSmpl);
        //                printf("          s = %d  ns = %lu TAI flag = %d\n", d[i + 1], (d[i + 2] & (((1UL << 32) - 1) - 0x3)), (d[i + 2] & 0x3));
        i += 3; // skip two timestamp words (they contain TAI timestsamps)
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
    //        } else break;
    //    }
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
                //                printf("WR serial %08X ns = %d\n", serial, ns);
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

BmnStatus BmnRawDataDecoder::Process_FVME(UInt_t *d, UInt_t len, UInt_t serial, BmnEventType &evType, BmnTrigInfo* spillInfo) {
    //                                printf("FVME serial %08X\n",serial);
    UInt_t modId = 0;
    UInt_t slot = 0;
    UInt_t type = 0;
    for (UInt_t i = 0; i < len; i++) {
        type = d[i] >> 28;
        switch (type) {
            case kEVHEADER:
                //                printf("Ev header %d\n",(d[i] & 0xFFFFF));
            case kEVTRAILER:
                //                printf("Ev trailer\n");
            case kSPILLHEADER:
                //                printf("SPILLHEADER  spill type %i\n", ((d[i] >> 26) &0x1));
            case kSPILLTRAILER:
                //                printf("SPILLTRAILER spill type %i\n", ((d[i] >> 26) &0x1));
            case kSTATUS:
            case kPADDING:
                break;
            case kMODHEADER:
                modId = (d[i] >> 16) & 0x7F;
                slot = (d[i] >> 23) & 0x1F;
                //                printf("modid 0x%02X slot %d serial 0x%08X\n", modId, slot, serial);
                break;
            case kMODTRAILER:
                //                printf("module trailer\n");
                if (!((d[i] >> 16) & 0x1)) printf(ANSI_COLOR_RED "Readout overflow error\n" ANSI_COLOR_RESET);
                if (!((d[i] >> 17) & 0x1)) printf(ANSI_COLOR_RED "Readout error\n" ANSI_COLOR_RESET);
                if (!((d[i] >> 18) & 0x1)) printf(ANSI_COLOR_RED "TTC error\n" ANSI_COLOR_RESET);
                if (!((d[i] >> 19) & 0x1)) printf(ANSI_COLOR_RED "Access error\n" ANSI_COLOR_RESET);
                modId = 0x00;
                slot = 0x00;
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
                    case kTQDC16VS_ETH:
                        FillTQDC(d, serial, slot, modId, i);
                        break;
                    case kMSC:
                        FillMSC(d, serial, i); //empty now
                        break;
                    case kTRIG:
                        FillSYNC(d, serial, i);
                        break;
                    case kU40VE_RC:
                        FillU40VE(d, evType, slot, i, spillInfo);
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

BmnStatus BmnRawDataDecoder::FillU40VE(UInt_t *d, BmnEventType &evType, UInt_t slot, UInt_t & idx, BmnTrigInfo* trigInfo) {
    UInt_t type = d[idx] >> 28;
    //            printf("start type %u slot %u\n", type, slot);
    Bool_t countersDone = kFALSE;
    while (type == kWORDTAI || type == kWORDTRIG || type == kWORDAUX) {
        if (fPeriodId > 4 && type == kWORDTRIG && slot == kEVENTTYPESLOT) {
            evType = ((d[idx] & BIT(3)) >> 3) ? kBMNPEDESTAL : kBMNPAYLOAD;
            UInt_t trigSrc = ((d[idx] >> 16) & (BIT(8) - 1));
            //            printf("trig source %u\n", trigSrc);
            //            if (!( ((d[idx]>>10) & 0x1) ^ (fPeriodId >= 7 && fBmnSetup == kBMNSETUP)))
            //                printf("Ev not Good!\n");
            //            printf("evGood %d\n", (d[idx] & BIT(10)));
            if (evType == kBMNPEDESTAL)
                fPedoCounter++;
        }
        if (type == kWORDAUX && !countersDone) {
            trigInfo->SetTrigCand(d[idx + 0] & 0xFFFFFFF);
            trigInfo->SetTrigAccepted(d[idx + 1] & 0xFFFFFFF);
            trigInfo->SetTrigBefo(d[idx + 2] & 0xFFFFFFF);
            trigInfo->SetTrigAfter(d[idx + 3] & 0xFFFFFFF);
            trigInfo->SetTrigRjct(d[idx + 4] & 0xFFFFFFF);
            trigInfo->SetTrigAll(d[idx + 5] & 0xFFFFFFF);
            trigInfo->SetTrigAvail(d[idx + 6] & 0xFFFFFFF);
            //            for (Int_t j = 0; j <= 6; j++){
            //            type = (d[idx + j] >> 28) & (BIT(5) - 1);
            //            printf("%d type = %u %08X\n", j, type, type);
            //            }
            idx += 4;
            //            printf("cand %04u, acc %04u, bef %04u, after %04u, rjct %04u, all %04u, avail %04u\n",
            //                    trigInfo->GetTrigCand(),
            //                    trigInfo->GetTrigAccepted(),
            //                    trigInfo->GetTrigBefo(),
            //                    trigInfo->GetTrigAfter(),
            //                    trigInfo->GetTrigRjct(),
            //                    trigInfo->GetTrigAll(),
            //                    trigInfo->GetTrigAvail());
            countersDone = kTRUE;
        }
        idx++; //go to the next DATA-word
        type = d[idx] >> 28;
    }
    idx--;
}

BmnStatus BmnRawDataDecoder::FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t & idx) {
    UInt_t type = d[idx] >> 28;
    while (type != kMODTRAILER) { //data will be finished when module trailer appears
        if (type == 6) {
            fprintf(stderr, ANSI_COLOR_RED "Warning: TDC (modID 0x%02X serial 0x%08X slot %d tdcID %d) error code: 0x%04X\n" ANSI_COLOR_RESET,
                    modId, serial, slot, ((d[idx] >> 24) & 0xF), d[idx]); //(d[idx] & ((1<<15) - 1)));
            if (((d[idx] >> 12) & 0x1) || ((d[idx] >> 13) & 0x1)) {
                fprintf(stderr, ANSI_COLOR_RED "Warning: Critical TDC error thrown\n" ANSI_COLOR_RESET);
                return kBMNERROR;
            }
        }
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
    UInt_t type = d[idx] >> 28;
    UShort_t trigTimestamp = 0;
    UShort_t adcTimestamp = 0;
    UShort_t tdcTimestamp = 0;
    UInt_t iSampl = 0;
    UInt_t channel = 0;
    Short_t valI[ADC_SAMPLING_LIMIT];
    Bool_t inADC = kFALSE;
    while (type != kMODTRAILER) {
        if (type == 6) {
            fprintf(stderr, ANSI_COLOR_RED "ERROR: TDC (serial 0x%08X slot %d tdcID %d) error code: 0x%04X\n" ANSI_COLOR_RESET,
                    serial, slot, ((d[idx] >> 24) & ((1 << 4) - 1)), (d[idx] & ((1 << 15) - 1)));
            if (((d[idx] >> 12) & 0x1) || ((d[idx] >> 13) & 0x1)) {
                fprintf(stderr, ANSI_COLOR_RED "ERROR: Critical TQDC error thrown\n" ANSI_COLOR_RESET);
                return kBMNERROR;
            }
        }
        UInt_t mode = (d[idx] >> 26) & 0x3;
        if (!inADC) { //       printf("type %d mode %d word %0X\n", type, mode, d[idx]);
            if ((mode == 0) && (type == 4 || type == 5)) { // TDC time
                channel = (d[idx] >> 19) & 0x1F;
                UInt_t time = ((d[idx] & 0x7FFFF) << 2) | (d[idx] >> 24) & 0x3; // in 25 ps
                //               printf("TDC time %d channel %d\n", time, channel);
                new((*tqdc_tdc)[tqdc_tdc->GetEntriesFast()]) BmnTDCDigit(serial, modId, slot, (type == 4), channel, 0, time, tdcTimestamp);
            } else if ((type == 4) && (mode != 0)) { // Trig | ADC Timestamp
                channel = (d[idx] >> 19) & 0x1F;
                if (d[idx] & BIT(16)) { // ADC TS
                    adcTimestamp = d[idx] & 0xFFFF;
                    inADC = kTRUE;
                } else {// Trig TS
                    trigTimestamp = d[idx] & 0xFFFF;
                }
            } else if (type == 2) {
                tdcTimestamp = d[idx] & 0xFFF;
                // UInt_t iEv = (d[idx] >> 12) & 0xFFF;
                //                printf("TDC ev header: %d\n", iEv);
            } else if (type == 3) {
                // UInt_t iEv = (d[idx] >> 12) & 0xFFF;
                //                printf("TDC ev trailer: %d\n", iEv);
            }
        } else {
            if ((type == 5) && (mode == 2) && (iSampl < ADC_SAMPLING_LIMIT)) {
                Short_t val = (d[idx] & ((1 << 14) - 1)) - (1 << (14 - 1));
                valI[iSampl++] = val;
            } else {
                new((*tqdc_adc)[tqdc_adc->GetEntriesFast()]) BmnTQDCADCDigit(serial, channel, slot, iSampl, valI, trigTimestamp, adcTimestamp);
                inADC = kFALSE;
                iSampl = 0;
                --idx;
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

    if (tai_utc_dif == 0)
        tai_utc_dif = GetUTCShift(TTimeStamp(time_t(ts_t0_s), ts_t0_ns));

    fTime_ns = ts_t0_ns;
    fTime_s = ts_t0_s - tai_utc_dif;

    TClonesArray &ar_sync = *sync;
    new(ar_sync[sync->GetEntriesFast()]) BmnSyncDigit(serial, GlobalEvent, fTime_s, fTime_ns);

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
    eventHeaderDAQ = new BmnEventHeader();
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("TQDC_ADC", &tqdc_adc);
    fRawTree->SetBranchAddress("TQDC_TDC", &tqdc_tdc);
    fRawTree->SetBranchAddress("HRB", &hrb);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("ADC32", &adc32);
    fRawTree->SetBranchAddress("ADC128", &adc128);
    fRawTree->SetBranchAddress("ADC", &adc);
    fRawTree->SetBranchAddress("Tacquila", &tacquila);
    fRawTree->SetBranchAddress("BmnEventHeader.", &eventHeaderDAQ);

    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    BmnEventType curEventType = kBMNPAYLOAD;
    BmnEventType prevEventType = curEventType;
    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;
    /****************** 1st Iter of Si/GEM mapper   *************/
    InitDecoder();
    if (GetAdcDecoMode() == kBMNADCMK) {
        if (fSiliconMapper) fSiliconMapper->InitAdcProcessorMK(fRunId, 0, 0, 0, 0);
        if (fGemMapper) fGemMapper->InitAdcProcessorMK(fRunId, 0, 0, 0, 0);
        printf("\n[INFO]" ANSI_COLOR_BLUE " Processing pedestals\n" ANSI_COLOR_RESET);
        if (fSiliconMapper) fSiliconMapper->LoadPedestalsMK(fRawTree, adc128, eventHeaderDAQ, Min(fNevents, (UInt_t) 100000));
        if (fGemMapper) fGemMapper->LoadPedestalsMK(fRawTree, adc32, eventHeaderDAQ, Min(fNevents, (UInt_t) 100000));
        printf("[INFO]" ANSI_COLOR_BLUE " First payload loop\n" ANSI_COLOR_RESET);
        for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
        if (fVerbose == 1) {
            if (iEv % 5000 == 0) cout << "1st Digitization event #" << iEv << "/" << fNevents << ";" << endl;
        } else if (fVerbose == 0)
            DrawBar(iEv, fNevents);
            ClearArrays();
            fRawTree->GetEntry(iEv);
            BmnEventHeader* headDAQ = eventHeaderDAQ;
            if (!headDAQ) continue;
            curEventType = headDAQ->GetEventType();
            fEventId = headDAQ->GetEventId();
            FillTimeShiftsMap();
            if (curEventType == kBMNPEDESTAL) continue;
            //        for (UInt_t iAdc = 0; iAdc < adc32->GetEntriesFast(); ++iAdc) {
            //            BmnADCDigit* adcDig = (BmnADCDigit*) adc32->At(iAdc);
            //            printf("gem ser 0x%08X\n", adcDig->GetSerial());
            //        }
            if (fSiliconMapper) fSiliconMapper->FillEventMK(adc128, nullptr);
            if (fGemMapper) fGemMapper->FillEventMK(adc32, nullptr, nullptr);
            //        prevEventType = curEventType;
        }
    } else if (fGemMapper || fSiliconMapper || fCscMapper) {
        printf("\n[INFO]" ANSI_COLOR_BLUE "1st iter Collecting data for ADC pedestals calculation:\n" ANSI_COLOR_RESET);
        printf("\tNumber of requested pedestal events is ");
        printf(ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, fEvForPedestals);
        for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
            if (fVerbose == 0)
                DrawBar(fPedEvCntr, fEvForPedestals);
            fRawTree->GetEntry(iEv);

            BmnEventHeader* headDAQ = eventHeaderDAQ;
            if (!headDAQ) continue;
            curEventType = headDAQ->GetEventType();
            if (curEventType != kBMNPEDESTAL) continue;
            if (fPedEvCntr != fEvForPedestals) {
                CopyDataToPedMap(adc32, adc128, fPedEvCntr);
                fPedEvCntr++;
            } else break;
        }
        if (fPedEvCntr != fEvForPedestals) {
            printf(ANSI_COLOR_RED "\n[WARNING]" ANSI_COLOR_RESET);
            printf(" Not enough pedestal events (%d instead of %d)\n", fPedEvCntr, fEvForPedestals);
        }
        if (fGemMapper) fGemMapper->RecalculatePedestalsAugmented();
        if (fSiliconMapper) fSiliconMapper->RecalculatePedestalsAugmented();
        if (fCscMapper)fCscMapper->RecalculatePedestalsAugmented();
        fPedEvCntr = 0;
        printf("\n[INFO]" ANSI_COLOR_BLUE " Clear noisy channels:\n" ANSI_COLOR_RESET);
        printf("\tFilling signal profiles for station-module-layer histograms\n");
        //        UInt_t nEvForNoiseCorrection = 10000;
        //                printf("\tNumber of requested events is ");
        //                printf(ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, nEvForNoiseCorrection);
        //                printf("\tActual number of events is ");
        //                printf(ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, fNevents);        
        UInt_t n = fNevents; //Min(fNevents, nEvForNoiseCorrection);
        for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
            fRawTree->GetEntry(iEv);
            BmnEventHeader* headDAQ = eventHeaderDAQ;
            if (!headDAQ) continue;
            curEventType = headDAQ->GetEventType();
            if (curEventType == kBMNPEDESTAL) {
                if (fPedEvCntr == fEvForPedestals - 1) continue;
                CopyDataToPedMap(adc32, adc128, fPedEvCntr);
                fPedEvCntr++;
            } else { // payload
                if (prevEventType == kBMNPEDESTAL && fPedEvCntr == fEvForPedestals - 1) {
                    if (fGemMapper) fGemMapper->RecalculatePedestalsAugmented();
                    if (fSiliconMapper) fSiliconMapper->RecalculatePedestalsAugmented();
                    if (fCscMapper)fCscMapper->RecalculatePedestalsAugmented();
                    fPedEvCntr = 0;
                }
            }
            if (fGemMapper) fGemMapper->FillProfiles(adc32);
            if (fSiliconMapper) fSiliconMapper->FillProfiles(adc128);
            if (fCscMapper) fCscMapper->FillProfiles(adc32);
            prevEventType = curEventType;
            if (fVerbose == 0)
                DrawBar(iEv, n);
        }
        printf("\tMarking noisy channels\n");
        if (fGemMapper) fGemMapper->FillNoisyChannels();
        if (fSiliconMapper) fSiliconMapper->FillNoisyChannels();
        if (fCscMapper) fCscMapper->FillNoisyChannels();
    }

    /******************  End of the 1st iter ******************/
    curEventType = kBMNPAYLOAD;
    prevEventType = curEventType;
    fPedEvCntr = 0;
    if (GetAdcDecoMode() == kBMNADCMK) {
        printf("\n[INFO]" ANSI_COLOR_BLUE " Processing pedestals\n" ANSI_COLOR_RESET);
        if (fSiliconMapper) {
            delete fSiliconMapper;
            fSiliconMapper = new BmnSiliconRaw2Digit(fPeriodId, fRunId, fSiliconSerials, fBmnSetup, GetAdcDecoMode());
            fSiliconMapper->InitAdcProcessorMK(fRunId, 1, 0, 0, 0);
            fSiliconMapper->LoadPedestalsMK(fRawTree, adc128, eventHeaderDAQ, Min(fNevents, (UInt_t) 100000));
            //            fSiliconMapper->RecalculatePedestalsMK(fPedEvCntr);
        }
        if (fGemMapper) {
            delete fGemMapper;
            fGemMapper = new BmnGemRaw2Digit(fPeriodId, fRunId, fGemSerials, fGemMapFileName, fBmnSetup, GetAdcDecoMode());
            fGemMapper->InitAdcProcessorMK(fRunId, 1, 0, 0, 0);
            fGemMapper->LoadPedestalsMK(fRawTree, adc32, eventHeaderDAQ, Min(fNevents, (UInt_t) 100000));
            //            fGemMapper->RecalculatePedestalsMK(fPedEvCntr);
        }
        //        InitDecoder();
    } else
        if (fGemMapper || fSiliconMapper || fCscMapper) {
        printf("\n[INFO]");
        printf(ANSI_COLOR_BLUE " Collecting data for ADC pedestals calculation:\n" ANSI_COLOR_RESET);
        printf("\tNumber of requested pedestal events is ");
        printf(ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, fEvForPedestals);
        for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
            if (fVerbose == 0)
                DrawBar(fPedEvCntr, fEvForPedestals);
            fRawTree->GetEntry(iEv);

            BmnEventHeader* headDAQ = eventHeaderDAQ;
            if (!headDAQ) continue;
            curEventType = headDAQ->GetEventType();

            if (curEventType != kBMNPEDESTAL) continue;
            if (fPedEvCntr != fEvForPedestals) {
                CopyDataToPedMap(adc32, adc128, fPedEvCntr);
                fPedEvCntr++;
            } else break;
        }
        if (fPedEvCntr != fEvForPedestals) {
            printf(ANSI_COLOR_RED "\n[WARNING]" ANSI_COLOR_RESET);
            printf(" Not enough pedestal events (%d instead of %d)\n", fPedEvCntr, fEvForPedestals);
        }
        if (fGemMapper) fGemMapper->RecalculatePedestalsAugmented();
        if (fSiliconMapper) fSiliconMapper->RecalculatePedestalsAugmented();
        if (fCscMapper) fCscMapper->RecalculatePedestalsAugmented();
        fPedEvCntr = 0;

        //        UInt_t nEvForNoiseCorrection = 10000;
        //        printf("\n[INFO]");
        //        printf(ANSI_COLOR_BLUE " Clear noisy channels:\n" ANSI_COLOR_RESET);
        //        printf("\tFilling signal profiles for station-module-layer histograms\n");
        //        printf("\tNumber of requested events is ");
        //        printf(ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, nEvForNoiseCorrection);
        //        printf("\tActual number of events is ");
        //        printf(ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, fNevents);
        //
        //        UInt_t n = Min(fNevents, nEvForNoiseCorrection);
        //        for (UInt_t iEv = 0; iEv < n; ++iEv) {
        //            fRawTree->GetEntry(iEv);
        //            BmnEventHeader* headDAQ = eventHeaderDAQ;
        //            if (!headDAQ) continue;
        //            curEventType = headDAQ->GetEventType();
        //            if (curEventType == kBMNPEDESTAL) continue;
        //            if (fGemMapper) fGemMapper->FillProfiles(adc32);
        //            if (fSiliconMapper) fSiliconMapper->FillProfiles(adc128);
        //            if (fCscMapper) fCscMapper->FillProfiles(adc32);
        //            DrawBar(iEv, n);
        //        }
        //        if (fGemMapper) fGemMapper->FillNoisyChannels();
        //        if (fSiliconMapper) fSiliconMapper->FillNoisyChannels();
        //        if (fCscMapper) fCscMapper->FillNoisyChannels();
    }
    Double_t fSize = 0.0;
    UInt_t runId = 0;
    vector<UInt_t> startTripEvent;
    vector<UInt_t> endTripEvent;

    printf("\n[INFO]");
    printf(ANSI_COLOR_BLUE " Main loop over events:\n" ANSI_COLOR_RESET);
    for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
        ClearArrays();

        fRawTree->GetEntry(iEv);

        BmnEventHeader* headDAQ = eventHeaderDAQ;
        curEventType = headDAQ->GetEventType();
        fEventId = headDAQ->GetEventId();
        FillTimeShiftsMap();
        if (!headDAQ) continue;

        if (iEv == 0) {

            fSize = Double_t(fLengthRawFile / 1024. / 1024.);

            //            if (!UniDbRun::GetRun(fPeriodId, runId))
            //                UniDbRun::CreateRun(fPeriodId, runId, TString::Format("/nica/data4mpd1/dataBMN/bmndata2/run6/raw/mpd_run_Glob_%d.data", runId), "", NULL, NULL, fRunStartTime, &fRunEndTime, &nEv, NULL, &fSize, NULL);

            //check for trip information
            //            UniDbTangoData db_tango;
            //            enumConditions condition = conditionEqual;
            //            bool condition_value = 1;
            //            int map_channel[] = {1, 3, 0, 5, 2, 6, 4};
            //            TString date_start = fRunStartTime.AsSQLString(); // 1252 run
            //            TString date_end = fRunEndTime.AsSQLString();
            //
            //            UInt_t runLength = fRunEndTime.Convert() - fRunStartTime.Convert(); //in seconds
            //            Double_t timeStep = runLength * 1.0 / fNevents; //time for one event
            //            //printf("Run duration = %d sec.\t TimeStep = %f sec./event\n", runLength, timeStep);
            //
            //            TObjArray* tango_data_gem = db_tango.SearchTangoIntervals((char*) "gem", (char*) "trip", (char*) date_start.Data(), (char*) date_end.Data(), condition, condition_value, map_channel);
            //            if (tango_data_gem) {
            //                for (Int_t i = 0; i < tango_data_gem->GetEntriesFast(); ++i) {
            //                    TObjArray* currGemTripInfo = (TObjArray*) tango_data_gem->At(i);
            //                    if (currGemTripInfo->GetEntriesFast() != 0)
            //                        for (Int_t j = 0; j < currGemTripInfo->GetEntriesFast(); ++j) {
            //                            TangoTimeInterval* ti = (TangoTimeInterval*) currGemTripInfo->At(j);
            //                            startTripEvent.push_back(UInt_t((ti->start_time.Convert() - fRunStartTime.Convert()) / timeStep));
            //                            endTripEvent.push_back(UInt_t((ti->end_time.Convert() - fRunStartTime.Convert()) / timeStep));
            //                        }
            //                }
            //            }
        }

        Bool_t isTripEvent = kFALSE;
        for (Int_t iTrip = 0; iTrip < startTripEvent.size(); ++iTrip) {
            if (headDAQ->GetEventId() > startTripEvent[iTrip] && headDAQ->GetEventId() < endTripEvent[iTrip]) {
                isTripEvent = kTRUE;
                break;
            }
        }
        fSpillCntr += headDAQ->GetSpillStart() ? 1 : 0;
        if (fVerbose == 1) {
            if (iEv % 5000 == 0) cout << "Digitization event #" << iEv << "/" << fNevents << "; Spill #"<< fSpillCntr << endl;
        } else if (fVerbose == 0)
            DrawBar(iEv, fNevents);

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
            fTrigMapper->FillEvent(tdc);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        //        new((*eventHeader)[eventHeader->GetEntriesFast()]) BmnEventHeader(headDAQ->GetRunId(), headDAQ->GetEventId(), TTimeStamp(time_t(fTime_s), fTime_ns),
        //                curEventType, isTripEvent, headDAQ->GetTrigInfo(), fTimeShifts);
        eventHeader->SetRunId(headDAQ->GetRunId());
        eventHeader->SetEventId(headDAQ->GetEventId());
        eventHeader->SetPeriodId(headDAQ->GetPeriodId());
        eventHeader->SetEventTimeTS(TTimeStamp(time_t(fTime_s), fTime_ns));
        eventHeader->SetEventTime(TTimeStamp(time_t(fTime_s), fTime_ns).AsDouble());
        eventHeader->SetEventType(curEventType);
        eventHeader->SetTripWord(isTripEvent);
        eventHeader->SetTrigInfo(headDAQ->GetTrigInfo());
        eventHeader->SetTimeShift(fTimeShifts);
        eventHeader->SetStartSignalInfo(fT0Time, fT0Width);
        eventHeader->SetSpillStart(headDAQ->GetSpillStart());
        
        if (curEventType == kBMNPEDESTAL && GetAdcDecoMode() == kBMNADCSM) {
            if (fPedEvCntr == fEvForPedestals - 1) continue;
            CopyDataToPedMap(adc32, adc128, fPedEvCntr);
            fPedEvCntr++;
        } else { // payload
            if (prevEventType == kBMNPEDESTAL && fPedEvCntr == fEvForPedestals - 1) {
                if (fGemMapper) fGemMapper->RecalculatePedestalsAugmented();
                if (fSiliconMapper) fSiliconMapper->RecalculatePedestalsAugmented();
                if (fCscMapper)fCscMapper->RecalculatePedestalsAugmented();
                fPedEvCntr = 0;
            }
            if (GetAdcDecoMode() == kBMNADCSM) {
                if (fCscMapper) fCscMapper->FillEvent(adc32, csc);
                if (fGemMapper) fGemMapper->FillEvent(adc32, gem);
                if (fSiliconMapper) fSiliconMapper->FillEvent(adc128, silicon);
            } else {
                if (fGemMapper) fGemMapper->FillEventMK(adc32, gem, csc);
                if (fSiliconMapper) fSiliconMapper->FillEventMK(adc128, silicon);
            }
            if (fDchMapper) fDchMapper->FillEvent(tdc, &fTimeShifts, dch, fT0Time);
            if (fMwpcMapper) fMwpcMapper->FillEvent(hrb, mwpc);
            if (fTof400Mapper) fTof400Mapper->FillEvent(tdc, &fTimeShifts, tof400);
            if (fTof700Mapper && fT0Time != 0. && fT0Width != -1.) fTof700Mapper->fillEvent(tdc, &fTimeShifts, fT0Time, fT0Width, tof700);
            if (fZDCMapper) fZDCMapper->fillEvent(adc, zdc);
            if (fECALMapper) fECALMapper->fillEvent(adc, ecal);
            if (fLANDMapper) fLANDMapper->fillEvent(tacquila, land);
        }

        fDigiTree->Fill();
        prevEventType = curEventType;
    }

    if (fTof700Mapper) {
        fTof700Mapper->WriteSlewingResults();
        fDigiFileOut->cd();
    }
    fRunEndTime = TTimeStamp(time_t(fTime_s), fTime_ns);
    DigiRunHeader * runHeader = new DigiRunHeader(fPeriodId, fRunId, fRunStartTime, fRunEndTime);
    fDigiFileOut->WriteObject(runHeader, fDigiRunHdrName.Data());

    printf(ANSI_COLOR_RED "\n=============== RUN" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BLUE " %04d " ANSI_COLOR_RESET, runId);
    printf(ANSI_COLOR_RED "SUMMARY ===============\n" ANSI_COLOR_RESET);
    printf("START (event 1):\t%s\n", fRunStartTime.AsString());
    printf("FINISH (event %d):\t%s\n", fNevents, fRunEndTime.AsString());
    printf(ANSI_COLOR_RED "================================================\n" ANSI_COLOR_RESET);


    fDigiTree->Write();
    DisposeDecoder();
    fDigiFileOut->Write();
    fDigiFileOut->Close();
    fRootFileIn->Close();

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::InitDecoder() {

    //    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    fDigiTree = new TTree(
            conf.get<string>("Decoder.DigiTreeName").c_str(),
            conf.get<string>("Decoder.DigiTreeTitle").c_str());

    eventHeader = new BmnEventHeader();
    fDigiTree->Branch("BmnEventHeader.", &eventHeader);
    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    if (fDetectorSetup[0]) {
        fTrigMapper = new BmnTrigRaw2Digit(fTrigPlaceMapFileName, fTrigChannelMapFileName, fDigiTree);
        if (fT0Map == NULL) {
            BmnTrigChannelData tm = fTrigMapper->GetT0Map();
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
        fSiliconMapper = new BmnSiliconRaw2Digit(fPeriodId, fRunId, fSiliconSerials, fBmnSetup, GetAdcDecoMode());
    }

    if (fDetectorSetup[3] || fDetectorSetup[10] && GetAdcDecoMode() == kBMNADCMK) {
        gem = new TClonesArray("BmnGemStripDigit");
        fDigiTree->Branch("GEM", &gem);
        fGemMapper = new BmnGemRaw2Digit(fPeriodId, fRunId, fGemSerials, fGemMapFileName, fBmnSetup, GetAdcDecoMode());
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
        if (fTOF700ReferenceRun <= 0) {
            UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("TOF2", "slewing_file_id", fPeriodId, fRunId); //(detector_name, parameter_name, period_number, run_number)
            if (pDetectorParameter != NULL) {
                fTOF700ReferenceRun = pDetectorParameter->GetInt();
            } else {
                printf("Not found slewing run ID for run %d in DB\n", fRunId);
            }
        }
        tof700 = new TClonesArray("BmnTof2Digit");
        fDigiTree->Branch("TOF700", &tof700);
        fTof700Mapper = new BmnTof2Raw2DigitNew(fTof700MapFileName, fRootFileName, fTOF700ReferenceRun, fTOF700ReferenceChamber, fTof700GeomFileName);
        //        fTof700Mapper->print();
        for (int i = 0; i < 60; i++) {
            if (type_tof700_slewing[i]) {
                fTof700Mapper->SetSlewingReference(i + 1, refrun_tof700_slewing[i], refchamber_tof700_slewing[i]);
            }
        }
        //        fTof700Mapper->readSlewingT0();
        //        fTof700Mapper->readSlewingLimits();
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
        //fECALMapper->print();
    }

    if (fDetectorSetup[9]) {
        land = new TClonesArray("BmnLANDDigit");
        fDigiTree->Branch("LAND", &land);
        fLANDMapper = new BmnLANDRaw2Digit(fLANDMapFileName,
                fLANDClockFileName, fLANDTCalFileName, fLANDDiffSyncFileName,
                fLANDVScintFileName);
    }

    if (fDetectorSetup[3] && GetAdcDecoMode() == kBMNADCMK && GetPeriodId() > 6 || fDetectorSetup[10]) {
        csc = new TClonesArray("BmnCSCDigit");
        fDigiTree->Branch("CSC", &csc);
        if (GetAdcDecoMode() == kBMNADCSM)
            fCscMapper = new BmnCscRaw2Digit(fPeriodId, fRunId, fCscSerials);
    }

    fPedEvCntr = 0; // counter for pedestal events between two spills
    fPedEnough = kFALSE;
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ClearArrays() {
    if (dch) dch->Delete();
    if (mwpc) mwpc->Delete();
    if (gem) gem->Delete();
    if (csc) csc->Delete();
    if (silicon) silicon->Delete();
    if (tof400) tof400->Delete();
    if (tof700) tof700->Delete();
    if (zdc) zdc->Delete();
    if (ecal) ecal->Delete();
    if (land) land->Delete();
    if (fTrigMapper)
        fTrigMapper->ClearArrays();
    fTimeShifts.clear();
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::DecodeDataToDigiIterate() {
    ClearArrays();
    //            Int_t iEv = fRawTree->GetEntries();
    //            fRawTree->GetEntry(iEv);

    BmnEventHeader* headDAQ = eventHeaderDAQ;
    fCurEventType = headDAQ->GetEventType();
    fEventId = headDAQ->GetEventId();
    FillTimeShiftsMap();

    if (fTrigMapper) {
        fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
        fTrigMapper->FillEvent(tdc);
    }
    fT0Time = 0.;
    GetT0Info(fT0Time, fT0Width);
    if (fCurEventType == kBMNPEDESTAL) {
        if (fPedEvCntr == fEvForPedestals - 1) return kBMNERROR; //FIX return!
        CopyDataToPedMap(adc32, adc128, fPedEvCntr);
        fPedEvCntr++;
    } else { // payload
        if (fPrevEventType == kBMNPEDESTAL) {
            if (fPedEvCntr >= fEvForPedestals - 1) {
                if (fGemMapper)fGemMapper->RecalculatePedestals();
                if (fSiliconMapper)fSiliconMapper->RecalculatePedestals();
                if (fCscMapper)fCscMapper->RecalculatePedestals();
                fPedEvCntr = 0;
                fPedEnough = kTRUE;
            }
        }
        if ((fGemMapper) && (fPedEnough)) fGemMapper->FillEvent(adc32, gem);
        if ((fCscMapper) && (fPedEnough)) fCscMapper->FillEvent(adc32, csc);
        if ((fSiliconMapper) && (fPedEnough)) fSiliconMapper->FillEvent(adc128, silicon);
        if (fDchMapper) fDchMapper->FillEvent(tdc, &fTimeShifts, dch, fT0Time);
        if (fMwpcMapper) fMwpcMapper->FillEvent(hrb, mwpc);
        if (fTof400Mapper) fTof400Mapper->FillEvent(tdc, &fTimeShifts, tof400);
        if (fTof700Mapper && fT0Time != 0. && fT0Width != -1.) fTof700Mapper->fillEvent(tdc, &fTimeShifts, fT0Time, fT0Width, tof700);
        if (fZDCMapper) fZDCMapper->fillEvent(adc, zdc);
        if (fECALMapper) fECALMapper->fillEvent(adc, ecal);
        if (fLANDMapper) fLANDMapper->fillEvent(tacquila, land);
    }
    //    new((*eventHeader)[eventHeader->GetEntriesFast()]) BmnEventHeader(headDAQ->GetRunId(), headDAQ->GetEventId(),
    //            TTimeStamp(time_t(fTime_s), fTime_ns), fCurEventType, kFALSE, headDAQ->GetTrigInfo());
    eventHeader->SetRunId(headDAQ->GetRunId());
    eventHeader->SetEventId(headDAQ->GetEventId());
    eventHeader->SetPeriodId(headDAQ->GetPeriodId());
    eventHeader->SetEventTimeTS(TTimeStamp(time_t(fTime_s), fTime_ns));
    eventHeader->SetEventTime(TTimeStamp(time_t(fTime_s), fTime_ns).AsDouble());
    eventHeader->SetEventType(fCurEventType);
    eventHeader->SetTripWord(kFALSE);
    eventHeader->SetTrigInfo(headDAQ->GetTrigInfo());
    eventHeader->SetTimeShift(fTimeShifts);
    eventHeader->SetStartSignalInfo(fT0Time, fT0Width);
    eventHeader->SetSpillStart(headDAQ->GetSpillStart());
    //        fDigiTree->Fill();
    fPrevEventType = fCurEventType;

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FinishRun() {
    Double_t fSize = Double_t(fLengthRawFile / 1024. / 1024.);
    Int_t nEv = fNevents;

    //    if (!UniDbRun::GetRun(fPeriodId, fRunId))
    //        UniDbRun::CreateRun(fPeriodId, fRunId, fRawFileName, "", NULL, NULL, fRunStartTime, &fRunEndTime, &nEv, NULL, &fSize, NULL);

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
    if (csc) delete csc;
    if (dch) delete dch;
    if (mwpc) delete mwpc;
    if (silicon) delete silicon;
    if (tof400) delete tof400;
    if (tof700) delete tof700;
    if (zdc) delete zdc;
    if (ecal) delete ecal;
    if (land) delete land;

    delete eventHeader;
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
            fTime_s = syncDig->GetTime_sec();
            fTime_ns = syncDig->GetTime_ns();
            if (fEventId == 1) {
                fRunStartTime = TTimeStamp(time_t(fTime_s), fTime_ns);
            }
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
            //            printf("GEM ser 0x%08X, ev %d\n", adcDig->GetSerial(), ev);

            for (Int_t iSer = 0; iSer < fNGemSerials; ++iSer) {
                if (adcDig->GetSerial() != fGemSerials[iSer]) continue;
                //                printf("GEM ser = 0x%08x, iSer = %02d, ev= %05d, ch = %d\n", adcDig->GetSerial(), iSer, ev, adcDig->GetChannel());
                for (UInt_t iSmpl = 0; iSmpl < adcDig->GetNSamples(); ++iSmpl) {
                    if (fRunId > GetBoundaryRun(ADC32_N_SAMPLES)) {
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetShortValue())[iSmpl] / 16.0;
                    } else
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetUShortValue())[iSmpl] / 16.0;
                    //                printf("adc = %i pedData = %f\n", (adcDig->GetShortValue())[iSmpl], pedData[iSer][ev][adcDig->GetChannel()][iSmpl]);
                }
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
                //                printf("SIL ser = 0x%08x, iSer = %02d, ev= %05d, ch = %d\n", adcDig->GetSerial(), iSer, ev, adcDig->GetChannel());
                for (UInt_t iSmpl = 0; iSmpl < adcDig->GetNSamples(); ++iSmpl) {
                    if (fRunId > GetBoundaryRun(ADC128_N_SAMPLES))
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetShortValue())[iSmpl] / 16.0;
                    else
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetUShortValue())[iSmpl] / 16.0;
                    //                printf("adc = %i pedData = %f\n", (adcDig->GetShortValue())[iSmpl], pedData[iSer][ev][adcDig->GetChannel()][iSmpl]);
                }
                break;
            }
        }
    }
    if (fCscMapper) {
        Double_t**** pedData = fCscMapper->GetPedData();
        for (UInt_t iAdc = 0; iAdc < adcGem->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adcGem->At(iAdc);
            for (Int_t iSer = 0; iSer < fNCscSerials; ++iSer) {
                if (adcDig->GetSerial() != fCscSerials[iSer]) continue;
                for (UInt_t iSmpl = 0; iSmpl < adcDig->GetNSamples(); ++iSmpl)
                    pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetShortValue())[iSmpl] / 16;
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

void BmnRawDataDecoder::SetTof700SlewingReference(Int_t chamber, Int_t refrun, Int_t refchamber) {
    if (chamber <= 0 || chamber > 60) {
        printf("Wrong slewing chamber number %d\n", chamber);
        return;
    }
    if (refchamber <= 0 || refchamber > 60) {
        printf("Wrong slewing reference chamber number %d\n", refchamber);
        return;
    }
    if (refrun < 0 || refrun > 9999) {
        printf("Wrong slewing reference run number %d\n", refrun);
        return;
    }
    refrun_tof700_slewing[chamber - 1] = refrun;
    refchamber_tof700_slewing[chamber - 1] = refchamber;
    type_tof700_slewing[chamber - 1] = 1;
    return;
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
    printf("Open GEM map file %s\n", fGemMapFileName.Data());
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
    name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fCscMapFileName;
    ifstream inFileCSC(name.Data());
    if (!inFileCSC.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 2; ++i) getline(inFileCSC, dummy); //comment line in input file

    while (!inFileCSC.eof()) {
        inFileCSC >> std::hex >> ser >> std::dec >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
        if (!inFileCSC.good()) break;
        seials.insert(ser);
    }
    for (auto s : seials) fCscSerials.push_back(s);
    fNCscSerials = fCscSerials.size();
    // @TODO REMOVE CSC addition to GEM serials!
    if (GetAdcDecoMode() == kBMNADCMK) {
        for (auto s : seials) fGemSerials.push_back(s);
        fNGemSerials = fGemSerials.size();
    }

    fZDCSerials.push_back(0x046f4083);
    fZDCSerials.push_back(0x046f4bb2);
    fNZDCSerials = fZDCSerials.size();

    seials.clear();
    name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fECALMapFileName;
    ifstream inFileECAL(name.Data());
    //    printf("ECal name = %s\n", name.Data());
    if (!inFileECAL.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 2; ++i) getline(inFileECAL, dummy); //comment line in input file

    while (!inFileECAL.eof()) {
        inFileECAL >> std::hex >> ser >> std::dec >> dummy >> dummy >> dummy >> dummy;
        if (!inFileECAL.good()) break;
        seials.insert(ser);
        //printf("ECAL serial: 0x%08x\n", ser);
    }
    for (auto s : seials) fECALSerials.push_back(s);
    fNECALSerials = fECALSerials.size();
    printf("ECal name = %s, Nboards = %d, \n", name.Data(), fNECALSerials);

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
    BmnTrigDigit* dig = 0;
    for (auto ar : *trigArr) {
        if (fPeriodId > 6) {
            if (strcmp(ar->GetName(), "BC2")) continue;
        } else {
            if (strcmp(ar->GetName(), "T0")) continue;
        }
        for (int i = 0; i < ar->GetEntriesFast(); i++) {
            dig = (BmnTrigDigit*) ar->At(i);
            if (fPeriodId > 6) {
                if (dig->GetMod() == 0) {
                    t0time = dig->GetTime();
                    t0width = dig->GetAmp();
                    //		printf(" t0 %f t0w %f n %d\n", t0time, t0width, ar->GetEntriesFast());
                    return kBMNSUCCESS;
                }
            } else {
                t0time = dig->GetTime();
                t0width = dig->GetAmp();
                return kBMNSUCCESS;
            }
        }
    }
    return kBMNERROR;
}

BmnStatus BmnRawDataDecoder::InitUTCShift() {
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1972, 1, 1, 0, 0, 9), 10));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1972, 7, 1, 0, 0, 10), 11));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1973, 1, 1, 0, 0, 11), 12));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1974, 1, 1, 0, 0, 12), 13));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1975, 1, 1, 0, 0, 13), 14));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1976, 1, 1, 0, 0, 14), 15));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1977, 1, 1, 0, 0, 15), 16));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1978, 1, 1, 0, 0, 16), 17));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1979, 1, 1, 0, 0, 17), 18));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1980, 1, 1, 0, 0, 18), 19));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1981, 7, 1, 0, 0, 19), 20));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1982, 7, 1, 0, 0, 20), 21));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1983, 7, 1, 0, 0, 21), 22));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1985, 7, 1, 0, 0, 22), 23));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1988, 1, 1, 0, 0, 23), 24));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1990, 1, 1, 0, 0, 24), 25));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1991, 1, 1, 0, 0, 25), 26));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1992, 7, 1, 0, 0, 26), 27));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1993, 7, 1, 0, 0, 27), 28));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1994, 7, 1, 0, 0, 28), 29));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1996, 1, 1, 0, 0, 29), 30));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1997, 7, 1, 0, 0, 30), 31));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(1999, 1, 1, 0, 0, 31), 32));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2006, 1, 1, 0, 0, 32), 33));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2009, 1, 1, 0, 0, 33), 34));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2012, 7, 1, 0, 0, 34), 35));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2015, 7, 1, 0, 0, 35), 36));
    leaps.insert(pair<TTimeStamp, Int_t>(TTimeStamp(2017, 1, 1, 0, 0, 36), 37));
    utc_valid = TTimeStamp(2019, 6, 28, 0, 0, 36);
    return kBMNSUCCESS;
}

Int_t BmnRawDataDecoder::GetUTCShift(TTimeStamp t) {
    if (t > utc_valid)
        printf(ANSI_COLOR_RED "Warning! Leap seconds table expired!\n" ANSI_COLOR_RESET);
    Int_t shift = 0;
    auto it = leaps.lower_bound(t);
    if ((it == leaps.end()))
        it--;
    else
        if (it->first > t)
        it--;
    shift = it->second;
    return shift;
}
