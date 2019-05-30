#include "BmnDigiConverter.h"

BmnDigiConverter::BmnDigiConverter() :
fEventNo(0),
fGemDigitsIn(nullptr),
fSiDigitsIn(nullptr),
fTOF400DigitsIn(nullptr),
fDchDigitsIn(nullptr),
fGemDigitsOut(nullptr),
fSiDigitsOut(nullptr),
fTOF400DigitsOut(nullptr),
fDchDigitsOut(nullptr),
fCSCDigitsOut(nullptr),
fBC1In(nullptr),
fBC2In(nullptr),
fVetoIn(nullptr),
fBC1Out(nullptr),
fBC2Out(nullptr),
fVetoOut(nullptr),
fBC3In(nullptr),
fBC4In(nullptr),
fX1LIn(nullptr),
fX2LIn(nullptr),
fY1LIn(nullptr),
fY2LIn(nullptr),
fX1RIn(nullptr),
fX2RIn(nullptr),
fY1RIn(nullptr),
fY2RIn(nullptr),
fTQDC_BC1In(nullptr),
fTQDC_BC2In(nullptr),
fTQDC_BC3In(nullptr),
fTQDC_BC4In(nullptr),
fTQDC_VCIn(nullptr),
fBC3Out(nullptr),
fBC4Out(nullptr),
fX1LOut(nullptr),
fX2LOut(nullptr),
fY1LOut(nullptr),
fY2LOut(nullptr),
fX1ROut(nullptr),
fX2ROut(nullptr),
fY1ROut(nullptr),
fY2ROut(nullptr),
fTQDC_BC1Out(nullptr),
fTQDC_BC2Out(nullptr),
fTQDC_BC3Out(nullptr),
fTQDC_BC4Out(nullptr),
fTQDC_VCOut(nullptr),
fSiTrigIn(nullptr),
fBDIn(nullptr),
fSiTrigOut(nullptr),
fBDOut(nullptr),
isTrig(kTRUE),
isMwpc(kTRUE),
isGem(kTRUE),
isSil(kTRUE),
isTof400(kTRUE),
isTof700(kTRUE),
isDch(kTRUE),
isEcal(kTRUE),
isZdc(kTRUE),
isCsc(kTRUE),
isBMN(kFALSE),
isSRC(kFALSE) {
    // Set names for all branches available
    fGemBranchIn = "STRIPGEM";
    fSiBranchIn = "MYSILICON";
    fTOF400BranchIn = "TOF400";
    fDchBranchIn = "DCH";
    fMwpcBranchIn = "MWPC";
    fTOF700BranchIn = "TOF700";
    fECALBranchIn = "ECAL";
    fZDCBranchIn = "ZDC";
    fGemBranchOut = "GEM";
    fSiBranchOut = "SILICON";
    fTOF400BranchOut = "TOF400";
    fDchBranchOut = "DCH";
    fMwpcBranchOut = "MWPC";
    fTOF700BranchOut = "TOF700";
    fECALBranchOut = "ECAL";
    fZDCBranchOut = "ZDC";
    fCSCBranchOut = "CSC";

    fBC1BranchIn = "BC1";
    fBC2BranchIn = "BC2";
    fVetoBranchIn = "VC";
    fBC1BranchOut = "BC1";
    fBC2BranchOut = "BC2";
    fVetoBranchOut = "VC";

    fBC3BranchIn = "BC3";
    fBC4BranchIn = "BC4";
    fX1LBranchIn = "X1_Left";
    fX2LBranchIn = "X2_Left";
    fY1LBranchIn = "Y1_Left";
    fY2LBranchIn = "Y2_Left";
    fX1RBranchIn = "X1_Right";
    fX2RBranchIn = "X2_Right";
    fY1RBranchIn = "Y1_Right";
    fY2RBranchIn = "Y2_Right";
    fTQDC_BC1BranchIn = "TQDC_BC1";
    fTQDC_BC2BranchIn = "TQDC_BC2";
    fTQDC_BC3BranchIn = "TQDC_BC3";
    fTQDC_BC4BranchIn = "TQDC_BC4";
    fTQDC_VCBranchIn = "TQDC_VC";

    fBC3BranchOut = "BC3";
    fBC4BranchOut = "BC4";
    fX1LBranchOut = "X1L";
    fX2LBranchOut = "X2L";
    fY1LBranchOut = "Y1L";
    fY2LBranchOut = "Y2L";
    fX1RBranchOut = "X1R";
    fX2RBranchOut = "X2R";
    fY1RBranchOut = "Y1R";
    fY2RBranchOut = "Y2R";
    fTQDC_BC1BranchOut = "TQDC_BC1";
    fTQDC_BC2BranchOut = "TQDC_BC2";
    fTQDC_BC3BranchOut = "TQDC_BC3";
    fTQDC_BC4BranchOut = "TQDC_BC4";
    fTQDC_VCBranchOut = "TQDC_VC";

    fSiTrigBranchIn = "Si";
    fBDBranchIn = "BD";
    fSiTrigBranchOut = "Si";
    fBDBranchOut = "BD";
}

InitStatus BmnDigiConverter::Init() {
    cout << " BmnDigiConverter::Init() " << endl;

    ioman = FairRootManager::Instance();
    FairEventHeader* h = (FairEventHeader*) ioman->GetObject("EventHeader.");
    h->SetNameTitle("FairEventHeader", "FairEventHeader");

    fBmnHeaderIn = (TClonesArray*) ioman->GetObject("EventHeader");
    if (!fBmnHeaderIn) {
        cout << "Set correct input digi-tree (BMN_DIGIT at present) in $VMCWORKDIR/config/rootmanager.dat" << endl;
        throw;
    }

    // BM@N or SRC
    UInt_t runId = ((BmnEventHeader*) fBmnHeaderIn->UncheckedAt(0))->GetRunId();
    if (runId > 3589) // FIXME!
        isBMN = kTRUE;
    else
        isSRC = kTRUE;

    // get input branches
    fGemDigitsIn = (TClonesArray*) ioman->GetObject(fGemBranchIn.Data());
    fSiDigitsIn = (TClonesArray*) ioman->GetObject(fSiBranchIn.Data());
    fTOF400DigitsIn = (TClonesArray*) ioman->GetObject(fTOF400BranchIn.Data());
    fDchDigitsIn = (TClonesArray*) ioman->GetObject(fDchBranchIn.Data());
    fMwpcDigitsIn = (TClonesArray*) ioman->GetObject(fMwpcBranchIn.Data());
    fTOF700DigitsIn = (TClonesArray*) ioman->GetObject(fTOF700BranchIn.Data());
    fECALDigitsIn = (TClonesArray*) ioman->GetObject(fECALBranchIn.Data());
    fZDCDigitsIn = (TClonesArray*) ioman->GetObject(fZDCBranchIn.Data());

    fBC1In = (TClonesArray*) ioman->GetObject(fBC1BranchIn.Data());
    fBC2In = (TClonesArray*) ioman->GetObject(fBC2BranchIn.Data());
    fVetoIn = (TClonesArray*) ioman->GetObject(fVetoBranchIn.Data());

    fBC3In = (TClonesArray*) ioman->GetObject(fBC3BranchIn.Data());
    fBC4In = (TClonesArray*) ioman->GetObject(fBC4BranchIn.Data());
    fX1LIn = (TClonesArray*) ioman->GetObject(fX1LBranchIn.Data());
    fX2LIn = (TClonesArray*) ioman->GetObject(fX2LBranchIn.Data());
    fY1LIn = (TClonesArray*) ioman->GetObject(fY1LBranchIn.Data());
    fY2LIn = (TClonesArray*) ioman->GetObject(fY2LBranchIn.Data());
    fX1RIn = (TClonesArray*) ioman->GetObject(fX1RBranchIn.Data());
    fX2RIn = (TClonesArray*) ioman->GetObject(fX2RBranchIn.Data());
    fY1RIn = (TClonesArray*) ioman->GetObject(fY1RBranchIn.Data());
    fY2RIn = (TClonesArray*) ioman->GetObject(fY2RBranchIn.Data());
    fTQDC_BC1In = (TClonesArray*) ioman->GetObject(fTQDC_BC1BranchIn.Data());
    fTQDC_BC2In = (TClonesArray*) ioman->GetObject(fTQDC_BC2BranchIn.Data());
    fTQDC_BC3In = (TClonesArray*) ioman->GetObject(fTQDC_BC3BranchIn.Data());
    fTQDC_BC4In = (TClonesArray*) ioman->GetObject(fTQDC_BC4BranchIn.Data());
    fTQDC_VCIn = (TClonesArray*) ioman->GetObject(fTQDC_VCBranchIn.Data());

    fSiTrigIn = (TClonesArray*) ioman->GetObject(fSiTrigBranchIn.Data());
    fBDIn = (TClonesArray*) ioman->GetObject(fBDBranchIn.Data());

    // create output branches
    fBmnHeaderOut = new TClonesArray("BmnEventHeader");
    ioman->Register("EventHeader", "EventHeader_", fBmnHeaderOut, kTRUE);

    Bool_t isWriteTrig = isTrig ? kTRUE : kFALSE;

    fGemDigitsOut = new TClonesArray("BmnGemStripDigit");
    fSiDigitsOut = new TClonesArray("BmnSiliconDigit");
    fTOF400DigitsOut = new TClonesArray("BmnTof1Digit");
    fDchDigitsOut = new TClonesArray("BmnDchDigit");
    fMwpcDigitsOut = new TClonesArray("BmnMwpcDigit");
    fTOF700DigitsOut = new TClonesArray("BmnTof2Digit");
    fECALDigitsOut = new TClonesArray("BmnECALDigit");
    fZDCDigitsOut = new TClonesArray("BmnZDCDigit");
    fCSCDigitsOut = new TClonesArray("BmnCSCDigit");

    fBC1Out = new TClonesArray("BmnTrigDigit");
    fBC2Out = new TClonesArray("BmnTrigDigit");
    fVetoOut = new TClonesArray("BmnTrigDigit");

    ioman->Register(fGemBranchOut.Data(), "GEM_", fGemDigitsOut, isGem ? kTRUE : kFALSE);
    ioman->Register(fSiBranchOut.Data(), "SILICON_", fSiDigitsOut, isSil ? kTRUE : kFALSE);
    ioman->Register(fTOF400BranchOut.Data(), "TOF400_", fTOF400DigitsOut, isTof400 ? kTRUE : kFALSE);
    ioman->Register(fDchBranchOut.Data(), "DCH_", fDchDigitsOut, isDch ? kTRUE : kFALSE);
    ioman->Register(fMwpcBranchOut.Data(), "MWPC_", fMwpcDigitsOut, isMwpc ? kTRUE : kFALSE);
    ioman->Register(fTOF700BranchOut.Data(), "TOF700_", fTOF700DigitsOut, isTof700 ? kTRUE : kFALSE);
    ioman->Register(fECALBranchOut.Data(), "ECAL_", fECALDigitsOut, isEcal ? kTRUE : kFALSE);
    ioman->Register(fZDCBranchOut.Data(), "ZDC_", fZDCDigitsOut, isZdc ? kTRUE : kFALSE);
    ioman->Register(fCSCBranchOut.Data(), "CSC_", fCSCDigitsOut, isCsc ? kTRUE : kFALSE);

    ioman->Register(fBC1BranchOut.Data(), "BC1_", fBC1Out, isWriteTrig);
    ioman->Register(fBC2BranchOut.Data(), "BC2_", fBC2Out, isWriteTrig);
    ioman->Register(fVetoBranchOut.Data(), "VETO_", fVetoOut, isWriteTrig);

    if (isBMN) {
        fSiTrigOut = new TClonesArray("BmnTrigDigit");
        fBDOut = new TClonesArray("BmnTrigDigit");

        ioman->Register(fSiTrigBranchOut.Data(), "SI_", fSiTrigOut, isWriteTrig);
        ioman->Register(fBDBranchOut.Data(), "BD_", fBDOut, isWriteTrig);
    }

    if (isSRC) {
        fBC3Out = new TClonesArray("BmnTrigDigit");
        fBC4Out = new TClonesArray("BmnTrigDigit");
        fX1LOut = new TClonesArray("BmnTrigDigit");
        fX2LOut = new TClonesArray("BmnTrigDigit");
        fY1LOut = new TClonesArray("BmnTrigDigit");
        fY2LOut = new TClonesArray("BmnTrigDigit");
        fX1ROut = new TClonesArray("BmnTrigDigit");
        fX2ROut = new TClonesArray("BmnTrigDigit");
        fY1ROut = new TClonesArray("BmnTrigDigit");
        fY2ROut = new TClonesArray("BmnTrigDigit");
        fTQDC_BC1Out = new TClonesArray("BmnTrigWaveDigit");
        fTQDC_BC2Out = new TClonesArray("BmnTrigWaveDigit");
        fTQDC_BC3Out = new TClonesArray("BmnTrigWaveDigit");
        fTQDC_BC4Out = new TClonesArray("BmnTrigWaveDigit");
        fTQDC_VCOut = new TClonesArray("BmnTrigWaveDigit");

        ioman->Register(fBC3BranchOut.Data(), "BC3_", fBC3Out, isWriteTrig);
        ioman->Register(fBC4BranchOut.Data(), "BC4_", fBC4Out, isWriteTrig);
        ioman->Register(fX1LBranchOut.Data(), "X1L_", fX1LOut, isWriteTrig);
        ioman->Register(fX2LBranchOut.Data(), "X2L_", fX2LOut, isWriteTrig);
        ioman->Register(fY1LBranchOut.Data(), "Y1L_", fY1LOut, isWriteTrig);
        ioman->Register(fY2LBranchOut.Data(), "Y2L_", fY2LOut, isWriteTrig);
        ioman->Register(fX1RBranchOut.Data(), "X1R_", fX1ROut, isWriteTrig);
        ioman->Register(fX2RBranchOut.Data(), "X2R_", fX2ROut, isWriteTrig);
        ioman->Register(fY1RBranchOut.Data(), "Y1R_", fY1ROut, isWriteTrig);
        ioman->Register(fY2RBranchOut.Data(), "Y2R_", fY2ROut, isWriteTrig);
        ioman->Register(fTQDC_BC1BranchOut.Data(), "TQDC_BC1_", fTQDC_BC1Out, isWriteTrig);
        ioman->Register(fTQDC_BC2BranchOut.Data(), "TQDC_BC2_", fTQDC_BC2Out, isWriteTrig);
        ioman->Register(fTQDC_BC3BranchOut.Data(), "TQDC_BC3_", fTQDC_BC3Out, isWriteTrig);
        ioman->Register(fTQDC_BC4BranchOut.Data(), "TQDC_BC4_", fTQDC_BC4Out, isWriteTrig);
        ioman->Register(fTQDC_VCBranchOut.Data(), "TQDC_VC_", fTQDC_VCOut, isWriteTrig);
    }

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString confSi = isBMN ? "SiliconRunSpring2018.xml" : isSRC ? "SiliconRunSRCSpring2018.xml" : "";
    TString confGem = isBMN ? "GemRunSpring2018.xml" : isSRC ? "GemRunSRCSpring2018.xml" : "";
    TString confCsc = "CSCRunSpring2018.xml";

    // SI
    TString gPathSiliconConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    fDetectorSI = new BmnSiliconStationSet(gPathSiliconConfig + confSi);

    // GEM
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    fDetectorGEM = new BmnGemStripStationSet(gPathGemConfig + confGem);

    // CSC
    TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
    fDetectorCSC = new BmnCSCStationSet(gPathCscConfig + confCsc);

    Int_t* statsGem = new Int_t[fDetectorGEM->GetNStations()];
    Int_t* statsSil = new Int_t[fDetectorSI->GetNStations()];

    Int_t* statsGemPermut = new Int_t[fDetectorGEM->GetNStations()];
    Int_t* statsSilPermut = new Int_t[fDetectorSI->GetNStations()];

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
        statsGem[iStat] = -1;
        statsGemPermut[iStat] = -1;
    }

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
        statsSil[iStat] = -1;
        statsSilPermut[iStat] = -1;
    }

    Run7(statsGem, statsSil, statsGemPermut, statsSilPermut); // FIXME!

    if (isGem)
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            fGemStats[statsGem[iStat]] = statsGemPermut[iStat];

    if (isSil)
        for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
            fSilStats[statsSil[iStat]] = statsSilPermut[iStat];

    if (isTrig) {
        // Common triggers
        fTriggers[fBC1In] = fBC1Out;
        fTriggers[fBC2In] = fBC2Out;
        fTriggers[fVetoIn] = fVetoOut;

        if (isBMN) {
            fTriggers[fSiTrigIn] = fSiTrigOut;
            fTriggers[fBDIn] = fBDOut;
        }

        if (isSRC) {
            fTriggers[fBC3In] = fBC3Out;
            fTriggers[fBC4In] = fBC4Out;
            fTriggers[fX1LIn] = fX1LOut;
            fTriggers[fX2LIn] = fX2LOut;
            fTriggers[fY1LIn] = fY1LOut;
            fTriggers[fY2LIn] = fY2LOut;
            fTriggers[fX1RIn] = fX1ROut;
            fTriggers[fX2RIn] = fX2ROut;
            fTriggers[fY1RIn] = fY1ROut;
            fTriggers[fY2RIn] = fY2ROut;
            fTriggers[fTQDC_BC1In] = fTQDC_BC1Out;
            fTriggers[fTQDC_BC2In] = fTQDC_BC2Out;
            fTriggers[fTQDC_BC3In] = fTQDC_BC3Out;
            fTriggers[fTQDC_BC4In] = fTQDC_BC4Out;
            fTriggers[fTQDC_VCIn] = fTQDC_VCOut;
        }
    }

    delete statsGem;
    delete statsGemPermut;
    delete statsSil;
    delete statsSilPermut;

    return kSUCCESS;
}

void BmnDigiConverter::Run7(Int_t* statsGem, Int_t* statsSil, Int_t* statsGemPermut, Int_t* statsSilPermut) {
    // Setup is valid for two modes (BM@N and SRC)
    // To be moved to the UniDb
    if (isBMN) {
        if (isGem) {
            statsGem[0] = 1; // MK-numeration
            statsGem[1] = 2; // MK-numeration
            statsGem[2] = 4; // MK-numeration
            statsGem[3] = 5; // MK-numeration
            statsGem[4] = 6; // MK-numeration
            statsGem[5] = 7; // MK-numeration

            statsGemPermut[0] = 0;
            statsGemPermut[1] = 1;
            statsGemPermut[2] = 2;
            statsGemPermut[3] = 3;
            statsGemPermut[4] = 4;
            statsGemPermut[5] = 5;
        }
        if (isSil) {
            statsSil[0] = 1; // MK-numeration
            statsSil[1] = 2; // MK-numeration
            statsSil[2] = 3; // MK-numeration

            statsSilPermut[0] = 0;
            statsSilPermut[1] = 1;
            statsSilPermut[2] = 2;

        }
    } else if (isSRC) {
        if (isGem) {
            statsGem[0] = 1; // MK-numeration
            statsGem[1] = 2; // MK-numeration
            statsGem[2] = 3; // MK-numeration
            statsGem[3] = 4; // MK-numeration
            statsGem[4] = 5; // MK-numeration
            statsGem[5] = 6; // MK-numeration
            statsGem[6] = 7; // MK-numeration
            statsGem[7] = 8; // MK-numeration
            statsGem[8] = 9; // MK-numeration
            statsGem[9] = 10; // MK-numeration

            statsGemPermut[0] = 0;
            statsGemPermut[1] = 1;
            statsGemPermut[2] = 2;
            statsGemPermut[3] = 3;
            statsGemPermut[4] = 4;
            statsGemPermut[5] = 5;
            statsGemPermut[6] = 6;
            statsGemPermut[7] = 7;
            statsGemPermut[8] = 8;
            statsGemPermut[9] = 9;
        }

        if (isSil) {
            statsSil[0] = 1; // MK-numeration
            statsSil[1] = 2; // MK-numeration
            statsSil[2] = 3; // MK-numeration

            statsSilPermut[0] = 0;
            statsSilPermut[1] = 1;
            statsSilPermut[2] = 2;
        }
    } else {
        cout << "Configuration not defined!" << endl;
        throw;
    }
}

void BmnDigiConverter::Exec(Option_t* opt) {
    if (fEventNo % 1000 == 0)
        cout << "Ev# " << fEventNo << endl;

    // Event Header
    BmnEventHeader* evHeaderIn = (BmnEventHeader*) fBmnHeaderIn->UncheckedAt(0);
    UInt_t runID = evHeaderIn->GetRunId();
    UInt_t eventID = evHeaderIn->GetEventId();
    TTimeStamp eventTime = evHeaderIn->GetEventTime();
    BmnEventType eventType = evHeaderIn->GetEventType();
    BmnTrigInfo* trigInfo = evHeaderIn->GetTrigInfo(); // cout << trigType << endl;
    Bool_t tripWord = evHeaderIn->GetTripWord();

    fBmnHeaderOut->Delete();

    new((*fBmnHeaderOut)[fBmnHeaderOut->GetEntriesFast()]) BmnEventHeader(runID, eventID, eventTime, eventType, tripWord, trigInfo);

    fGemDigitsOut->Delete();
    fSiDigitsOut->Delete();
    fTOF400DigitsOut->Delete();
    fDchDigitsOut->Delete();
    fMwpcDigitsOut->Delete();
    fTOF700DigitsOut->Delete();
    fECALDigitsOut->Delete();
    fZDCDigitsOut->Delete();
    fCSCDigitsOut->Delete();

    // Clear array with common triggers
    fBC1Out->Delete();
    fBC2Out->Delete();
    fVetoOut->Delete();

    // Clear BM@N triggers
    if (isBMN) {
        fSiTrigOut->Delete();
        fBDOut->Delete();
    }

    // Clear SRC triggers
    for (auto &it : fTriggers)
        it.second->Delete();

    // CSC
    if (isCsc && fGemDigitsIn) // NOTE: Csc is written to GEM digi array by BmnGemStripDigit !!!
        for (UInt_t iDigi = 0; iDigi < fGemDigitsIn->GetEntriesFast(); iDigi++) {
            BmnGemStripDigit* cscDig = (BmnGemStripDigit*) fGemDigitsIn->UncheckedAt(iDigi);
            Int_t stat = cscDig->GetStation();

            if ((isBMN && stat != 8) || (isSRC && stat != 11))
                continue;

            Int_t strip = cscDig->GetStripNumber() - 1; // strips should be enumerated from zero
            Double_t signal = cscDig->GetStripSignal();

            // Modules should be permutated! (mod0 <--> mod1)
            if (cscDig->GetModule() == 0) {
                cscDig->SetModule(1);
                // Layers should be permutated in mod0 only
                if (cscDig->GetStripLayer() == 2)
                    cscDig->SetStripLayer(0);
                else if (cscDig->GetStripLayer() == 3)
                    cscDig->SetStripLayer(1);
                else if (cscDig->GetStripLayer() == 0)
                    cscDig->SetStripLayer(2);
                else if (cscDig->GetStripLayer() == 1)
                    cscDig->SetStripLayer(3);
                else {
                    cout << "Something went wrong!" << endl;
                    throw;
                }
            }

            else if (cscDig->GetModule() == 1)
                cscDig->SetModule(0);

            else {
                cout << "Something went wrong!" << endl;
                throw;
            }

            new((*fCSCDigitsOut)[fCSCDigitsOut->GetEntriesFast()]) BmnCSCDigit(0, cscDig->GetModule(), cscDig->GetStripLayer(), strip, signal);
        }

    // GEM
    if (isGem && fGemDigitsIn)
        for (UInt_t iDigi = 0; iDigi < fGemDigitsIn->GetEntriesFast(); iDigi++) {
            BmnGemStripDigit* gemDig = (BmnGemStripDigit*) fGemDigitsIn->UncheckedAt(iDigi);

            Int_t stat = gemDig->GetStation();
            Int_t strip = gemDig->GetStripNumber() - 1; // strips should be enumerated from zero
            Double_t signal = gemDig->GetStripSignal();

            if (isBMN) {
                // Gem stat 3 and Csc should be omitted
                if (stat == 8 || stat == 3)
                    continue;
                gemDig->SetStation(GemStatPermutation(stat));
                stat = gemDig->GetStation(); // Stations permuted already!

                if (stat == 0 || stat == 3 || stat == 5) {
                    if (gemDig->GetModule() == 0)
                        gemDig->SetModule(1);
                    else if (gemDig->GetModule() == 1)
                        gemDig->SetModule(0);
                    else {
                        cout << "Something went wrong!" << endl;
                        throw;
                    }
                }
            }

            if (isSRC) {
                // CSC should be omitted
                if (stat == 11)
                    continue;

                gemDig->SetStation(GemStatPermutation(stat));
                stat = gemDig->GetStation();

                if (stat == 5 || stat == 7 || stat == 9) {
                    if (gemDig->GetModule() == 0)
                        gemDig->SetModule(1);
                    else if (gemDig->GetModule() == 1)
                        gemDig->SetModule(0);
                    else {
                        cout << "Something went wrong!" << endl;
                        throw;
                    }
                }
            }

            new((*fGemDigitsOut)[fGemDigitsOut->GetEntriesFast()]) BmnGemStripDigit(gemDig->GetStation(),
                    gemDig->GetModule(),
                    gemDig->GetStripLayer(),
                    strip, signal);
        }

    // SILICON
    if (isSil && fSiDigitsIn)
        for (UInt_t iDigi = 0; iDigi < fSiDigitsIn->GetEntriesFast(); iDigi++) {
            BmnSiliconDigit* siDig = (BmnSiliconDigit*) fSiDigitsIn->UncheckedAt(iDigi);

            Int_t stat = siDig->GetStation();
            siDig->SetStation(SiliconStatPermutation(stat));
            stat = siDig->GetStation(); // Stations permuted already!

            Int_t strip = siDig->GetStripNumber() - 1; // strips should be enumerated from zero
            Int_t signal = siDig->GetStripSignal();

            Int_t mod = siDig->GetModule();
            Int_t layer = siDig->GetStripLayer();

            new((*fSiDigitsOut)[fSiDigitsOut->GetEntriesFast()]) BmnSiliconDigit(stat, mod, layer, strip, signal);
        }

    // TRIGGERS (BM@N and SRC)
    if (isTrig)
        ConvertTriggers(fTriggers);

    // TOF
    if (isTof400 && fTOF400DigitsIn)
        for (UInt_t iDigi = 0; iDigi < fTOF400DigitsIn->GetEntriesFast(); iDigi++) {
            BmnTof1Digit* tofDig = (BmnTof1Digit*) fTOF400DigitsIn->UncheckedAt(iDigi);
            new((*fTOF400DigitsOut)[fTOF400DigitsOut->GetEntriesFast()]) BmnTof1Digit(tofDig->GetPlane(), tofDig->GetStrip(),
                    tofDig->GetSide(), tofDig->GetTime(), tofDig->GetAmplitude());
        }

    // DCH
    if (isDch && fDchDigitsIn)
        for (UInt_t iDigi = 0; iDigi < fDchDigitsIn->GetEntriesFast(); iDigi++) {
            BmnDchDigit* dchDig = (BmnDchDigit*) fDchDigitsIn->UncheckedAt(iDigi);
            new((*fDchDigitsOut)[fDchDigitsOut->GetEntriesFast()]) BmnDchDigit(dchDig->GetPlane(), dchDig->GetWireNumber(), dchDig->GetTime(), dchDig->GetRefId());
        }

    // MWPC
    if (isMwpc && fMwpcDigitsIn)
        for (UInt_t iDigi = 0; iDigi < fMwpcDigitsIn->GetEntriesFast(); iDigi++) {
            BmnMwpcDigit* mwpcDig = (BmnMwpcDigit*) fMwpcDigitsIn->UncheckedAt(iDigi);
            new((*fMwpcDigitsOut)[fMwpcDigitsOut->GetEntriesFast()]) BmnMwpcDigit(mwpcDig->GetStation(), mwpcDig->GetPlane(), mwpcDig->GetWireNumber(), mwpcDig->GetTime());
        }

    // TOF700
    if (isTof700 && fTOF700DigitsIn)
        for (UInt_t iDigi = 0; iDigi < fTOF700DigitsIn->GetEntriesFast(); iDigi++) {
            BmnTof2Digit* tofDig = (BmnTof2Digit*) fTOF700DigitsIn->UncheckedAt(iDigi);
            new((*fTOF700DigitsOut)[fTOF700DigitsOut->GetEntriesFast()]) BmnTof2Digit(tofDig->GetPlane(), tofDig->GetStrip(), tofDig->GetTime(), tofDig->GetAmplitude(), tofDig->GetDiff());
        }

    // ECAL
    if (isEcal && fECALDigitsIn)
        for (UInt_t iDigi = 0; iDigi < fECALDigitsIn->GetEntriesFast(); iDigi++) {
            BmnECALDigit* ecalDig = (BmnECALDigit*) fECALDigitsIn->UncheckedAt(iDigi);
            new((*fECALDigitsOut)[fECALDigitsOut->GetEntriesFast()]) BmnECALDigit(ecalDig->GetIX(), ecalDig->GetIY(), ecalDig->GetX(), ecalDig->GetY(), ecalDig->GetSize(),
                    ecalDig->GetChannel(), ecalDig->GetAmp());
        }

    // ZDC
    if (isZdc && fZDCDigitsIn)
        for (UInt_t iDigi = 0; iDigi < fZDCDigitsIn->GetEntriesFast(); iDigi++) {
            BmnZDCDigit* zdcDig = (BmnZDCDigit*) fZDCDigitsIn->UncheckedAt(iDigi);
            new((*fZDCDigitsOut)[fZDCDigitsOut->GetEntriesFast()]) BmnZDCDigit(zdcDig->GetIX(), zdcDig->GetIY(), zdcDig->GetX(), zdcDig->GetY(), zdcDig->GetSize(),
                    zdcDig->GetChannel(), zdcDig->GetAmp());
        }

    fEventNo++;
}

void BmnDigiConverter::Finish() {
    ioman->GetOutTree()->SetObject("bmndata", "digit");
    gDirectory->Delete(TString::Format("%s;*", "BMN_DIGIT"));

    delete fDetectorSI;
    delete fDetectorGEM;
}

void BmnDigiConverter::ConvertTriggers(map <TClonesArray*, TClonesArray*> trig) {
    for (auto &it : trig) {
        TClonesArray* in = it.first;
        TClonesArray* out = it.second;

        TString arrName = in->GetName();
        Bool_t isTqdcDig = (arrName.Contains("Wave")) ? kTRUE : kFALSE;

        for (UInt_t iDigi = 0; iDigi < in->GetEntriesFast(); iDigi++) {
            if (isTqdcDig) {
                BmnTrigWaveDigit* dig = (BmnTrigWaveDigit*) in->UncheckedAt(iDigi);
                new((*out)[out->GetEntriesFast()]) BmnTrigWaveDigit(dig->GetMod(), dig->GetShortValue(), dig->GetNSamples(),
                        dig->GetTrigTimestamp(), dig->GetAdcTimestamp(), dig->GetTime());
            } else {
                BmnTrigDigit* dig = (BmnTrigDigit*) in->UncheckedAt(iDigi);
                new((*out)[out->GetEntriesFast()]) BmnTrigDigit(dig->GetMod(), dig->GetTime(), dig->GetAmp());
            }
        }
    }
}

BmnDigiConverter::~BmnDigiConverter() {



}



