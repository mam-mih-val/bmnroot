#include "BmnRawDataDecoder.h"

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
    fMaxEvent = 0;
    t0 = NULL;
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
    fPedestalRan = kFALSE;
    fRootFileName = "";
    fRawFileName = "";
    fDigiFileName = "";
    fDchMapFileName = "";
    fT0MapFileName = "";
    fGemMapFileName = "";
    fTof400MapFileName = "";
    fTof700MapFileName = "";
}

BmnRawDataDecoder::BmnRawDataDecoder(TString file, ULong_t nEvents) {

    t0 = NULL;
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
    fPedestalRan = kFALSE;
    fRunId = TString(file(fRawFileName.Length() - 8, 3)).Atoi();
    fRootFileName = Form("bmn_run%04d_raw.root", fRunId);
    fDigiFileName = Form("bmn_run%04d_digi.root", fRunId);
    fDchMapFileName = "";
    fT0MapFileName = "";
    fGemMapFileName = "";
    fTof400MapFileName = "";
    fTof700MapFileName = "";
}

BmnRawDataDecoder::~BmnRawDataDecoder() {
}

BmnStatus BmnRawDataDecoder::ConvertRawToRoot() {

    fRawTree = new TTree("BMN_RAW", "BMN_RAW");
    fRawFileIn = fopen(fRawFileName, "rb");
    fRootFileOut = new TFile(fRootFileName, "recreate");

    sync = new TClonesArray("BmnSyncDigit");
    adc = new TClonesArray("BmnADC32Digit");
    tdc = new TClonesArray("BmnTDCDigit");
    msc = new TClonesArray("BmnMSCDigit");

    fRawTree->Branch("SYNC", &sync);
    fRawTree->Branch("ADC", &adc);
    fRawTree->Branch("TDC", &tdc);
    fRawTree->Branch("MSC", &msc);

    UInt_t dat = 0;
    for (;;) {
        if (fMaxEvent > 0 && fNevents == fMaxEvent) break;
        if (fread(&dat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
        if (dat == kSYNC1) { //search for start of event

            // read number of bytes in event
            if (fread(&dat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
            dat = dat / kNBYTESINWORD + 1; // bytes --> words
            if (dat >= 100000) { // what the constant?
                printf("Wrong data size: %d\n", dat);
                return kBMNERROR;
            }

            //read array of current event data and process them
            if (fread(data, kWORDSIZE, dat, fRawFileIn) != dat) return kBMNERROR;
            fEventId = data[0];
            if (fEventId <= 0) continue; // skip bad events (it is possible, but what about 0?) 
            ProcessEvent(data, dat);
            fNevents++;
            fRawTree->Fill();
        }
    }

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

    if (fEventId % 100 == 0) cout << "Converting event #" << d[0] << endl;

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
            //if (modId == kTDC64V && tdcId == 2) channel += 32;
            TClonesArray &ar_tdc = *tdc;
            new(ar_tdc[tdc->GetEntriesFast()]) BmnTDCDigit(serial, modId, slot, (type == 4), channel, tdcId, time);
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

BmnStatus BmnRawDataDecoder::DecodeDataToDigi() {

    fRootFileIn = new TFile(fRootFileName, "READ");
    fRawTree = (TTree *) fRootFileIn->Get("BMN_RAW");
    tdc = new TClonesArray("BmnTDCDigit");
    sync = new TClonesArray("BmnSyncDigit");
    adc = new TClonesArray("BmnADC32Digit");
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("ADC", &adc);

    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    fDigiTree = new TTree("cbmsim", "bmn_digit");

    gem = new TClonesArray("BmnGemStripDigit");
    tof400 = new TClonesArray("BmnTof1Digit");
    tof700 = new TClonesArray("BmnTof2Digit");
    dch = new TClonesArray("BmnDchDigit");
    t0 = new TClonesArray("BmnT0Digit");

    fDigiTree->Branch("run", &fRunId, "bmn_run/I");
    fDigiTree->Branch("event", &fEventId, "bmn_event/I");
    fDigiTree->Branch("time_sec", &fTime_s, "bmn_time_sec/I");
    fDigiTree->Branch("time_ns", &fTime_ns, "bmn_time_ns/I");
    fDigiTree->Branch("BmnT0Digit", &t0);
    fDigiTree->Branch("BmnDchDigit", &dch);
    fDigiTree->Branch("BmnGemStripDigit", &gem);
    fDigiTree->Branch("tof400_digit", &tof400);
    fDigiTree->Branch("tof700_digit", &tof700);

    //    fDchMapFile.open(fDchMapFileName.Data(), ifstream::in);
    //    fDchMapFile.open(fGemMapFileName.Data(), ifstream::in);
    //    fTof400MapFile.open(fTof400MapFileName.Data(), ifstream::in);
    //    fTof700MapFile.open(fTof700MapFileName.Data(), ifstream::in);

    fNevents = fRawTree->GetEntries();

    BmnGemRaw2Digit *gemMapper = new BmnGemRaw2Digit(fGemMapFileName);
    BmnDchRaw2Digit *dchMapper = new BmnDchRaw2Digit(fDchMapFileName);
    BmnT0Raw2Digit *t0Mapper = new BmnT0Raw2Digit(fT0MapFileName);
    BmnTof1Raw2Digit *tof400Mapper = new BmnTof1Raw2Digit(4, 83); //Pass period and run index here or by BmnTof1Raw2Digit->setRun(...)
    //    BmnTof2Raw2Digit *tof700Mapper = new BmnDchRaw2Digit(fTof700MapFileName);

    if (fPedestalRan) {
        gemMapper->CalcGemPedestals(adc, fRawTree);
    } else {
        for (Int_t iEv = 0; iEv < fNevents; ++iEv) {

            if (iEv % 100 == 0) cout << "Decoding event #" << iEv << endl;
            dch->Clear();
            gem->Clear();
            tof400->Clear();
            tof700->Clear();
            t0->Clear();

            fRawTree->GetEntry(iEv);

            //t0Mapper->FillEvent(tdc, t0);
            //gemMapper->FillEvent(adc, gem);
            //dchMapper->FillEvent(tdc, sync, dch);
            tof400Mapper->FillEvent(tdc, tof400);

            fDigiTree->Fill();
        }
    }

    fDigiTree->Write();
    fDigiFileOut->Close();
    fRootFileIn->Close();

    delete sync;
    delete adc;
    delete tdc;
    delete gem;
    delete dch;
    delete t0;
    delete tof400;
    delete tof700;

    return kBMNSUCCESS;
}

//BmnStatus BmnRawDataDecoder::CalcGemPedestals() {
//    ofstream pedFile(Form("%s/input/GEM_pedestals.txt", getenv("VMCWORKDIR")));
//    pedFile << "Serial\tCh_id\tPed\tComMode" << endl;
//    pedFile << "===============================" << endl;
//    const UShort_t nSmpl = 32;
//    const UInt_t nDigs = 640; // 10 ADC x 64 ch
//    ULong_t pedestals[nDigs][nSmpl];
//    for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc)
//        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
//            pedestals[iAdc][iSmpl] = 0;
//
//    for (Int_t iEv = 0; iEv < fNevents; ++iEv) {
//        if (iEv % 1 == 0) cout << "Decoding event #" << iEv << endl;
//        fRawTree->GetEntry(iEv);
//        for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
//            BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
//            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
//                pedestals[iAdc][iSmpl] += ((adcDig->GetValue())[iSmpl] / 16);
//        }
//    }
//    UInt_t comMode = 0;
//    for (Int_t iAdc = 0; iAdc < nDigs; ++iAdc) {
//        BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
//        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
//            pedestals[iAdc][iSmpl] /= fNevents;
//            comMode += pedestals[iAdc][iSmpl];
//        }
//        comMode /= nSmpl;
//        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
//            pedFile << hex << adcDig->GetSerial() << dec << "\t" << adcDig->GetChannel() * nSmpl + iSmpl << "\t" << pedestals[iAdc][iSmpl] << "\t" << comMode << endl;
//    }
//    pedFile.close();
//}
