#include "BmnRawDataDecoder.h"
#include "TH1I.h"

/***************** SET OF DAQ CONSTANTS *****************/
const UInt_t kSYNC1 = 0x2A502A50;
const UInt_t kSYNC2 = 0x4A624A62;
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
const UInt_t kTDC64V = 0x10;
const UInt_t kTDC64VHLE = 0x53;
const UInt_t kTDC72VHL = 0x12;
const UInt_t kTRIG = 0xA;
const UInt_t kMSC = 0xF;
const UInt_t kUT24VE = 0x49;
const UInt_t kADC64VE = 0xD4;
const UInt_t kFVME = 0xD1;
const UInt_t kU40VE_RC = 0x4C;
/********************************************************/

const Int_t kPERIOD = 4;

using namespace std;

BmnRawDataDecoder::BmnRawDataDecoder() {
    fRunId = 0;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = 0;
    fLengthRawFile = 0;
    fCurentPositionRawFile = 0;
    t0 = NULL;
    bc1 = NULL;
    bc2 = NULL;
    veto = NULL;
    header = NULL;
    headerDAQ = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fRawTree = NULL;
    fDigiTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
    fDigiFileOut = NULL;
    sync = NULL;
    tdc = NULL;
    adc = NULL;
    msc = NULL;
    dch = NULL;
    tof400 = NULL;
    tof700 = NULL;
    gem = NULL;
    fPedestalRun = kFALSE;
    fRootFileName = "";
    fRawFileName = "";
    fDigiFileName = "";
    fDchMapFileName = "";
    fTrigMapFileName = "";
    fGemMapFileName = "";
    fTof400MapFileName = "";
    fTof700MapFileName = "";
}

BmnRawDataDecoder::BmnRawDataDecoder(TString file, ULong_t nEvents) {

    t0 = NULL;
    header = NULL;
    headerDAQ = NULL;
    bc2 = NULL;
    bc1 = NULL;
    veto = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fRawTree = NULL;
    fDigiTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
    fDigiFileOut = NULL;
    sync = NULL;
    tdc = NULL;
    adc = NULL;
    msc = NULL;
    dch = NULL;
    tof400 = NULL;
    tof700 = NULL;
    gem = NULL;
    fRawFileName = file;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = nEvents;
    fPedestalRun = kFALSE;
    fRunId = TString(file(fRawFileName.Length() - 8, 3)).Atoi();
    fRootFileName = Form("bmn_run%04d_raw.root", fRunId);
    fDigiFileName = Form("bmn_run%04d_digi.root", fRunId);
    fDchMapFileName = "";
    fTrigMapFileName = "";
    fGemMapFileName = "";
    fTof400MapFileName = "";
    fTof700MapFileName = "";
}

BmnRawDataDecoder::~BmnRawDataDecoder() {
}

BmnStatus BmnRawDataDecoder::ConvertRawToRoot() {

    fRawTree = new TTree("BMN_RAW", "BMN_RAW");
    fRawFileIn = fopen(fRawFileName, "rb");
    if (fRawFileIn == NULL) {
        printf("\n!!!!!\ncannot open file %s\nConvertRawToRoot are stopped\n!!!!!\n\n", fRawFileName.Data());
        return kBMNERROR;
    }
    fRootFileOut = new TFile(fRootFileName, "recreate");

    fseeko64(fRawFileIn, 0, SEEK_END);
    fLengthRawFile = ftello64(fRawFileIn);
    rewind(fRawFileIn);
    printf("\nRawData File %s;\nLength RawData - %lld bytes (%.3f Mb)\n", fRawFileName.Data(), fLengthRawFile, fLengthRawFile / 1024. / 1024.);
    printf("RawRoot File %s\n\n", fRootFileName.Data());

    sync = new TClonesArray("BmnSyncDigit");
    adc = new TClonesArray("BmnADC32Digit");
    tdc = new TClonesArray("BmnTDCDigit");
    msc = new TClonesArray("BmnMSCDigit");
    headerDAQ = new TClonesArray("BmnEventHeader");

    fRawTree->Branch("SYNC", &sync);
    fRawTree->Branch("ADC", &adc);
    fRawTree->Branch("TDC", &tdc);
    fRawTree->Branch("MSC", &msc);
    fRawTree->Branch("EventHeader", &headerDAQ);

    UInt_t dat = 0;
    for (;;) {
        if (fMaxEvent > 0 && fNevents == fMaxEvent) break;
        //if (fread(&dat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
        fread(&dat, kWORDSIZE, 1, fRawFileIn);
        fCurentPositionRawFile = ftello64(fRawFileIn);
        if (fCurentPositionRawFile >= fLengthRawFile) break;
        if (dat == kSYNC1) { //search for start of event

            // read number of bytes in event
            if (fread(&dat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
            dat = dat / kNBYTESINWORD + 1; // bytes --> words
            if (dat >= 100000) { // what the constant?
                //printf("Wrong data size: %d\n", dat);
                //return kBMNERROR; // Why do you stop to convert in this case??
                printf("Wrong data size: %d:  skip this event\n", dat);
                fread(data, kWORDSIZE, dat, fRawFileIn);
            } else {

                //read array of current event data and process them
                if (fread(data, kWORDSIZE, dat, fRawFileIn) != dat) return kBMNERROR;
                fEventId = data[0];
                if (fEventId <= 0) continue; // skip bad events (it is possible, but what about 0?) 
                ProcessEvent(data, dat);
                fNevents++;
                fRawTree->Fill();
            }
        }
    }

    fCurentPositionRawFile = ftello64(fRawFileIn);
    printf("Readed %d events; %lld bytes (%.3f Mb)\n\n", fNevents, fCurentPositionRawFile, fCurentPositionRawFile / 1024. / 1024.);

    fRawTree->Write();
    fRootFileOut->Close();
    fclose(fRawFileIn);

    delete sync;
    delete adc;
    delete tdc;
    delete msc;

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ProcessEvent(UInt_t *d, UInt_t len) {

    sync->Clear();
    tdc->Clear();
    adc->Clear();
    msc->Clear();
    headerDAQ->Clear();

    if (fEventId % 1000 == 0) cout << "Converting event #" << d[0] << endl;

    UInt_t idx = 1;
    BmnEventType evType = kBMNPAYLOAD;

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
                Process_FVME(&data[idx], payload, serial, evType);
                break;
        }
        idx += payload;
    }
    new((*headerDAQ)[headerDAQ->GetEntriesFast()]) BmnEventHeader(fRunId, fEventId, fTime_s, fTime_ns, evType);
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

BmnStatus BmnRawDataDecoder::Process_FVME(UInt_t *d, UInt_t len, UInt_t serial, BmnEventType &evType) {
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
                        FillSYNC(d, serial, i);
                        break;
                    case kU40VE_RC:
                        if (type == 3)
                            //FIXME Make back compatibility!
                            //evType = ((d[i] & 0xFFFF) == 0xD8) ? kBMNPEDESTAL : kBMNPAYLOAD;
                            break;
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

    TClonesArray &ar_sync = *sync;
    new(ar_sync[sync->GetEntriesFast()]) BmnSyncDigit(serial, GlobalEvent, ts_t0_s, ts_t0_ns);

    idx += 3; //skip next 3 words (we've processed them)
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::DecodeDataToDigi() {

    fRootFileIn = new TFile(fRootFileName, "READ");
    if (fRootFileIn->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s \nDecodeDataToDigi are stopped\n!!!!\n", fRootFileName.Data());
        return kBMNERROR;
    } else {
        printf("\nInput root file: %s;\nOutput digi file: %s;\n", fRootFileName.Data(), fDigiFileName.Data());
    }
    fRawTree = (TTree *) fRootFileIn->Get("BMN_RAW");
    tdc = new TClonesArray("BmnTDCDigit");
    sync = new TClonesArray("BmnSyncDigit");
    adc = new TClonesArray("BmnADC32Digit");
    headerDAQ = new TClonesArray("BmnEventHeader");
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("ADC", &adc);
    fRawTree->SetBranchAddress("EventHeader", &headerDAQ);

    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    fDigiTree = new TTree("cbmsim", "bmn_digit");

    gem = new TClonesArray("BmnGemStripDigit");
    tof400 = new TClonesArray("BmnTof1Digit");
    tof700 = new TClonesArray("BmnTof2Digit");
    dch = new TClonesArray("BmnDchDigit");
    t0 = new TClonesArray("BmnTrigDigit");
    bc1 = new TClonesArray("BmnTrigDigit");
    bc2 = new TClonesArray("BmnTrigDigit");
    veto = new TClonesArray("BmnTrigDigit");
    header = new TClonesArray("BmnEventHeader");

    fDigiTree->Branch("EventHeader", &header);
    fDigiTree->Branch("T0", &t0);
    fDigiTree->Branch("BC1", &bc1);
    fDigiTree->Branch("BC2", &bc2);
    fDigiTree->Branch("VETO", &veto);
    fDigiTree->Branch("DCH", &dch);
    fDigiTree->Branch("GEM", &gem);
    fDigiTree->Branch("TOF400", &tof400);
    fDigiTree->Branch("TOF700", &tof700);

    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    BmnGemRaw2Digit *gemMapper = NULL;
    BmnDchRaw2Digit *dchMapper = new BmnDchRaw2Digit(kPERIOD, fRunId);
    BmnTrigRaw2Digit *trigMapper = new BmnTrigRaw2Digit(fTrigMapFileName);
    BmnTof1Raw2Digit *tof400Mapper = new BmnTof1Raw2Digit(kPERIOD, fRunId); //Pass period and run index here or by BmnTof1Raw2Digit->setRun(...)
    //    BmnTof2Raw2Digit *tof700Mapper = new BmnTof2Raw2Digit(fTof700MapFileName);

    UInt_t pedEvCntr = 0; // counter for pedestal events between two spills
    UInt_t pedBunchCntr = 0; // counter for bunches of pedestal events between two spills
    const UInt_t kNBUNCH = 10; // counter for bunches of pedestal events between two spills
    BmnEventType curEventType = kBMNPAYLOAD;
    BmnEventType prevEventType = curEventType;
    list<TClonesArray*> pedListFullSet; //storage for pedestal events from last kNBUNCH bunches
    list<TClonesArray*> pedListCurrBunch; //storage for pedestal events from one bunch
    vector<BmnGemPedestal*> pedVec;

    if (fPedestalRun) {
        gemMapper = new BmnGemRaw2Digit();
        gemMapper->CalcGemPedestals(adc, fRawTree);
    } else {
        gemMapper = new BmnGemRaw2Digit(kPERIOD, fRunId);
        for (Int_t iEv = 0; iEv < fNevents; ++iEv) {
            if (iEv % 1000 == 0) cout << "Decoding event #" << iEv << endl;
            dch->Clear();
            gem->Clear();
            tof400->Clear();
            tof700->Clear();
            t0->Clear();
            bc1->Clear();
            bc2->Clear();
            veto->Clear();
            header->Clear();
            fTimeShifts.clear();
            
            fRawTree->GetEntry(iEv);

            if (FillTimeShiftsMap() == kBMNERROR) {
                cout << "No TimeShiftMap created" << endl;
                continue;
            }

            BmnEventHeader* headDAQ = (BmnEventHeader*) headerDAQ->At(0);
            curEventType = headDAQ->GetType();
            new((*header)[header->GetEntriesFast()]) BmnEventHeader(headDAQ->GetRunId(), headDAQ->GetEventId(), headDAQ->GetEventTimeS(), headDAQ->GetEventTimeNS(), curEventType);

            trigMapper->FillEvent(tdc, t0, bc1, bc2, veto);

            if (curEventType == kBMNPEDESTAL) {
                pedEvCntr++;
                pedListCurrBunch.push_back(adc);
            } else { // payload
                if (prevEventType == kBMNPEDESTAL) {
                    for (Int_t i = 0; i < pedEvCntr; ++i) {
                        pedListFullSet.push_back(pedListCurrBunch.front());
                        if (pedBunchCntr >= kNBUNCH)
                            pedListFullSet.pop_front();
                    }
                    gemMapper->RecalculatePedestals(pedListFullSet, pedVec);
                    pedEvCntr = 0;
                    pedBunchCntr++;
                }
                gemMapper->FillEvent(adc, gem);
            }

            dchMapper->FillEvent(tdc, sync, dch);
            tof400Mapper->FillEvent(tdc, tof400);
            prevEventType = curEventType;

            fDigiTree->Fill();
        }
    }

    fDigiTree->Write();
    fDigiFileOut->Close();
    fRootFileIn->Close();

    delete gemMapper;
    delete dchMapper;
    delete trigMapper;
    delete tof400Mapper;

    delete sync;
    delete adc;
    delete tdc;
    delete gem;
    delete dch;
    delete t0;
    delete bc1;
    delete bc2;
    delete veto;
    delete tof400;
    delete tof700;
    delete header;

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTimeShiftsMap() {

    TriggerMapStructure* map;
    Int_t nEntries = 1;
    UniDbDetectorParameter* mapPar = UniDbDetectorParameter::GetDetectorParameter("T0", "T0_global_mapping", kPERIOD, fRunId);
    if (mapPar != NULL)
        mapPar->GetTriggerMapArray(map, nEntries);
    else {
        cerr << "No TO map found in DB" << endl;
        return kBMNERROR;
    }
    Long64_t t0time = -1;
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        if (syncDig->GetSerial() == map->serial) {
            t0time = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1e9;
            break;
        }
    }

    if (t0time == -1) {
        cerr << "No TO digit found in tree" << endl;
        return kBMNERROR;
    }

    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        Long64_t syncTime = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1e9;
        fTimeShifts.insert(pair<UInt_t, Long64_t>(syncDig->GetSerial(), syncTime - t0time));
    }

    return kBMNSUCCESS;
}