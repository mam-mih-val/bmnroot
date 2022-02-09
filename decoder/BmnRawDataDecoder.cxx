
#include "BmnRawDataDecoder.h"
#include "BmnTDCDigit.h"
#include "BmnHRBDigit.h"
#include "BmnADCDigit.h"
#include "BmnTacquilaDigit.h"
#include "BmnTQDCADCDigit.h"
#include "BmnLANDDigit.h"
#include "BmnTofCalDigit.h"
#include "BmnSyncDigit.h"
#include "BmnGemStripDigit.h"
#include "BmnMSCDigit.h"
#include "DigiRunHeader.h"
#include "UniDbDetectorParameter.h"
#include "UniDbRun.h"
#include "TangoData.h"
#include "RawTypes.h"

#include "TSystem.h"
#include "TStopwatch.h"

#include <bitset>
#include <iostream>

#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */

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
    spillHeader = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fT0Time = 0.0;
    fRawTree = NULL;
    fRawTreeSpills = NULL;
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
    tacquila2 = NULL;
    msc = NULL;
    dch = NULL;
    tof400 = NULL;
    tof700 = NULL;
    zdc = NULL;
    scwall = NULL;
    fhcal = NULL;
    hodo = NULL;
    ecal = NULL;
    gem = NULL;
    silicon = NULL;
    land = NULL;
    tofcal = NULL;
    mwpc = NULL;
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
    fScWallCalibrationFileName = "";
    fScWallMapFileName = "";
    fFHCalCalibrationFileName = "";
    fFHCalMapFileName = "";
    fHodoCalibrationFileName = "";
    fHodoMapFileName = "";
    fECALCalibrationFileName = "";
    fECALMapFileName = "";
    fLANDMapFileName = "";
    fLANDClockFileName = "";
    fLANDTCalFileName = "";
    fLANDDiffSyncFileName = "";
    fLANDVScintFileName = "";
    fTofCalMapFileName = "";
    fTofCalClockFileName = "";
    fTofCalTCalFileName = "";
    fTofCalDiffSyncFileName = "";
    fTofCalVScintFileName = "";
    fDigiRunHdrName = "DigiRunHeader";
    fDat = 0;
    fGemMapper = NULL;
    fSiliconMapper = NULL;
    fMwpcMapper = NULL;
    fCscMapper = NULL;
    fDchMapper = NULL;
    fTrigMapper = NULL;
    fTof400Mapper = NULL;
    fTof700Mapper = NULL;
    fZDCMapper = NULL;
    fScWallMapper = NULL;
    fFHCalMapper = NULL;
    fHodoMapper = NULL;
    fECALMapper = NULL;
    fLANDMapper = NULL;
    fTofCalMapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fPedoCounter = 0;
    fGemMap = NULL;
    fEvForPedestals = N_EV_FOR_PEDESTALS;
    fBmnSetup = kBMNSETUP;
    fT0Serial = 0;
    tai_utc_dif = 0;
    fVerbose = 0;
    isSpillStart = kFALSE;
    fSpillCntr = 0;
    fMSCMapper = nullptr;
    InitUTCShift();
    fNECALSerials = 0;
    fNZDCSerials = 0;
    fNScWallSerials = 0;
    fNFHCalSerials = 0;
    fNHodoSerials = 0;
    //InitMaps();
}

BmnRawDataDecoder::~BmnRawDataDecoder() {
}

BmnStatus BmnRawDataDecoder::ParseRunTLV(UInt_t *d, UInt_t &len) {
    uint16_t iWord = 0;
    while (iWord < len) {
        UInt_t word = d[iWord++];
        UInt_t idLen = 0;
        switch (word) {
            case SYNC_RUN_NUMBER:
                idLen = d[iWord++] / kNBYTESINWORD;
                if (idLen != 1) {
                    printf("Wrong RunId length %u\n", idLen);
                    return kBMNERROR;
                }
                fRunId = d[iWord];
                printf("Run Id %u\n", fRunId);
                break;
            case SYNC_RUN_INDEX:
            {
                idLen = d[iWord++] / kNBYTESINWORD;
                if (idLen + iWord > len) {
                    printf("Wrong RunIndex length %u\n", idLen);
                    return kBMNERROR;
                }
                TString runIndex(reinterpret_cast<const char *> (d + iWord), idLen * kNBYTESINWORD);
                printf("Run index %s\n", runIndex.Data());
                break;
            }
            default:
                printf("Unknown sync %08X\n", word);
                break;
        }
        iWord += idLen;
    }
    return kBMNSUCCESS;
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
        //                                    printf( "word %08X\n", fDat);
        fCurentPositionRawFile = ftello64(fRawFileIn);
        if (fCurentPositionRawFile >= fLengthRawFile) break;
        switch (fDat) {
            case SYNC_EVENT:
            case SYNC_EVENT_OLD: //search for start of event
                //            printf(ANSI_COLOR_BLUE "kSYNC1\n" ANSI_COLOR_RESET);
                // read number of bytes in event
                if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) continue;
                fDat = fDat / kNBYTESINWORD + (fPeriodId <= 7 ? 1 : 0); // bytes --> words
                //                printf("ev length %d\n", fDat);
                if (fDat >= 100000) { // what the constant?
                    printf("Wrong data size: %d:  skip this event\n", fDat);
                    fread(data, kWORDSIZE, fDat, fRawFileIn);
                } else {
                    //read array of current event data and process them
                    if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) continue;
                    fEventId = data[0];
                    //                    printf(ANSI_COLOR_BLUE "iEv = %u\n" ANSI_COLOR_RESET, data[0]);
                    if (fEventId <= 0) continue; // skip bad events
                    ProcessEvent(data, fDat);
                    if (data[0] != (fNevents + 1)) // Just a check to see if somehow ProcessEvent messed up our counting
                        printf(ANSI_COLOR_RED "***Extreme warning, events are not synced: %i, %i***\n" ANSI_COLOR_RESET, fEventId, fNevents + 1);
                    fRawTree->Fill();
                    if (isSpillStart == kTRUE)
                        isSpillStart = kFALSE;
                    fNevents++;
                    nSpillEvents++;
                }
                break;
            case SYNC_EOS:
            case SYNC_EOS_OLD:
                // read number of bytes in event
                if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) continue;
                fDat = fDat / kNBYTESINWORD + (fPeriodId <= 7 ? 1 : 0); // bytes --> words
                //            printf("ev length %d\n", fDat);
                //read array of current event data and process them
                if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) continue;
                //                        printf(ANSI_COLOR_BLUE "EOS iEv = %u lastEv  = %u\n" ANSI_COLOR_RESET,
                //                                data[0], fEventId);
                ProcessEvent(data, fDat);
                if (msc->GetEntriesFast() > 0)
                    fRawTreeSpills->Fill();
                isSpillStart = kTRUE;
                nSpillEvents = 0;
                break;
            case SYNC_RUN_START:
                printf("RUN START\n");
            case SYNC_RUN_STOP:
                if (fDat == SYNC_RUN_STOP)
                    printf("RUN STOP\n");
                if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) continue;
                fDat = fDat / kNBYTESINWORD;
                if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) continue;
                ParseRunTLV(data, fDat);
                break;
            case SYNC_JSON:
                if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) continue;
                fDat = fDat / kNBYTESINWORD;
                printf("SYNC JSON len %u\n", fDat);
            default:
                printf("unrecognized sync %08X\n", fDat);
                break;

        }
    }
    fRunEndTime = TTimeStamp(time_t(fTime_s), fTime_ns);
    Int_t shift = GetUTCShift(fRunEndTime);
    if (shift != tai_utc_dif)
        fprintf(stderr, ANSI_COLOR_RED "Critical Warning! Leap second added during the %i run!\n\n" ANSI_COLOR_RESET, fRunId);
    //    fRunEndTime = TTimeStamp(time_t(fTime_s - shift), fTime_ns);
    //    fRawTree->Fill();

    fCurentPositionRawFile = ftello64(fRawFileIn);
    printf("Read %d events; %lld bytes (%.3f Mb)\n\n", fNevents, fCurentPositionRawFile, fCurentPositionRawFile / 1024. / 1024.);

    fRawTree->Write();
    fRawTreeSpills->Write();
    fRootFileOut->Close();
    fclose(fRawFileIn);

    delete sync;
    delete adc32;
    delete adc128;
    delete adc;
    delete tacquila;
    delete tacquila2;
    delete hrb;
    delete tdc;
    delete tqdc_tdc;
    delete tqdc_adc;
    delete msc;
    if (eventHeaderDAQ) delete eventHeaderDAQ;

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
    tacquila2 = new TClonesArray("BmnTacquilaDigit");
    tdc = new TClonesArray("BmnTDCDigit");
    tqdc_adc = new TClonesArray("BmnTQDCADCDigit");
    tqdc_tdc = new TClonesArray("BmnTDCDigit");
    hrb = new TClonesArray("BmnHRBDigit");
    eventHeaderDAQ = new BmnEventHeader();

    fRawTree->Branch("SYNC", &sync);
    fRawTree->Branch("ADC32", &adc32);
    fRawTree->Branch("ADC128", &adc128);
    fRawTree->Branch("ADC", &adc);
    fRawTree->Branch("Tacquila", &tacquila);
    fRawTree->Branch("Tacquila2", &tacquila2);
    fRawTree->Branch("TDC", &tdc);
    fRawTree->Branch("TQDC_ADC", &tqdc_adc);
    fRawTree->Branch("TQDC_TDC", &tqdc_tdc);
    fRawTree->Branch("HRB", &hrb);
    fRawTree->Branch("BmnEventHeader.", &eventHeaderDAQ);

    fRawTreeSpills = new TTree("BMN_RAW_SPILLS", "BMN_RAW_SPILLS");
    msc = new TClonesArray(BmnMSCDigit::Class());
    fRawTreeSpills->Branch("MSC", &msc);
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
    //                printf("EventID = %d\n", fEventId);
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
        if (fDat == SYNC_RUN_NUMBER) {
            printf("RunNumberSync\n");
            syncCounter++;
            if (syncCounter > 1) {
                cout << "Finish by SYNC" << endl;
                return kBMNFINISH;
            }
            fread(&fDat, kWORDSIZE, 1, fRawFileIn); //skip word
        }

        if (fDat == SYNC_EOS_OLD || fDat == SYNC_EOS) {
            // read number of bytes in event
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) continue;
            fDat = fDat / kNBYTESINWORD + (fPeriodId <= 7 ? 1 : 0); // bytes --> words
            //            printf("ev length %d\n", fDat);
            //read array of current event data and process them
            if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) continue;
            //            printf(ANSI_COLOR_BLUE "EOS iEv = %u lastEv  = %u\n" ANSI_COLOR_RESET,
            //            data[0], fEventId);
            ProcessEvent(data, fDat);
            if (msc->GetEntriesFast() > 0)
                fRawTreeSpills->Fill();
            isSpillStart = kTRUE;
            nSpillEvents = 0;
        }
        if (fDat == SYNC_EVENT_OLD || fDat == SYNC_EVENT) { //search for start of event
            //            printf(ANSI_COLOR_BLUE "kSYNC1\n" ANSI_COLOR_RESET);
            // read number of bytes in event
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) continue;
            fDat = fDat / kNBYTESINWORD + (fPeriodId <= 7 ? 1 : 0); // bytes --> words
            //                        printf("ev length %d\n", fDat);
            if (fDat >= 100000) { // what the constant?
                printf("Wrong data size: %d:  skip this event\n", fDat);
                fread(data, kWORDSIZE, fDat, fRawFileIn);
            } else {
                //read array of current event data and process them
                if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) continue;
                fEventId = data[0];
                //                printf(ANSI_COLOR_BLUE "iEv = %u\n" ANSI_COLOR_RESET, data[0]);
                if (fEventId <= 0) continue; // skip bad events
                BmnStatus convResult = ProcessEvent(data, fDat);
                if (data[0] != (fNevents + 1)) // Just a check to see if somehow ProcessEvent messed up our counting
                    printf(ANSI_COLOR_RED "***Extreme warning, events are not synced: %i, %i***\n" ANSI_COLOR_RESET, fEventId, fNevents + 1);
                //                fRawTree->Fill();
                if (isSpillStart == kTRUE)
                    isSpillStart = kFALSE;
                fNevents++;
                nSpillEvents++;
                if (convResult == kBMNSUCCESS)
                    return kBMNSUCCESS;
            }
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
    tacquila2->Delete();
    msc->Delete();
    if (fVerbose == 1) {
        if (fEventId % 5000 == 0) cout << "Converted event #" << fEventId << endl;
    } else if (fVerbose == 0)
        DrawBar(fCurentPositionRawFile, fLengthRawFile);

    Long64_t idx = 1;
    BmnEventType evType = kBMNPAYLOAD;

    while (idx < len) {
        Bool_t recognized = kTRUE;
        UInt_t serial = d[idx++];
        UInt_t id = (d[idx] >> 24);
        UInt_t payload = (d[idx++] & 0x7FFF) / kNBYTESINWORD;
        if (payload > 2000000) {
            printf("[WARNING] Event %d:\n serial = 0x%06X\n id = Ox%02X\n payload = %d\n", fEventId, serial, id, payload);
            break;
        }
        //        printf("iev %7d  idx %7lld   idev %02X serial 0x%08X payload %4u\n", fEventId, idx, id, serial, payload);
        switch (id) {
            case kTQDC16VS_E:
                //                printf("TQDC-E serial 0x%08X  words %u\n", serial, payload);
                FillTQDC_Eth(&d[idx], serial, payload);
                break;
            case kTDC72VXS:
                //                printf("TDC72VXS serial 0x%08X  words %u\n", serial, payload);
                FillTDC72VXS(&d[idx], serial, payload);
                break;
            case kADC64VE_XGE:
            case kADC64VE:
            {
                bitset<kWORDSIZE * 8> chMaskLo(d[idx + 3]);
                bitset<kWORDSIZE * 8> chMaskHi(d[idx + 4]);
                Int_t nCh = chMaskHi.count() + chMaskLo.count(); //64;
                payload -= 5;
                idx += 5;
                Double_t nSmpl = 2 * (payload / (Double_t) nCh - 3); // MStream ADC payload count
                //                                printf("payload %08d nCh %d nSmpl %f\n", payload, nCh, nSmpl);
                if (nSmpl > 100.0) // number means nothing but an arbitrary number between 128 and 32
                    Process_ADC64VE(&d[idx], payload, serial, 128, adc128);
                else
                    Process_ADC64VE(&d[idx], payload, serial, 32, adc32);
                break;
            }
            case kADC64WR:
            {
                auto isZDC = (std::find(fZDCSerials.begin(), fZDCSerials.end(), serial) != fZDCSerials.end());
                auto isECAL = (std::find(fECALSerials.begin(), fECALSerials.end(), serial) != fECALSerials.end());
                auto isSCWALL = (std::find(fScWallSerials.begin(), fScWallSerials.end(), serial) != fScWallSerials.end());
                auto isFHCAL = (std::find(fFHCalSerials.begin(), fFHCalSerials.end(), serial) != fFHCalSerials.end());

                if (isZDC || isECAL || isSCWALL || isFHCAL)
                    Process_ADC64WR(&d[idx], payload, serial, adc);

                break;
            }
            case kFVME:
            {
                BmnTrigInfo* trigInfo = eventHeaderDAQ->GetTrigInfo();
                Process_FVME(&d[idx], payload, serial, evType, trigInfo);
            }
                break;
            case kHRB:
                Process_HRB(&d[idx], payload, serial);
                break;
            //case kLANDDAQ:
            //    Process_Tacquila(&d[idx], payload);
            //    break;
            //case kTOFCALDAQ:
            //    Process_Tacquila(&d[idx], payload);
            //    break;
            case kTACQUILADAQ:
                Process_Tacquila(&d[idx], payload);
                break;
            case kUT24VE_TRC:
                FillUT24VE_TRC(&d[idx], payload, evType);
                break;
            default:
                //                printf("Device id %02X not recognized\n", id);
                recognized = kFALSE;
                break;
        }
        if (payload + idx > len) {
            printf("Error in the event #%d: device payload length mismatch!\n", fEventId);
            return kBMNERROR;
        } else {
            if (recognized)
                idx += payload;
            else
                idx--;
        }
    }
    eventHeaderDAQ->SetRunId(fRunId);
    eventHeaderDAQ->SetPeriodId(fPeriodId);
    eventHeaderDAQ->SetEventId(fEventId);
    eventHeaderDAQ->SetEventTimeTS(TTimeStamp(time_t(fTime_s), fTime_ns));
    eventHeaderDAQ->SetEventTime(TTimeStamp(time_t(fTime_s), fTime_ns).AsDouble());
    eventHeaderDAQ->SetEventType(evType);
    eventHeaderDAQ->SetTripWord(kFALSE);
    //eventHeaderDAQ->SetTrigInfo(trigInfo);
    eventHeaderDAQ->SetTimeShift(fTimeShifts);
    //    eventHeaderDAQ->SetStartSignalInfo(fT0Time, fT0Width);
    eventHeaderDAQ->SetSpillStart(isSpillStart);

    return kBMNSUCCESS;
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
    UInt_t iCh = 0;
    while (iCh < kNCH - 1 && i < len) {
        iCh = d[i] >> 24;
        if (iCh > 64) {
            printf("Wrong ADC channel! serial = 0x%X  iCh = %d  nSmpl = %d\n", serial, iCh, nSmpl);
            break;
        }
        i += 3; // skip two timestamp words (they contain TAI timestsamps)
        TClonesArray& ar_adc = *arr;

        if (fRunId > GetBoundaryRun(kNSTAMPS) || fPeriodId == 8) {
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
    //                                    printf("FVME serial %08X len %u\n",serial, len);
    UInt_t modId = 0;
    UInt_t slot = 0;
    UInt_t type = 0;
    for (UInt_t i = 0; i < len; i++) {
        type = d[i] >> 28;
        //    printf("     type %02X  i %u\n", type, i);
        switch (type) {
            case kEVHEADER:
                //                printf("Ev header \n"));
                break;
            case kEVTRAILER:
                //                                printf("Ev trailer\n");
                return kBMNSUCCESS;
                break;
            case kSTATUS:
            case kPADDING:
                break;
            case kMODHEADER:
                modId = (d[i] >> 16) & 0x7F;
                slot = (d[i] >> 23) & 0x1F;
                //                                printf("\tmodid 0x%02X slot %d serial 0x%08X\n", modId, slot, serial);
                break;
            case kMODTRAILER:
                //                                printf("\tmodule trailer\n");
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
                    case kTQDC16:
                    case kTQDC16VS:
                        FillTQDC(d, serial, slot, modId, i);
                        break;
                    case kMSC:
                        FillMSC(d, serial, slot, i);
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
    //p32 += 0;

	//Make initial check how many scaler words to skip
	//for ToF-Cal vs. LAND
	uint32_t scaler_header = ntohl(*p32);
	unsigned chain = 0;
	if(scaler_header == 0x5){ //ToF-Cal
		p32 += 7;
	}else if(scaler_header == 0x4){ //LAND
		p32 += 6;
		//might change ... veto not connected yet
		//thus gives still error messages
		chain = 0;
	}else{
	cerr << __FILE__ << ':' << __LINE__ << ": Wrong NIM scalers " << scaler_header << ".\n";	
		return kBMNFINISH;
	}

    /*
     * Tacquila data!
     * We have 2 chains of 10 Tacquila cards each.
     */
    //for (unsigned chain = 0; chain < 2; ++chain) {
    for (chain; chain < 2; ++chain) {
        uint32_t header = ntohl(*p32++);
	//JK
	if(header == 0x5a5a5a5a) header = ntohl(*p32++);
#define TACQUILA_PRINT_HEADER << "(header=" << header << ")" <<
	//std::cout << std::hex << header << std::endl;
	//std::cout << "#############" << std::endl;
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
        if ((3 != sam) && (5 != sam)) { //ToF-Cal or LAND
            cerr << __FILE__ << ':' << __LINE__ << ": SAM=" << sam << "!=3 or 5"
                    "forbidden " TACQUILA_PRINT_HEADER ".\n";
            return kBMNFINISH;
        }
        unsigned tac, clock;
        for (unsigned i = 0; i < count; ++i) {
            uint32_t u32 = ntohl(*p32++);
	    //JK
	    if(u32 == 0x5a5a5a5a) ntohl(*p32++);
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
		    if(sam == 3){
			    TClonesArray &ar_tacquila = *tacquila;
			    new(ar_tacquila[tacquila->GetEntriesFast()])
				    BmnTacquilaDigit(sam, gtb, module - 1, channel, tac, clock, qdc);
		    }else if(sam == 5){
			    TClonesArray &ar_tacquila2 = *tacquila2;
			    new(ar_tacquila2[tacquila2->GetEntriesFast()])
				    BmnTacquilaDigit(sam, gtb, module - 1, channel, tac, clock, qdc);
		    }else{
			    cerr << __FILE__ << ':' << __LINE__ << ": Wrong SAM for TClonesArray.\n";	
		    }
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillU40VE(UInt_t *d, BmnEventType &evType, UInt_t slot, UInt_t & idx, BmnTrigInfo* trigInfo) {
    UInt_t type = d[idx] >> 28;
    //                printf("\t\tU40VE start type %u slot %u idx %u\n", type, slot, idx);
    Bool_t countersDone = kFALSE;
    while (type == kWORDTAI || type == kWORDTRIG || type == kWORDAUX) {
        if (fPeriodId > 4 && type == kWORDTRIG && slot == kEVENTTYPESLOT) {
            evType = ((d[idx] & BIT(3)) >> 3) ? kBMNPEDESTAL : kBMNPAYLOAD;
            UInt_t trigSrc = ((d[idx] >> 16) & (BIT(8) - 1));
            //            printf("EvId %6u trig source %u  evType %d\n", fEventId, trigSrc, evType);
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
            //            printf("EvId %6u cand %7u, acc %5u, bef %5u, after %5u, rjct %7u, all %8u, avail %8u\n",
            //                    fEventId,
            //                    trigInfo->GetTrigCand(),
            //                    trigInfo->GetTrigAccepted(),
            //                    trigInfo->GetTrigBefo(),
            //                    trigInfo->GetTrigAfter(),
            //                    trigInfo->GetTrigRjct(),
            //                    trigInfo->GetTrigAll(),
            //                    trigInfo->GetTrigAvail());
            countersDone = kTRUE;
            //            if (trigInfoTemp)
            //                delete trigInfoTemp;
            //            trigInfoTemp = new BmnTrigInfo(trigInfo);
        }
        idx++; //go to the next DATA-word
        type = d[idx] >> 28;
        //                    printf("\t            type %u slot %u idx %u\n", type, slot, idx);
    }

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t & idx) {
    UInt_t type = d[idx] >> 28;
    while (type != kMODTRAILER) { //data will be finished when module trailer appears
        if (type == TDC_ERROR) {
            if (fVerbose == 0) {
                fprintf(stderr, ANSI_COLOR_RED "Warning: TDC (modID 0x%02X serial 0x%08X slot %d tdcID %d) error flags: 0x%04X\n" ANSI_COLOR_RESET,
                        modId, serial, slot, ((d[idx] >> 24) & 0xF), (d[idx] & ((1 << 15) - 1)));
                if ((d[idx] & BIT(12)) || (d[idx] & BIT(13))) {
                    fprintf(stderr, ANSI_COLOR_RED "Warning: Critical TDC error thrown\n" ANSI_COLOR_RESET);
                    return kBMNERROR;
                }
            }
        }
        if (type == TDC_LEADING || type == TDC_TRAILING) {
            UInt_t tdcId = (d[idx] >> 24) & 0xF;
            UInt_t time = (modId == kTDC64V) ? (d[idx] & 0x7FFFF) : ((d[idx] & 0x7FFFF) << 2) | ((d[idx] & 0x180000) >> 19);
            UInt_t channel = (modId == kTDC64V) ? (d[idx] >> 19) & 0x1F : (d[idx] >> 21) & 0x7;
            //if (modId == kTDC64V && tdcId == 2) channel += 32;
            TClonesArray &ar_tdc = *tdc;
            new(ar_tdc[tdc->GetEntriesFast()]) BmnTDCDigit(serial, modId, slot, (type == TDC_LEADING), channel, tdcId, time);
            //            printf("tdc  %08X : %d channel %d\n", serial, slot, channel);
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
        if (type == TDC_ERROR) {
            if (fVerbose == 0) {
                fprintf(stderr, ANSI_COLOR_RED "ERROR: TDC (serial 0x%08X slot %d tdcID %d) error flags: 0x%04X\n" ANSI_COLOR_RESET,
                        serial, slot, ((d[idx] >> 24) & ((1 << 4) - 1)), (d[idx] & ((1 << 15) - 1)));
                if ((d[idx] & BIT(12)) || (d[idx] & BIT(13))) {
                    fprintf(stderr, ANSI_COLOR_RED "ERROR: Critical TQDC error thrown\n" ANSI_COLOR_RESET);
                    return kBMNERROR;
                }
            }
        }
        UInt_t mode = (d[idx] >> 26) & 0x3;
        if (!inADC) { //       printf("type %d mode %d word %0X\n", type, mode, d[idx]);
            if ((mode == 0) && (type == TDC_LEADING || type == TDC_TRAILING)) { // TDC time
                channel = (d[idx] >> 19) & 0x1F;
                UInt_t time = ((d[idx] & 0x7FFFF) << 2) | (d[idx] >> 24) & 0x3; // in 25 ps
                //               printf("TDC time %d channel %d\n", time, channel);
                new((*tqdc_tdc)[tqdc_tdc->GetEntriesFast()]) BmnTDCDigit(serial, modId, slot, (type == TDC_LEADING), channel, 0, time, tdcTimestamp);
                //                printf("tqdc tdc %08X : %d channel %d\n", serial, slot, channel);
            } else if ((type == 4) && (mode != 0)) { // Trig | ADC Timestamp
                channel = (d[idx] >> 19) & 0x1F;
                if (d[idx] & BIT(16)) { // ADC TS
                    adcTimestamp = d[idx] & 0xFFFF;
                    inADC = kTRUE;
                } else {// Trig TS
                    trigTimestamp = d[idx] & 0xFFFF;
                }
            } else if (type == TDC_EV_HEADER) {
                tdcTimestamp = d[idx] & 0xFFF;
                // UInt_t iEv = (d[idx] >> 12) & 0xFFF;
                //                printf("TDC ev header: %d\n", iEv);
            } else if (type == TDC_EV_TRAILER) {
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
                //                printf("tqdc adc %08X : %d channel %d\n", serial, slot, channel);
            }
        }
        type = d[++idx] >> 28;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillBlockADC(UInt_t *d, UInt_t serial, uint8_t channel, uint16_t &len, TClonesArray *ar) {
    int16_t valI[ADC_SAMPLING_LIMIT];
    const uint8_t NBytesInSample = 2;
    uint16_t iWord = 0;
    while (iWord < len) {
        uint16_t adcTS = d[iWord] & (BIT(16) - 1);
        uint16_t SigLen = d[iWord] >> 16;
        uint16_t NSamples = SigLen / NBytesInSample;
        uint16_t NSampleWords = SigLen / kNBYTESINWORD + ((SigLen % kNBYTESINWORD) ? 1 : 0);
        //        printf("adc len %2u ts %3u NSampleWords %u\n", NSamples, adcTS, NSampleWords);
        if (iWord + NSampleWords + 1 > len) {
            printf("Error! TQDC ADC wrong payload length! iWord %u SigLen %u len %u\n",
                    iWord, SigLen, len);
            return kBMNERROR;
        }
        uint16_t iSampleWord = 0;
        while (iSampleWord++ < NSampleWords) {
            int16_t adcLo = static_cast<int16_t> (d[iWord + iSampleWord] & (BIT(16) - 1));
            int16_t adcHi = static_cast<int16_t> (d[iWord + iSampleWord] >> 16);
            //            printf("\tadcHi %4d  adcLow %4d\n", adcHi, adcLo);
            valI[iSampleWord * 2 - 2] = adcLo;
            valI[iSampleWord * 2 - 1] = adcHi;
        }
        // no slot id for ethernet
        if (NSamples)
            new((*ar)[ar->GetEntriesFast()]) BmnTQDCADCDigit(serial, channel, 0, NSamples, valI, 0, adcTS);
        iWord += iSampleWord;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillBlockTDC(UInt_t *d, UInt_t serial, uint16_t &len, TClonesArray *ar) {
    uint16_t tdcTS = 0;
    uint8_t tdcId = 0;
    uint16_t tdcLine = 0;
    while (tdcLine < len) {
        UInt_t word = d[tdcLine];
        UInt_t bt = word >> 28;
        switch (bt) {
            case TDC_EV_HEADER:
            {
                uint16_t evId = (word >> 12) & (BIT(12) - 1);
                tdcId = (word >> 24) & (BIT(4) - 1);
                tdcTS = word & (BIT(12) - 1);
                //                printf("\tTDC header  ev %u TS %u\n", evId, tdcTS);
                break;
            }
            case TDC_EV_TRAILER:
            {
                uint16_t evId = (word >> 12) & (BIT(12) - 1);
                uint16_t tdcWC = word & (BIT(12) - 1);
                //                printf("\tTDC trailer ev %u WC %u\n", evId, tdcWC);
                break;
            }
            case TDC_LEADING:
            case TDC_TRAILING:
            {
                uint8_t channel = (word >> 21)&(BIT(7) - 1);
                //                uint16_t time = (word>>2) & (BIT(19) - 1);
                uint16_t time = word & (BIT(21) - 1);
                //                printf("\tTDC %s ch %u id %d time %u\n", (bt == TDC_LEADING) ? "leading" : "trailing", channel, tdcId, time);
                new((*ar)[ar->GetEntriesFast()]) BmnTDCDigit(serial, 0, 0, (bt == TDC_LEADING), channel, 0/*tdcId*/, time, 0/*tdcTS*/); // ignore idcId in TQDC
                break;
            }
            case TDC_ERROR:
                if (fVerbose == 0) {
                    fprintf(stderr, ANSI_COLOR_RED "ERROR: TDC (serial 0x%08X tdcID %d) error flags: 0x%04X\n" ANSI_COLOR_RESET,
                            serial, ((word >> 24) & ((1 << 4) - 1)), (word & ((1 << 15) - 1)));
                    if ((word & BIT(12)) || (word & BIT(13))) {
                        fprintf(stderr, ANSI_COLOR_RED "ERROR: Critical TQDC error thrown\n" ANSI_COLOR_RESET);
                    }
                }
                break;
        }
        tdcLine++;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTQDC_Eth(UInt_t *d, UInt_t serial, UInt_t &len) {
    UInt_t index = 0;
    MStreamHeader ms = {};
    memcpy(&ms, d, sizeof (ms));
    index += sizeof (ms) / kNBYTESINWORD;
    //    ms.Print();
    MStreamSubtype0Header ms0 = {};
    memcpy(&ms0, d + index, sizeof (ms0));
    index += sizeof (ms0) / kNBYTESINWORD;
    FillWR(serial, fEventId, ms0.TaiSec, ms0.TaiNSec);
    //    printf("len %u msHeader len %u\n", len, ms.Len / kNBYTESINWORD);
    //    printf("taiFlags %u TAI %s\n",
    //            ms0.TaiFlags, TTimeStamp(time_t(ms0.TaiSec), ms0.TaiNSec).AsString());
    while (index < ms.Len / kNBYTESINWORD) {
        //        TqdcDataHeader th = *reinterpret_cast<TqdcDataHeader*> (d + index);
        TqdcDataHeader th = {};
        memcpy(&th, d + index, sizeof (th));
        index += sizeof (th) / kNBYTESINWORD;
        //        printf("TQDC DataType %u channel %2u adcBits %u len %4u\n", th.DataType, th.Chan, th.AdcBits, th.Len);
        uint16_t blockLen = th.Len / kNBYTESINWORD;
        switch (th.DataType) {
            case 0: // TDC
                FillBlockTDC(d + index, serial, blockLen, tqdc_tdc);
                break;
            case 1: // ADC
                FillBlockADC(d + index, serial, th.Chan, blockLen, tqdc_adc);
                break;
            default:
                printf("Wrong TQDC data type %u !\n", th.DataType);
                break;
        }
        index += blockLen;

    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTDC72VXS(UInt_t *d, UInt_t serial, UInt_t &len) {
    UInt_t index = 0;
    MStreamHeader ms = {};
    memcpy(&ms, d, sizeof (ms));
    index += sizeof (ms) / kNBYTESINWORD;
    MStreamSubtype0Header ms0 = {};
    memcpy(&ms0, d + index, sizeof (ms0));
    index += sizeof (ms0) / kNBYTESINWORD;
    FillWR(serial, fEventId, ms0.TaiSec, ms0.TaiNSec);
    //    printf("\t index %u len %u inner len %u\n", index, len, (ms.Len / kNBYTESINWORD));
    while (index < ms.Len / kNBYTESINWORD) {
        uint8_t dtype = d[index] >> 28;
        bool overflow = d[index] & BIT(16);
        uint16_t blockLen = (d[index] & (BIT(16) - 1)) / kNBYTESINWORD;
        if (!overflow)
            switch (dtype) {
                case 0: // TDC
                    //                    printf("TDC at index %4u  len %4u\n", index, blockLen);
                    FillBlockTDC(d + index, serial, blockLen, tdc);
                    break;
                case 0xF: // Stat
                    break;
                default:
                    printf("Wrong VXS data type %u !\n", dtype);
                    break;
            }
        index += blockLen;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillUT24VE_TRC(UInt_t *d, UInt_t &len, BmnEventType &evType) {
    UInt_t index = 0;
    MStreamHeader ms = {};
    memcpy(&ms, d, sizeof (ms));
    index += sizeof (ms) / kNBYTESINWORD;
    MStreamSubtype0Header ms0 = {};
    memcpy(&ms0, d + index, sizeof (ms0));
    index += sizeof (ms0) / kNBYTESINWORD;
    if (ms.Len / kNBYTESINWORD > len)
        printf("UT24VE-TRC Error! MSHeader payload length larger than from device header!\n");
    evType = (d[index] & BIT(16)) ? kBMNPEDESTAL : kBMNPAYLOAD;
    bool randomTrigger = d[index] & BIT(17);
    bool periodicTrigger = d[index] & BIT(18);
    bool externalTTL = d[index] & BIT(19);
    //    printf("evType %s taiFlags %u TAI %s \n",
    //            evType == kBMNPEDESTAL ? "pedestal" : "payload", 
    //            ms0.TaiFlags, TTimeStamp(time_t(ms0.TaiSec), ms0.TaiNSec).AsString());
    return kBMNSUCCESS;
}

void BmnRawDataDecoder::FillWR(UInt_t serial, Long64_t iEvent, Long64_t t_sec, Long64_t t_ns) {
    if (tai_utc_dif == 0)
        tai_utc_dif = GetUTCShift(TTimeStamp(time_t(t_sec), t_ns));
    fTime_ns = t_ns;
    fTime_s = t_sec - tai_utc_dif;
    new((*sync)[sync->GetEntriesFast()]) BmnSyncDigit(serial, iEvent, fTime_s, fTime_ns);
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
    FillWR(serial, GlobalEvent, ts_t0_s, ts_t0_ns);

    idx += 3; //skip next 3 words (we've processed them)
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillMSC(UInt_t* d, UInt_t serial, UInt_t slot, UInt_t& idx) {
    const UInt_t nCnt = 16;
    UInt_t type = d[idx] >> 28;
    UInt_t iCnt = 0;
    BmnMSCDigit *dig = new((*msc)[msc->GetEntriesFast()]) BmnMSCDigit(serial, slot, fEventId);
    UInt_t *cntrArrCur = dig->GetValue();
    //    printf("MSC type %u serial %08X last eventID = %6u\n", type, serial, fEventId);
    //    printf("\t%u events \n", nSpillEvents);
    while (type < 6) {
        if (type < 5) {
            UInt_t cnt3 = (d[idx] >> 21) & (BIT(8) - 1);
            UInt_t cnt2 = (d[idx] >> 14) & (BIT(8) - 1);
            UInt_t cnt1 = (d[idx] >> 7) & (BIT(8) - 1);
            UInt_t cnt0 = d[idx] & (BIT(8) - 1);
            //            printf("type = %u  %06u  %06u  %06u  %06u  \n", type, cnt3, cnt2, cnt1, cnt0);
        } else
            if (type == 5) {
            UInt_t cnt = d[idx] & (BIT(28) - 1);
            if (iCnt >= nCnt)
                continue;
            cntrArrCur[iCnt++] = cnt;
            //            printf("\ttype = %u  arr[%2u] = %8u\n", type, iCnt - 1, cntrArrCur[iCnt - 1]);
        }
        type = (d[++idx] >> 28) & (BIT(5) - 1);
    }

    return kBMNSUCCESS;
};

BmnStatus BmnRawDataDecoder::DecodeDataToDigi() {
    fRawTree = nullptr;
    fRawTreeSpills = nullptr;
    tdc = nullptr;
    tqdc_adc = nullptr;
    tqdc_tdc = nullptr;
    hrb = nullptr;
    sync = nullptr;
    adc32 = nullptr;
    adc128 = nullptr;
    adc = nullptr;
    tacquila = nullptr;
    tacquila2 = nullptr;
    eventHeaderDAQ = nullptr;
    msc = nullptr;

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
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("TQDC_ADC", &tqdc_adc);
    fRawTree->SetBranchAddress("TQDC_TDC", &tqdc_tdc);
    fRawTree->SetBranchAddress("HRB", &hrb);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("ADC32", &adc32);
    fRawTree->SetBranchAddress("ADC128", &adc128);
    fRawTree->SetBranchAddress("ADC", &adc);
    fRawTree->SetBranchAddress("Tacquila", &tacquila);
    fRawTree->SetBranchAddress("Tacquila2", &tacquila2);
    fRawTree->SetBranchAddress("BmnEventHeader.", &eventHeaderDAQ);

    fRawTreeSpills = (TTree *) fRootFileIn->Get("BMN_RAW_SPILLS");
    if (fRawTreeSpills)
        fRawTreeSpills->SetBranchAddress("MSC", &msc);

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
        if (fSiliconMapper) fSiliconMapper->LoadPedestalsMK(fRawTree, adc128, eventHeaderDAQ, Min(fNevents, (UInt_t) 300000));
        //        if (fSiliconMapper){
        //            fSiliconMapper->DrawDebugHistsMK("sil-sig-cms-MK.pdf");
        ////            fSiliconMapper->DrawDebugHists("sil-sig-cms-SM.pdf");
        ////            return kBMNSUCCESS;
        //        }
        if (fGemMapper) fGemMapper->LoadPedestalsMK(fRawTree, adc32, eventHeaderDAQ, Min(fNevents, (UInt_t) 300000));
        //        if (fSiliconMapper) {
        //            fSiliconMapper->DrawDebugHists2D();
        //            fSiliconMapper->DrawDebugHists();
        //            fSiliconMapper->ClearDebugHists();
        ////            return kBMNSUCCESS;
        //        }
        printf("[INFO]" ANSI_COLOR_BLUE " First payload loop\n" ANSI_COLOR_RESET);
        for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
            ClearArrays();
            fRawTree->GetEntry(iEv);
            BmnEventHeader* headDAQ = eventHeaderDAQ;
            if (!headDAQ) continue;
            curEventType = headDAQ->GetEventType();
            fEventId = headDAQ->GetEventId();
            if (fVerbose == 1) {
                if (iEv % 5000 == 0) cout << "Digitization events " << iEv << "/" << fNevents << ";" << endl;
            } else if (fVerbose == 0)
                DrawBar(iEv, fNevents);
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
            fRawTree->GetEntry(iEv);

            BmnEventHeader* headDAQ = eventHeaderDAQ;
            if (!headDAQ) continue;
            curEventType = headDAQ->GetEventType();
            if (curEventType != kBMNPEDESTAL) continue;
            if (fPedEvCntr != fEvForPedestals) {
                CopyDataToPedMap(adc32, adc128, fPedEvCntr);
                fPedEvCntr++;
                if (fVerbose == 1) {
                    if (fPedEvCntr % 100 == 0 && fPedEvCntr > 0) cout << "Pedestal event #" << fPedEvCntr << "/" << fEvForPedestals << ";" << endl;
                } else if (fVerbose == 0)
                    DrawBar(fPedEvCntr, fEvForPedestals);
            } else break;
        }
        if (fPedEvCntr != fEvForPedestals) {
            printf(ANSI_COLOR_RED "\n[WARNING]" ANSI_COLOR_RESET);
            printf(" Not enough pedestal events (%d instead of %d)\n", fPedEvCntr, fEvForPedestals);
        }
        if (fGemMapper) fGemMapper->RecalculatePedestalsAugmented();
        if (fSiliconMapper) fSiliconMapper->RecalculatePedestalsAugmented(); //RecalculatePedestalsByMap();
        if (fCscMapper)fCscMapper->RecalculatePedestalsAugmented();
        //        if (fGemMapper) {
        //            fGemMapper->DrawDebugHists2D();
        //            fGemMapper->DrawDebugHists();
        ////            fGemMapper->ClearDebugHists();
        //            return kBMNSUCCESS;
        //        }
        //        if (fSiliconMapper) {
        //            fSiliconMapper->DrawDebugHists2D();
        //            fSiliconMapper->DrawDebugHists();
        //            fSiliconMapper->ClearDebugHists();
        ////            return kBMNSUCCESS;
        //        }
        fPedEvCntr = 0;
        printf("\n[INFO]" ANSI_COLOR_BLUE " Clear noisy channels:\n" ANSI_COLOR_RESET);
        printf("\tFilling signal profiles for station-module-layer histograms\n");
        UInt_t nEvForNoiseCorrection = 10000;
        printf("\tNumber of requested events is ");
        printf(ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, nEvForNoiseCorrection);
        printf("\tActual number of events is ");
        printf(ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, fNevents);
        UInt_t n = Min(fNevents, nEvForNoiseCorrection);
        for (UInt_t iEv = 0; iEv < n; ++iEv) {
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
                    printf("\n[INFO]");
                    printf(ANSI_COLOR_BLUE " ADC pedestals recalculation\n" ANSI_COLOR_RESET);
                    if (fGemMapper) fGemMapper->RecalculatePedestalsAugmented();
                    if (fSiliconMapper) fSiliconMapper->RecalculatePedestalsAugmented(); //RecalculatePedestalsByMap();
                    if (fCscMapper)fCscMapper->RecalculatePedestalsAugmented();
                    fPedEvCntr = 0;
                }
            }
            if (fVerbose == 1) {
                if (iEv % 5000 == 0 && iEv > 0) cout << "Profile event #" << iEv << "/" << n << ";" << endl;
            } else if (fVerbose == 0)
                DrawBar(iEv, n);
            if (fGemMapper) fGemMapper->FillProfiles(adc32);
            //            printf("ev %6d\n", iEv);
            if (fSiliconMapper) fSiliconMapper->FillProfiles(adc128);
            if (fCscMapper) fCscMapper->FillProfiles(adc32);
            prevEventType = curEventType;
        }
        printf("\n[INFO]" ANSI_COLOR_BLUE " Marking noisy channels\n" ANSI_COLOR_RESET);
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
            fSiliconMapper = new BmnSiliconRaw2Digit(fPeriodId, fRunId, fSiliconSerials, fSiliconMapFileName, fBmnSetup, GetAdcDecoMode());
            fSiliconMapper->InitAdcProcessorMK(fRunId, 1, 0, 0, 0);
            fSiliconMapper->LoadPedestalsMK(fRawTree, adc128, eventHeaderDAQ, Min(fNevents, (UInt_t) 300000));
            //            fSiliconMapper->RecalculatePedestalsMK(fPedEvCntr);
        }
        //        if (fSiliconMapper) {
        //            fSiliconMapper->DrawDebugHists2D();
        //            fSiliconMapper->DrawDebugHists();
        //            fSiliconMapper->ClearDebugHists();
        ////            return kBMNSUCCESS;
        //        }
        if (fGemMapper) {
            delete fGemMapper;
            fGemMapper = new BmnGemRaw2Digit(fPeriodId, fRunId, fGemSerials, fGemMapFileName, fBmnSetup, GetAdcDecoMode());
            fGemMapper->InitAdcProcessorMK(fRunId, 1, 0, 0, 0);
            fGemMapper->LoadPedestalsMK(fRawTree, adc32, eventHeaderDAQ, Min(fNevents, (UInt_t) 300000));
            //            fGemMapper->RecalculatePedestalsMK(fPedEvCntr);
        }
    } else
        if (fGemMapper || fSiliconMapper || fCscMapper) {
        printf("\n[INFO]");
        printf(ANSI_COLOR_BLUE " Collecting data for ADC pedestals calculation:\n" ANSI_COLOR_RESET);
        printf("\tNumber of requested pedestal events is ");
        printf(ANSI_COLOR_RED "%d\n" ANSI_COLOR_RESET, fEvForPedestals);
        for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
            fRawTree->GetEntry(iEv);

            BmnEventHeader* headDAQ = eventHeaderDAQ;
            if (!headDAQ) continue;
            curEventType = headDAQ->GetEventType();

            if (curEventType != kBMNPEDESTAL) continue;
            if (fPedEvCntr != fEvForPedestals) {
                CopyDataToPedMap(adc32, adc128, fPedEvCntr);
                fPedEvCntr++;
                if (fVerbose == 1) {
                    if (fPedEvCntr % 100 == 0 && fPedEvCntr > 0) cout << "Pedestal event #" << fPedEvCntr << "/" << fEvForPedestals << ";" << endl;
                } else if (fVerbose == 0)
                    DrawBar(fPedEvCntr, fEvForPedestals);
            } else break;
        }
        if (fPedEvCntr != fEvForPedestals) {
            printf(ANSI_COLOR_RED "\n[WARNING]" ANSI_COLOR_RESET);
            printf(" Not enough pedestal events (%d instead of %d)\n", fPedEvCntr, fEvForPedestals);
        }
        printf("\n[INFO]");
        printf(ANSI_COLOR_BLUE " Calculating pedestals\n" ANSI_COLOR_RESET);
        if (fGemMapper) fGemMapper->RecalculatePedestalsAugmented();
        if (fSiliconMapper) fSiliconMapper->RecalculatePedestalsAugmented(); //RecalculatePedestalsByMap();
        if (fCscMapper) fCscMapper->RecalculatePedestalsAugmented();
        fPedEvCntr = 0;

        //        if (fSiliconMapper) {
        //            fSiliconMapper->DrawDebugHists2D();
        //            fSiliconMapper->DrawDebugHists();
        //            fSiliconMapper->ClearDebugHists();
        ////            return kBMNSUCCESS;
        //        }
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
        //        printf("iEv %d\n", iEv);
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
            TangoData db_tango;
            enumConditions condition = conditionEqual;
            bool condition_value = 1;
            vector<int> map_channel_run5{1, 3, 0, 5, 2, 6, 4, 7};
            vector<int> map_channel_run6_b1529{1, 3, 0, 5, 2, 6, 4};
            vector<int> map_channel_run6_a1529{1, 3, 0, 5, 6, 4, 2};
            vector<int>* map_channel = nullptr;
            switch (fPeriodId) {
                case 5:
                    map_channel = &map_channel_run5;
                    break;
                case 6:
                    if (fRunId < 1569)
                        map_channel = &map_channel_run6_b1529;
                    else
                        map_channel = &map_channel_run6_a1529;
                    break;
                default:
                    printf("Warning: unknown GEM Tango channel map for the run!\n");
                    break;

            }
            TString date_start = fRunStartTime.AsString("s"); // 1252 run
            TString date_end = fRunEndTime.AsString("s");

            UInt_t runLength = fRunEndTime.AsDouble() - fRunStartTime.AsDouble(); //in seconds.nanoseconds
            Double_t timeStep = runLength * 1.0 / fNevents; //time for one event
            //printf("Run duration = %d sec.\t TimeStep = %f sec./event\n", runLength, timeStep);
            if (map_channel) {
                TObjArray* tango_data_gem = db_tango.SearchTangoIntervals(
                        (char*) "gem", (char*) "trip", (char*) date_start.Data(), (char*) date_end.Data(), condition, condition_value, map_channel);
                if (tango_data_gem) {
                    for (Int_t i = 0; i < tango_data_gem->GetEntriesFast(); ++i) {
                        TObjArray* currGemTripInfo = (TObjArray*) tango_data_gem->At(i);
                        if (currGemTripInfo->GetEntriesFast() != 0)
                            for (Int_t j = 0; j < currGemTripInfo->GetEntriesFast(); ++j) {
                                TangoTimeInterval* ti = (TangoTimeInterval*) currGemTripInfo->At(j);
                                startTripEvent.push_back(UInt_t((ti->start_time.Convert() - fRunStartTime.AsDouble()) / timeStep));
                                endTripEvent.push_back(UInt_t((ti->end_time.Convert() - fRunStartTime.AsDouble()) / timeStep));
                            }
                    }
                }
            }
        }

        fSpillCntr += headDAQ->GetSpillStart() ? 1 : 0;
        //        if (headDAQ->GetSpillStart())
        //            printf("Spills: %6d  iEv %6d\n", fSpillCntr, iEv);
        Bool_t isTripEvent = kFALSE;
        for (Int_t iTrip = 0; iTrip < startTripEvent.size(); ++iTrip) {
            if (headDAQ->GetEventId() > startTripEvent[iTrip] && headDAQ->GetEventId() < endTripEvent[iTrip]) {
                isTripEvent = kTRUE;
                break;
            }
        }
        if (fVerbose == 1) {
            if (iEv % 5000 == 0)
                cout << "Digitization: " << iEv << "/" << fNevents << " processed; Spill #" << fSpillCntr << endl;
        } else if (fVerbose == 0)
            DrawBar(iEv, fNevents);

        TStopwatch timer;
        Double_t rtime;
        Double_t ctime;
        if (fTrigMapper) {
            timer.Start();
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                    printf("\nReal time %f s, CPU time %f s  fTrigMapper TQDC\n", rtime, ctime);
            timer.Start();
            fTrigMapper->FillEvent(tdc);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                    printf("Real time %f s, CPU time %f s  fTrigMapper TDC\n", rtime, ctime);
            fT0Time = 0.;
            GetT0Info(fT0Time, fT0Width);
        }
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
        //        BmnTrigUnion s;
        //        BmnTrigStructPeriod7SetupBMN bs;
        //        bs.BC1 = true;
        //        bs.BC2 = true;
        //        bs.VETO = true;
        //        bs.ThrBD = 2;
        //        bs.ThrSI = 3;
        //        s.Period7BMN = bs;
        //        eventHeader->SetTrigState(s);
        //        BmnTrigUnion ws = eventHeader->GetTrigState();
        //        printf("ThrBD set to %u\n", ws.Period7BMN.ThrBD);
        if (curEventType == kBMNPEDESTAL) {
            fPedEvCntrBySpill++;
            if (GetAdcDecoMode() == kBMNADCSM) {
                if (fPedEvCntr < fEvForPedestals - 1) {
                    CopyDataToPedMap(adc32, adc128, fPedEvCntr);
                    fPedEvCntr++;
                }
            }
        } else { // payload
            if (prevEventType == kBMNPEDESTAL && fPedEvCntr == fEvForPedestals - 1) {
                printf("\n[INFO]");
                printf(ANSI_COLOR_BLUE " Recalculating pedestals\n" ANSI_COLOR_RESET);
                if (fGemMapper) fGemMapper->RecalculatePedestalsAugmented();
                if (fSiliconMapper) fSiliconMapper->RecalculatePedestalsAugmented(); //RecalculatePedestalsByMap();
                if (fCscMapper)fCscMapper->RecalculatePedestalsAugmented();
                fPedEvCntr = 0;
            }
            if (GetAdcDecoMode() == kBMNADCSM) {
                timer.Start();
                if (fCscMapper) fCscMapper->FillEvent(adc32, csc);
                timer.Stop();
                rtime = timer.RealTime();
                ctime = timer.CpuTime();
                //                                                printf("\nReal time %f s, CPU time %f s  fCscMapper\n", rtime, ctime);
                timer.Start();
                if (fGemMapper) fGemMapper->FillEvent(adc32, gem);
                timer.Stop();
                rtime = timer.RealTime();
                ctime = timer.CpuTime();
                //                                                printf("Real time %f s, CPU time %f s  fGemMapper\n", rtime, ctime);
                timer.Start();
                if (fSiliconMapper) fSiliconMapper->FillEvent(adc128, silicon);
                timer.Stop();
                rtime = timer.RealTime();
                ctime = timer.CpuTime();
                //                                                printf("Real time %f s, CPU time %f s  fSiliconMapper\n", rtime, ctime);
            } else {
                if (fGemMapper) fGemMapper->FillEventMK(adc32, gem, csc);
                if (fSiliconMapper) fSiliconMapper->FillEventMK(adc128, silicon);
            }
            timer.Start();
            if (fDchMapper) fDchMapper->FillEvent(tdc, &fTimeShifts, dch, fT0Time);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                                    printf("Real time %f s, CPU time %f s  fDchMapper\n", rtime, ctime);
            timer.Start();
            if (fMwpcMapper) fMwpcMapper->FillEvent(hrb, mwpc);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                                    printf("Real time %f s, CPU time %f s  fMwpcMapper\n", rtime, ctime);
            timer.Start();
            if (fTof400Mapper) fTof400Mapper->FillEvent(tdc, &fTimeShifts, tof400);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                                    printf("Real time %f s, CPU time %f s  fTof400Mapper\n", rtime, ctime);
            timer.Start();
            if (fTof700Mapper && fT0Time != 0. && fT0Width != -1.) fTof700Mapper->fillEvent(tdc, &fTimeShifts, fT0Time, fT0Width, tof700);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                                    printf("Real time %f s, CPU time %f s  fTof700Mapper\n", rtime, ctime);
            timer.Start();
            if (fZDCMapper) fZDCMapper->fillEvent(adc, zdc);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                                    printf("Real time %f s, CPU time %f s  fZDCMapper\n", rtime, ctime);
            timer.Start();
            if (fScWallMapper) fScWallMapper->fillEvent(adc, scwall);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                                    printf("Real time %f s, CPU time %f s  fScWallMapper\n", rtime, ctime);
            timer.Start();
            if (fFHCalMapper) fFHCalMapper->fillEvent(adc, fhcal);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                                    printf("Real time %f s, CPU time %f s  fFHCalMapper\n", rtime, ctime);
            timer.Start();
            if (fHodoMapper) fHodoMapper->fillEvent(tqdc_tdc, tqdc_adc, hodo);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                                    printf("Real time %f s, CPU time %f s  fHodoMapper\n", rtime, ctime);
            timer.Start();
            if (fECALMapper) fECALMapper->fillEvent(adc, ecal);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                                printf("Real time %f s, CPU time %f s  fECALMapper\n", rtime, ctime);
            timer.Start();
            if (fLANDMapper) fLANDMapper->fillEvent(tacquila2, land);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                    printf("Real time %f s, CPU time %f s  fLANDMapper\n", rtime, ctime);
            if (fTofCalMapper) fTofCalMapper->fillEvent(tacquila, tofcal);
            timer.Stop();
            rtime = timer.RealTime();
            ctime = timer.CpuTime();
            //                    printf("Real time %f s, CPU time %f s  fTOFCALMapper\n", rtime, ctime);
        }
        if (fMSCMapper) fMSCMapper->SumEvent(msc, eventHeader, spillHeader, fPedEvCntrBySpill);

        fDigiTree->Fill();
        prevEventType = curEventType;
    }
    //                        if (fSiliconMapper) {
    //                            fSiliconMapper->DrawDebugHistsMK("sil-sig-cms.pdf");
    ////                            fSiliconMapper->ClearDebugHists();
    //                        }

    if (fTof700Mapper) {
        fTof700Mapper->WriteSlewingResults();
        fDigiFileOut->cd();
    }
    fRunEndTime = TTimeStamp(time_t(fTime_s), fTime_ns);
    DigiRunHeader * runHeader = new DigiRunHeader(fPeriodId, fRunId, fRunStartTime, fRunEndTime);
    if (fMSCMapper) fMSCMapper->FillRunHeader(runHeader);
    fDigiFileOut->WriteObject(runHeader, fDigiRunHdrName.Data());

    printf(ANSI_COLOR_RED "\n=============== RUN" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BLUE " %04d " ANSI_COLOR_RESET, fRunId);
    printf(ANSI_COLOR_RED "SUMMARY ===============\n" ANSI_COLOR_RESET);
    printf("START (event 1):\t%s\n", fRunStartTime.AsString());
    printf("FINISH (event %d):\t%s\n", fNevents, fRunEndTime.AsString());
    printf(ANSI_COLOR_RED "================================================\n" ANSI_COLOR_RESET);

    fDigiTree->Write();
    fDigiTreeSpills->Write();
    DisposeDecoder();
    fDigiFileOut->Write();
    fDigiFileOut->Close();
    delete runHeader;
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

    fDigiTreeSpills = new TTree("spill", "spill");
    spillHeader = new BmnSpillHeader();
    fDigiTreeSpills->Branch("BmnSpillHeader.", &spillHeader);


    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    // check if detector is in setup and is active
    if (fDetectorSetup.count(kBC) > 0 && fDetectorSetup.at(kBC) == 1) {
        fTrigMapper = new BmnTrigRaw2Digit(fTrigPlaceMapFileName, fTrigChannelMapFileName, fDigiTree);
        if (fT0Serial == 0) {
            fT0Serial = fTrigMapper->GetT0Serial();
            printf("T0 serial 0x%X got from trig mapping\n", fT0Serial);
        }
        fTrigMapper->SetSetup(fBmnSetup);
    }

    if (fDetectorSetup.count(kMWPC) > 0 && fDetectorSetup.at(kMWPC) == 1) {
        mwpc = new TClonesArray("BmnMwpcDigit");
        fDigiTree->Branch("MWPC", &mwpc);
        fMwpcMapper = new BmnMwpcRaw2Digit(fMwpcMapFileName);
    }

    if (fDetectorSetup.count(kSILICON) > 0 && fDetectorSetup.at(kSILICON) == 1) {
        silicon = new TClonesArray("BmnSiliconDigit");
        fDigiTree->Branch("SILICON", &silicon);
        fSiliconMapper = new BmnSiliconRaw2Digit(fPeriodId, fRunId, fSiliconSerials, fSiliconMapFileName, fBmnSetup, GetAdcDecoMode());
    }

    bool isGEM = fDetectorSetup.count(kGEM) > 0 && fDetectorSetup.at(kGEM) == 1;
    bool isCSC = fDetectorSetup.count(kCSC) > 0 && fDetectorSetup.at(kCSC) == 1;
    if (isGEM || isCSC && GetAdcDecoMode() == kBMNADCMK) {
        gem = new TClonesArray("BmnGemStripDigit");
        fDigiTree->Branch("GEM", &gem);
        fGemMapper = new BmnGemRaw2Digit(fPeriodId, fRunId, fGemSerials, fGemMapFileName, fBmnSetup, GetAdcDecoMode());
    }

    if (fDetectorSetup.count(kTOF1) > 0 && fDetectorSetup.at(kTOF1) == 1) {
        tof400 = new TClonesArray("BmnTof1Digit");
        fDigiTree->Branch("TOF400", &tof400);
        fTof400Mapper = new BmnTof1Raw2Digit();
        fTof400Mapper->setVerbose(fVerbose);
        Bool_t FlagTemp = fTof400Mapper->setRun(fPeriodId, fRunId);
        if (FlagTemp == kFALSE) {
            if (fTof400PlaceMapFileName.Sizeof() > 1 && fTof400StripMapFileName.Sizeof() > 1) {
                TString dir = Form("%s%s", getenv("VMCWORKDIR"), "/input/");
                fTof400Mapper->setMapFromFile(dir + fTof400PlaceMapFileName.Data(), dir + fTof400StripMapFileName.Data());
            } else
                cout << "Map for TOF400 are not loaded" << endl;
        }
    }

    if (fDetectorSetup.count(kTOF) > 0 && fDetectorSetup.at(kTOF) == 1) {
        if (fTOF700ReferenceRun <= 0) {
            UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("TOF2", "slewing_file_id", fPeriodId, fRunId); //(detector_name, parameter_name, period_number, run_number)
            if (pDetectorParameter != NULL) {
                IntValue* pReferenceRun = (IntValue*) pDetectorParameter->GetValue();
                fTOF700ReferenceRun = pReferenceRun->value;
                delete pDetectorParameter;
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

    if (fDetectorSetup.count(kDCH) > 0 && fDetectorSetup.at(kDCH) == 1) {
        dch = new TClonesArray("BmnDchDigit");
        fDigiTree->Branch("DCH", &dch);
        fDchMapper = new BmnDchRaw2Digit(fPeriodId, fRunId);
    }

    if (fDetectorSetup.count(kZDC) > 0 && fDetectorSetup.at(kZDC) == 1) {
        zdc = new TClonesArray("BmnZDCDigit");
        fDigiTree->Branch("ZdcDigit", &zdc);
        fZDCMapper = new BmnZDCRaw2Digit(fPeriodId, fRunId, fZDCMapFileName, fZDCCalibrationFileName);
        //        fZDCMapper->print();
    }

    if (fDetectorSetup.count(kECAL) > 0 && fDetectorSetup.at(kECAL) == 1) {
        ecal = new TClonesArray("BmnECALDigit");
        fDigiTree->Branch("EcalDigit", &ecal);
        fECALMapper = new BmnECALRaw2Digit(fRunId);
        //        fECALMapper->print();
    }

    if (fDetectorSetup.count(kLAND) > 0 && fDetectorSetup.at(kLAND) == 1) {
        land = new TClonesArray("BmnLANDDigit");
        fDigiTree->Branch("LAND", &land);
        fLANDMapper = new BmnLANDRaw2Digit(fLANDMapFileName,
                fLANDClockFileName, fLANDTCalFileName, fLANDDiffSyncFileName,
                fLANDVScintFileName);
    }

   if (fDetectorSetup.count(kTOFCAL) > 0 && fDetectorSetup.at(kTOFCAL) == 1) {
        tofcal = new TClonesArray("BmnTofCalDigit");
	fDigiTree->Branch("TofCal", &tofcal);
        fTofCalMapper = new BmnTofCalRaw2Digit(fTofCalMapFileName,
                fTofCalClockFileName, fTofCalTCalFileName, fTofCalDiffSyncFileName,
                fTofCalVScintFileName);
    }

    if (fDetectorSetup.count(kSCWALL) > 0 && fDetectorSetup.at(kSCWALL) == 1) {
        printf("scwall in setup \n");
        scwall = new TClonesArray("BmnScWallDigi");
        fDigiTree->Branch("ScWallDigi", &scwall);
        fScWallMapper = new BmnScWallRaw2Digit(fPeriodId, fRunId, fScWallMapFileName, fScWallCalibrationFileName);
        fScWallMapper->print();
    }

    if (fDetectorSetup.count(kFHCAL) > 0 && fDetectorSetup.at(kFHCAL) == 1) {
        printf("fhcal in setup \n");
        fhcal = new TClonesArray("BmnFHCalDigi");
        fDigiTree->Branch("FHCalDigi", &fhcal);
        fFHCalMapper = new BmnFHCalRaw2Digit(fPeriodId, fRunId, fFHCalMapFileName, fFHCalCalibrationFileName);
        fFHCalMapper->print();
    }

    if (fDetectorSetup.count(kHODO) > 0 && fDetectorSetup.at(kHODO) == 1) {
        printf("hodo in setup \n");
        hodo = new TClonesArray("BmnHodoDigi");
        fDigiTree->Branch("HodoDigi", &hodo);
        fHodoMapper = new BmnHodoRaw2Digit(fPeriodId, fRunId, fHodoMapFileName, fHodoCalibrationFileName);
        fHodoMapper->print();
    }

    //bool isGEM = fDetectorSetup.count(kGEM) > 0 && fDetectorSetup.at(kGEM) == 1;
    //bool isCSC = fDetectorSetup.count(kCSC) > 0 && fDetectorSetup.at(kCSC) == 1;
    if (isGEM && GetAdcDecoMode() == kBMNADCMK && GetPeriodId() > 6 || isCSC) {
        csc = new TClonesArray("BmnCSCDigit");
        fDigiTree->Branch("CSC", &csc);
        if (GetAdcDecoMode() == kBMNADCSM)
            fCscMapper = new BmnCscRaw2Digit(fPeriodId, fRunId, fCscSerials, fCscMapFileName);
    }
    if (fRawTreeSpills)
        fMSCMapper = new BmnMscRaw2Digit(fPeriodId, fRunId, fMSCMapFileName, fRawTreeSpills, fDigiTreeSpills);

    fPedEvCntr = 0; // counter for pedestal events between two recalculations
    fPedEvCntrBySpill = 0; // counter for pedestal events between two spills
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
    if (scwall) scwall->Delete();
    if (fhcal) fhcal->Delete();
    if (hodo) hodo->Delete();
    if (ecal) ecal->Delete();
    if (land) land->Delete();
    if (tofcal) tofcal->Delete();
    if (fTrigMapper)
        fTrigMapper->ClearArrays();
    fTimeShifts.clear();
    eventHeader->Clear();
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::DecodeDataToDigiIterate() {
    ClearArrays();

    BmnEventHeader* headDAQ = eventHeaderDAQ;
    fCurEventType = headDAQ->GetEventType();
    fEventId = headDAQ->GetEventId();
    FillTimeShiftsMap();

    if (fVerbose == 1) {
        if (fEventId % 5000 == 0)
            cout << "Digitization: " << fEventId << " processed; Spill #" << fSpillCntr << endl;
    }
    //    printf("fCurEventType %d\n", fCurEventType);
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
                printf("\n[INFO]");
                printf(ANSI_COLOR_BLUE " Recalculating pedestals\n" ANSI_COLOR_RESET);
                if (fGemMapper)fGemMapper->RecalculatePedestalsAugmented();
                if (fSiliconMapper)fSiliconMapper->RecalculatePedestalsAugmented();
                if (fCscMapper)fCscMapper->RecalculatePedestalsAugmented();
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
        if (fScWallMapper) fScWallMapper->fillEvent(adc, scwall);
        if (fFHCalMapper) fFHCalMapper->fillEvent(adc, fhcal);
        if (fHodoMapper) fHodoMapper->fillEvent(tqdc_tdc, tqdc_adc, hodo);
        if (fECALMapper) fECALMapper->fillEvent(adc, ecal);
        if (fLANDMapper) fLANDMapper->fillEvent(tacquila2, land);
        if (fTofCalMapper) fTofCalMapper->fillEvent(tacquila, tofcal);
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

    return kBMNSUCCESS;
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
    if (fGemMapper) delete fGemMapper;
    if (fCscMapper) delete fCscMapper;
    if (fSiliconMapper) delete fSiliconMapper;
    if (fDchMapper) delete fDchMapper;
    if (fMwpcMapper) delete fMwpcMapper;
    if (fTrigMapper) delete fTrigMapper;
    if (fTof400Mapper) delete fTof400Mapper;
    if (fTof700Mapper) delete fTof700Mapper;
    if (fZDCMapper) delete fZDCMapper;
    if (fScWallMapper) delete fScWallMapper;
    if (fFHCalMapper) delete fFHCalMapper;
    if (fHodoMapper) delete fHodoMapper;
    if (fECALMapper) delete fECALMapper;
    if (fLANDMapper) delete fLANDMapper;
    if (fTofCalMapper) delete fTofCalMapper;
    if (fMSCMapper) delete fMSCMapper;

    delete sync;
    delete adc32;
    delete adc128;
    delete adc;
    delete tacquila;
    delete tacquila2;
    delete tdc;
    delete tqdc_adc;
    delete tqdc_tdc;
    delete hrb;
    if (msc) delete msc;

    if (gem) delete gem;
    if (csc) delete csc;
    if (dch) delete dch;
    if (mwpc) delete mwpc;
    if (silicon) delete silicon;
    if (tof400) delete tof400;
    if (tof700) delete tof700;
    if (zdc) delete zdc;
    if (scwall) delete scwall;
    if (fhcal) delete fhcal;
    if (hodo) delete hodo;
    if (ecal) delete ecal;
    if (land) delete land;
    if (tofcal) delete tofcal;

    delete eventHeader;
    delete spillHeader;
    if (eventHeaderDAQ) delete eventHeaderDAQ;
    if (fRawTree) fRawTree->Delete();
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTimeShiftsMap() {
    if (fT0Serial == 0) return kBMNERROR;
    Long64_t t0time = 0;
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        if (syncDig->GetSerial() == fT0Serial) {
            t0time = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
            fTime_s = syncDig->GetTime_sec();
            fTime_ns = syncDig->GetTime_ns();
            //            printf("serial %08X sync: %s\n", syncDig->GetSerial(), TTimeStamp(time_t(fTime_s), fTime_ns).AsString());
            if (fEventId == 1) {
                fRunStartTime = TTimeStamp(time_t(fTime_s), fTime_ns);
            }
            //            printf("T0 sync: %s \n", TTimeStamp(time_t(fTime_s), fTime_ns).AsString());
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
        vector<BmnSiliconMapping*> & channelMap = fSiliconMapper->GetMap();
        map<UInt_t, Int_t>& serialMap = fSiliconMapper->GetSerialMap();
        for (UInt_t iAdc = 0; iAdc < adcSil->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adcSil->At(iAdc);
            UInt_t iCh = adcDig->GetChannel();
            UInt_t ser = adcDig->GetSerial();
            //            for (Int_t iSer = 0; iSer < fNSiliconSerials; ++iSer) {
            for (auto & channelMapEl : channelMap) {
                //                if (adcDig->GetSerial() != fSiliconSerials[iSer]) continue;
                if ((ser != channelMapEl->serial) || (iCh < channelMapEl->channel_low) || (iCh > channelMapEl->channel_high)) continue;
                Int_t iSer = serialMap[ser];
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
        if (word == SYNC_RUN_NUMBER) {
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
    printf("ZDC map name = %s, Nboards = %d \n", fZDCMapFileName.Data(), fNZDCSerials);


    BmnScWallRaw2Digit* tempScWallMapper = new BmnScWallRaw2Digit();
    tempScWallMapper->ParseConfig(fScWallMapFileName);
    fScWallSerials = tempScWallMapper->GetScWallSerials();
    fNScWallSerials = fScWallSerials.size();
    delete tempScWallMapper;


    BmnFHCalRaw2Digit* tempFHCalMapper = new BmnFHCalRaw2Digit();
    tempFHCalMapper->ParseConfig(fFHCalMapFileName);
    fFHCalSerials = tempFHCalMapper->GetFHCalSerials();
    fNFHCalSerials = fFHCalSerials.size();
    delete tempFHCalMapper;


    BmnHodoRaw2Digit* tempHodoMapper = new BmnHodoRaw2Digit();
    tempHodoMapper->ParseConfig(fHodoMapFileName);
    fHodoSerials = tempHodoMapper->GetHodoSerials();
    fNHodoSerials = fHodoSerials.size();
    delete tempHodoMapper;


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

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::GetT0Info(Double_t& t0time, Double_t &t0width) {
    vector<TClonesArray*>* trigArr = fTrigMapper->GetTrigArrays();
    BmnTrigDigit* dig = 0;
    for (auto ar : *trigArr) {
        if ((fPeriodId == 8) && (fBmnSetup == kSRCSETUP)) { // temporary crutches
            if ((strcmp(ar->GetName(), "T0_1_A"))) continue;
            for (int i = 0; i < ar->GetEntriesFast(); i++) {
                if (ar->GetEntriesFast() == 0)
                    return kBMNERROR;
                dig = (BmnTrigDigit*) ar->At(i);
                t0time = dig->GetTime();
                t0width = dig->GetAmp();
                //                    printf(" t0 %f t0w %f n %d\n", t0time, t0width, ar->GetEntriesFast());
                return kBMNSUCCESS;
            }
        } else {
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
    utc_valid = TTimeStamp(2022, 6, 1, 0, 0, 1);
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
