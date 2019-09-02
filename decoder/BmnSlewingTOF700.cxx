
#include "BmnSlewingTOF700.h"

#include <BmnMath.h>
#include <sys/stat.h>
#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */

using namespace std;

class UniDbRun;

BmnSlewingTOF700::BmnSlewingTOF700() {
    fRunId = 0;
    fPeriodId = 0;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = 0;
    fLengthRawFile = 0;
    fCurentPositionRawFile = 0;
    eventHeaderDAQ = NULL;
    eventHeader = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fT0Time = 0.0;
    fRawTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
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
    fRootFileName = "";
    fRawFileName = "";
    fTrigPlaceMapFileName = "";
    fTrigChannelMapFileName = "";
    fTof700MapFileName = "";
    fTof700GeomFileName = "";
    fDat = 0;
    fTrigMapper = NULL;
    fTof700Mapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fBmnSetup = kBMNSETUP;
    fT0Map = NULL;
}

BmnSlewingTOF700::BmnSlewingTOF700(TString file, ULong_t nEvents, ULong_t period) {

    eventHeaderDAQ = NULL;
    eventHeader = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fT0Time = 0.0;
    fRawTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
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
    fRawFileName = file;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = nEvents;
    fPeriodId = period;
    fRunId = GetRunIdFromFile(fRawFileName);
    fRootFileName = Form("bmn_run%04d_raw.root", fRunId);
    fTrigPlaceMapFileName = "";
    fTrigChannelMapFileName = "";
    fTof700MapFileName = "";
    fTof700GeomFileName = "";
    fDat = 0;
    fTrigMapper = NULL;
    fTof700Mapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fBmnSetup = kBMNSETUP;
    fT0Map = NULL;
    //InitMaps();
}

BmnSlewingTOF700::~BmnSlewingTOF700() {
}


BmnStatus BmnSlewingTOF700::FillTimeShiftsMap() {
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


BmnStatus BmnSlewingTOF700::FillTimeShiftsMapNoDB(UInt_t t0serial) {

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

BmnStatus BmnSlewingTOF700::SlewingTOF700Init() {

    fDigiTree = new TTree("cbmsim", "bmn_digit");
    eventHeader = new TClonesArray("BmnEventHeader");
    fDigiTree->Branch("EventHeader", &eventHeader);

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
    fRawTree->SetBranchAddress("HRB", &hrb);
    fRawTree->SetBranchAddress("ADC32", &adc32);
    fRawTree->SetBranchAddress("ADC128", &adc128);
    fRawTree->SetBranchAddress("ADC", &adc);
    fRawTree->SetBranchAddress("Tacquila", &tacquila);

    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

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


    fTof700Mapper = new BmnTof2Raw2DigitNew(fTof700MapFileName, fRootFileName);
    //fTof700Mapper->print();

    return kBMNSUCCESS;
}

BmnStatus BmnSlewingTOF700::SlewingTOF700() {

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


    fTof700Mapper->FitSlewing();

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

BmnStatus BmnSlewingTOF700::PreparationTOF700() {

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

    fTof700Mapper->Equalization0();
    fTof700Mapper->writeSlewingLimits();

    //    fRootFileIn->Close();

    //    delete trigMapper;
    //    delete tof700Mapper;

    return kBMNSUCCESS;
}


Int_t BmnSlewingTOF700::GetRunIdFromFile(TString name) {
    Int_t runId = -1;
    FILE * file = fopen(name.Data(), "rb");
    if (file == NULL) {
        printf("File %s is not open!!!\n", name.Data());
        return -1;
    }
    UInt_t word;
    while (fread(&word, kWORDSIZE1, 1, file)) {
        if (word == kRUNNUMBERSYNC1) {
            fread(&word, kWORDSIZE1, 1, file); //skip word
            fread(&runId, kWORDSIZE1, 1, file);
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

BmnStatus BmnSlewingTOF700::GetT0Info(Double_t& t0time, Double_t &t0width) {
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
