#include "BmnQaOffline.h"

Int_t BmnQaOffline::fCurrentEvent = 0;

BmnQaOffline::BmnQaOffline() :
fGemDigits(nullptr),
fCscDigits(nullptr),
fSiDigits(nullptr),
fTOF400Digits(nullptr),
fDchDigits(nullptr),
fMwpcDigits(nullptr),
fTOF700Digits(nullptr),
fECALDigits(nullptr),
fZDCDigits(nullptr),
fBC1Digits(nullptr),
fBC2Digits(nullptr),
fVetoDigits(nullptr),
fBC3Digits(nullptr),
fBC4Digits(nullptr),
fX1LDigits(nullptr),
fX2LDigits(nullptr),
fY1LDigits(nullptr),
fY2LDigits(nullptr),
fX1RDigits(nullptr),
fX2RDigits(nullptr),
fY1RDigits(nullptr),
fY2RDigits(nullptr),
fTQDC_BC1Digits(nullptr),
fTQDC_BC2Digits(nullptr),
fTQDC_BC3Digits(nullptr),
fTQDC_BC4Digits(nullptr),
fTQDC_VetoDigits(nullptr),
fSiTrigDigits(nullptr),
fBDDigits(nullptr),
fBmnHeader(nullptr),
fDirectories(nullptr),
fDetectors(nullptr),
fTriggers(nullptr) {
    fGem = "GEM";
    fCsc = "CSC";
    fSi = "SILICON";
    fTOF400 = "TOF400";
    fDch = "DCH";
    fMwpc = "MWPC";
    fTOF700 = "TOF700";
    fECAL = "ECAL";
    fZDC = "ZDC";

    fBC1 = "BC1";
    fBC2 = "BC2";
    fVeto = "VETO";

    fBC3 = "BC3";
    fBC4 = "BC4";
    fX1L = "X1L";
    fX2L = "X2L";
    fY1L = "Y1L";
    fY2L = "Y2L";
    fX1R = "X1R";
    fX2R = "X2R";
    fY1R = "Y1R";
    fY2R = "Y2R";

    fTQDC_BC1 = "TQDC_BC1";
    fTQDC_BC2 = "TQDC_BC2";
    fTQDC_BC3 = "TQDC_BC3";
    fTQDC_BC4 = "TQDC_BC4";
    fTQDC_Veto = "TQDC_VC";

    fSiTrig = "SI";
    fBD = "BD";

    // fNEvents = nEvents;
}

InitStatus BmnQaOffline::Init() {
    cout << " BmnQaOffline::Init() " << endl;

    ioman = FairRootManager::Instance();

    fBmnHeader = (BmnEventHeader*) ioman->GetObject("BmnEventHeader.");

    fGemDigits = (TClonesArray*) ioman->GetObject(fGem.Data());
    fCscDigits = (TClonesArray*) ioman->GetObject(fCsc.Data());
    fSiDigits = (TClonesArray*) ioman->GetObject(fSi.Data());
    fTOF400Digits = (TClonesArray*) ioman->GetObject(fTOF400.Data());
    fDchDigits = (TClonesArray*) ioman->GetObject(fDch.Data());
    fMwpcDigits = (TClonesArray*) ioman->GetObject(fMwpc.Data());
    fTOF700Digits = (TClonesArray*) ioman->GetObject(fTOF700.Data());
    fECALDigits = (TClonesArray*) ioman->GetObject(fECAL.Data());
    fZDCDigits = (TClonesArray*) ioman->GetObject(fZDC.Data());

    fBC1Digits = (TClonesArray*) ioman->GetObject(fBC1.Data());
    fBC2Digits = (TClonesArray*) ioman->GetObject(fBC2.Data());
    fVetoDigits = (TClonesArray*) ioman->GetObject(fVeto.Data());

    fBC3Digits = (TClonesArray*) ioman->GetObject(fBC3.Data());
    fBC4Digits = (TClonesArray*) ioman->GetObject(fBC4.Data());
    fX1LDigits = (TClonesArray*) ioman->GetObject(fX1L.Data());
    fX2LDigits = (TClonesArray*) ioman->GetObject(fX2L.Data());
    fY1LDigits = (TClonesArray*) ioman->GetObject(fY1L.Data());
    fY2LDigits = (TClonesArray*) ioman->GetObject(fY2L.Data());
    fX1RDigits = (TClonesArray*) ioman->GetObject(fX1R.Data());
    fX2RDigits = (TClonesArray*) ioman->GetObject(fX2R.Data());
    fY1RDigits = (TClonesArray*) ioman->GetObject(fY1R.Data());
    fY2RDigits = (TClonesArray*) ioman->GetObject(fY2R.Data());

    fTQDC_BC1Digits = (TClonesArray*) ioman->GetObject(fTQDC_BC1.Data());
    fTQDC_BC2Digits = (TClonesArray*) ioman->GetObject(fTQDC_BC2.Data());
    fTQDC_BC3Digits = (TClonesArray*) ioman->GetObject(fTQDC_BC3.Data());
    fTQDC_BC4Digits = (TClonesArray*) ioman->GetObject(fTQDC_BC4.Data());
    fTQDC_VetoDigits = (TClonesArray*) ioman->GetObject(fTQDC_Veto.Data());

    fSiTrigDigits = (TClonesArray*) ioman->GetObject(fSiTrig.Data());
    fBDDigits = (TClonesArray*) ioman->GetObject(fBD.Data());

    // Call detector classes and its histo classes implicitly
    // Strip detectors
    gem = new BmnCoordinateDetQa("GEM", fBmnHeader->GetRunId());
    silicon = new BmnCoordinateDetQa("SILICON", fBmnHeader->GetRunId());
    csc = new BmnCoordinateDetQa("CSC", fBmnHeader->GetRunId());

    // Time detectors
    tof400 = new BmnTimeDetQa("TOF400");
    tof700 = new BmnTimeDetQa("TOF700");
    dch = new BmnTimeDetQa("DCH");
    mwpc = new BmnTimeDetQa("MWPC");

    // Calorim. detectors
    ecal = new BmnCalorimeterDetQa("ECAL");
    zdc = new BmnCalorimeterDetQa("ZDC");

    // Triggers   
    const Int_t nTrigDets = fBmnHeader->GetRunId() > 3589 ? 6 : 18; // FIXME (BM@N or SRC)
    TString trigsBMN[] = {"BC1", "BC2", "BC3", "VETO", "SI", "BD"};
    TClonesArray * _trigsBMN[] = {fBC1Digits, fBC2Digits, fBC3Digits, fVetoDigits, fSiTrigDigits, fBDDigits};

    TString trigsSRC[] = {"BC1", "BC2", "BC3", "BC4", "X1L", "X1R", "X2L", "X2R", "Y1L", "Y1R", "Y2L", "Y2R", "VC", "TQDC_BC1", "TQDC_BC2", "TQDC_BC3", "TQDC_BC4", "TQDC_VC"};
    TClonesArray * _trigsSRC[] = {fBC1Digits, fBC2Digits, fBC3Digits, fBC4Digits,
        fX1LDigits, fX1RDigits, fX2LDigits, fX2RDigits,
        fY1LDigits, fY1RDigits, fY2LDigits, fY2RDigits,
        fVetoDigits,
        fTQDC_BC1Digits, fTQDC_BC2Digits, fTQDC_BC3Digits, fTQDC_BC4Digits, fTQDC_VetoDigits};

    fTriggers = new TClonesArray*[nTrigDets];

    for (Int_t iDet = 0; iDet < nTrigDets; iDet++) {
        fTriggers[iDet] = (nTrigDets == 6) ? _trigsBMN[iDet] : _trigsSRC[iDet];
        fTrigCorr[fTriggers[iDet]] = (nTrigDets == 6) ? trigsBMN[iDet] : trigsSRC[iDet];
    }

    if (fTrigCorr.size() != 0)
        triggers = new BmnTrigDetQa(fTrigCorr);

    return kSUCCESS;
}

void BmnQaOffline::Finish() {
    const Int_t nDets = 10;

    fDetectors = new TString[nDets];
    fDetectors[0] = "GEM";
    fDetectors[1] = "SILICON";
    fDetectors[2] = "CSC";

    fDetectors[3] = "TOF400";
    fDetectors[4] = "TOF700";
    fDetectors[5] = "DCH";
    fDetectors[6] = "MWPC";

    fDetectors[7] = "ECAL";
    fDetectors[8] = "ZDC";

    fDetectors[9] = "TRIGGERS";

    BmnQaHistoManager * managers[nDets] = {gem->GetManager(), silicon->GetManager(), csc->GetManager(),
        tof400->GetManager(), tof700->GetManager(), dch->GetManager(), mwpc->GetManager(),
        ecal->GetManager(), zdc->GetManager(), triggers->GetManager()};

    fDirectories = new TDirectory*[nDets];
    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        fDirectories[iDet] = ioman->GetOutFile()->mkdir(fDetectors[iDet].Data());
        fDirectories[iDet]->cd();
        managers[iDet]->WriteToFile();
    }

    // Delete detector classes and its histo classes
    delete gem;
    delete silicon;
    delete csc;
    delete tof400;
    delete tof700;
    delete dch;
    delete mwpc;
    delete ecal;
    delete zdc;
    delete triggers;

    delete fTriggers;
}

void BmnQaOffline::Exec(Option_t* opt) {
    fCurrentEvent++;
    if (fCurrentEvent % 1000 == 0)
        cout << "Event# = " << fCurrentEvent << endl;

    GEM();
    CSC();
    SILICON();

    TOF400();
    TOF700();
    DCH();
    MWPC();

    ECAL();
    ZDC();

    TRIGGERS();
}

void BmnQaOffline::GEM() {
    GetDistributionOfFiredStrips <BmnGemStripDigit> (fGemDigits, gem, "GEM");


}

void BmnQaOffline::SILICON() {
    GetDistributionOfFiredStrips <BmnSiliconDigit> (fSiDigits, silicon, "SILICON");


}

void BmnQaOffline::CSC() {
    GetDistributionOfFiredStrips <BmnCSCDigit> (fCscDigits, csc, "CSC");


}

void BmnQaOffline::TOF400() {
    GetCommonInfo <BmnTof1Digit> (fTOF400Digits, tof400, "TOF400");
    GetTofInfo <BmnTof1Digit> (fTOF400Digits, tof400, "TOF400");


}

void BmnQaOffline::TOF700() {
    GetCommonInfo <BmnTof2Digit> (fTOF700Digits, tof700, "TOF700");
    GetTofInfo <BmnTof2Digit> (fTOF700Digits, tof700, "TOF700");


}

void BmnQaOffline::DCH() {
    GetCommonInfo <BmnDchDigit> (fDchDigits, dch, "DCH");
    GetMwpcDchInfo <BmnDchDigit> (fDchDigits, dch, "DCH");


}

void BmnQaOffline::MWPC() {
    GetCommonInfo <BmnMwpcDigit> (fMwpcDigits, mwpc, "MWPC");
    GetMwpcDchInfo <BmnMwpcDigit> (fMwpcDigits, mwpc, "MWPC");


}

void BmnQaOffline::ECAL() {
    GetCommonInfo <BmnECALDigit> (fECALDigits, ecal, "ECAL");


}

void BmnQaOffline::ZDC() {
    GetCommonInfo <BmnZDCDigit> (fZDCDigits, zdc, "ZDC");


}

void BmnQaOffline::TRIGGERS() {
    for (auto it : fTrigCorr)
        GetCommonInfo <BmnTrigDigit> (it.first, triggers, it.second);
}



ClassImp(BmnQaOffline);