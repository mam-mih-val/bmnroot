#include "BmnDigiConverter.h"

BmnDigiConverter::BmnDigiConverter() :
fEventNo(0),
fGemDigitsIn(NULL),
fSiDigitsIn(NULL),
fGemDigitsOut(NULL),
fSiDigitsOut(NULL),
isTrig(kTRUE),
isGem(kTRUE),
isSil(kTRUE) {
    fGemBranchIn = "STRIPGEM";
    fSiBranchIn = "MYSILICON";

    fGemBranchOut = "GEM";
    fSiBranchOut = "SILICON";

    fSiBranch = "Si";
    fVetoBranch = "VC";
    fBC1Branch = "BC1";
    fBC2Branch = "BC2";
    fBDBranch = "BD";
}

InitStatus BmnDigiConverter::Init() {
    cout << " BmnDigiConverter::Init() " << endl;

    ioman = FairRootManager::Instance();
    FairEventHeader* h = (FairEventHeader*) ioman->GetObject("EventHeader.");
    h->SetNameTitle("FairEventHeader", "FairEventHeader");

    fBmnHeaderIn = (TClonesArray*) ioman->GetObject("EventHeader");

    fSiIn = (TClonesArray*) ioman->GetObject(fSiBranch.Data());
    fVetoIn = (TClonesArray*) ioman->GetObject(fVetoBranch.Data());
    fBC1In = (TClonesArray*) ioman->GetObject(fBC1Branch.Data());
    fBC2In = (TClonesArray*) ioman->GetObject(fBC2Branch.Data());
    fBDIn = (TClonesArray*) ioman->GetObject(fBDBranch.Data());

    fGemDigitsIn = (TClonesArray*) ioman->GetObject(fGemBranchIn.Data());
    fSiDigitsIn = (TClonesArray*) ioman->GetObject(fSiBranchIn.Data());

    fGemDigitsOut = new TClonesArray("BmnGemStripDigit");
    fSiDigitsOut = new TClonesArray("BmnSiliconDigit");

    fSiOut = new TClonesArray("BmnTrigDigit");
    fVetoOut = new TClonesArray("BmnTrigDigit");
    fBC1Out = new TClonesArray("BmnTrigDigit");
    fBC2Out = new TClonesArray("BmnTrigDigit");
    fBDOut = new TClonesArray("BmnTrigDigit");

    fBmnHeaderOut = new TClonesArray("BmnEventHeader");
    ioman->Register("EventHeader", "EventHeader_", fBmnHeaderOut, kTRUE);

    if (isGem)
        ioman->Register(fGemBranchOut.Data(), "GEM_", fGemDigitsOut, kTRUE);
    if (isSil)
        ioman->Register(fSiBranchOut.Data(), "SILICON_", fSiDigitsOut, kTRUE);

    if (isTrig) {
        ioman->Register(fSiBranch.Data(), "Si_", fSiOut, kTRUE);
        ioman->Register(fVetoBranch.Data(), "VETO_", fVetoOut, kTRUE);
        ioman->Register(fBC1Branch.Data(), "BC1_", fBC1Out, kTRUE);
        ioman->Register(fBC2Branch.Data(), "BC2_", fBC2Out, kTRUE);
        ioman->Register(fBDBranch.Data(), "BD_", fBDOut, kTRUE);
    }

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString confSi = "SiliconRunSpring2018.xml";
    TString confGem = "GemRunSpring2018.xml";

    /// SI
    TString gPathSiliconConfig = gPathConfig + "/silicon/XMLConfigs/";
    fDetectorSI = new BmnSiliconStationSet(gPathSiliconConfig + confSi);

    /// GEM
    TString gPathGemConfig = gPathConfig + "/gem/XMLConfigs/";
    fDetectorGEM = new BmnGemStripStationSet(gPathGemConfig + confGem);

    // RUN7, FIXME
    Int_t stats[fDetectorGEM->GetNStations()] = {1, 2, 4, 5, 6, 7}; // MK-numeration
    Int_t statsPermut[fDetectorGEM->GetNStations()] = {0, 1, 2, 3, 4, 5}; // SM-numeration

    if (isGem) {
        for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
            fGemStats[stats[iStat]] = statsPermut[iStat];
    }

    //    for (auto it :fGemStats)
    //        cout << it.first << " " << it.second << endl;

    if (isTrig) {
        fTriggers.insert(pair <TClonesArray*, TClonesArray*> (fSiIn, fSiOut));
        fTriggers.insert(pair <TClonesArray*, TClonesArray*> (fVetoIn, fVetoOut));
        fTriggers.insert(pair <TClonesArray*, TClonesArray*> (fBC1In, fBC1Out));
        fTriggers.insert(pair <TClonesArray*, TClonesArray*> (fBC2In, fBC2Out));
        fTriggers.insert(pair <TClonesArray*, TClonesArray*> (fBDIn, fBDOut));
    }

    return kSUCCESS;
}

void BmnDigiConverter::Exec(Option_t* opt) {
    if (fEventNo % 1000 == 0)
        cout << "Ev# " << fEventNo << endl;

    // Event Header
    BmnEventHeader* evHeaderIn = (BmnEventHeader*) fBmnHeaderIn->UncheckedAt(0);
    UInt_t runID = evHeaderIn->GetRunId();
    UInt_t eventID = evHeaderIn->GetEventId();
    TDatime eventTime = evHeaderIn->GetEventTime();
    BmnEventType eventType = evHeaderIn->GetType();
    BmnTrigInfo* trigInfo = evHeaderIn->GetTrigInfo(); // cout << trigType << endl;
    Bool_t tripWord = evHeaderIn->GetTripWord();

    fBmnHeaderOut->Delete();

    new((*fBmnHeaderOut)[fBmnHeaderOut->GetEntriesFast()]) BmnEventHeader(runID, eventID, eventTime, eventType, tripWord, trigInfo);
    //    BmnEventHeader* evHeaderOut = new((*fBmnHeaderOut)[fBmnHeaderOut->GetEntriesFast()]) BmnEventHeader();  
    //    evHeaderOut->SetTrigType(trigType);

    //    cout << evHeaderOut->GetTrig() << endl;


    fGemDigitsOut->Delete();
    fSiDigitsOut->Delete();

    fSiOut->Delete();
    fVetoOut->Delete();
    fBC1Out->Delete();
    fBC2Out->Delete();
    fBDOut->Delete();

    // GEM
    if (isGem)
        for (UInt_t iDigi = 0; iDigi < fGemDigitsIn->GetEntriesFast(); iDigi++) {
            BmnGemStripDigit* gemDig = (BmnGemStripDigit*) fGemDigitsIn->UncheckedAt(iDigi);

            Int_t stat = gemDig->GetStation();
            // CSC should be omitted
            if (stat == 8 || stat == 3)
                continue;
            gemDig->SetStation(GemStatPermutation(stat));
            
            Int_t strip = gemDig->GetStripNumber() - 1; // strips are enumerated from 0

            // Stations permuted already!
            stat = gemDig->GetStation();           
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

            new((*fGemDigitsOut)[fGemDigitsOut->GetEntriesFast()]) BmnGemStripDigit(gemDig->GetStation(),
                    gemDig->GetModule(),
                    gemDig->GetStripLayer(),
                    strip,
                    gemDig->GetStripSignal());
        }

    // SILICON
    if (isSil)
        for (UInt_t iDigi = 0; iDigi < fSiDigitsIn->GetEntriesFast(); iDigi++) {
            BmnSiliconDigit* siDig = (BmnSiliconDigit*) fSiDigitsIn->UncheckedAt(iDigi);

            Int_t stat = siDig->GetStation() - 1;
            Int_t strip = siDig->GetStripNumber() - 1; // strips are enumerated from 0
            Int_t signal = siDig->GetStripSignal();

            Int_t mod = siDig->GetModule();
            Int_t layer = siDig->GetStripLayer();

            new((*fSiDigitsOut)[fSiDigitsOut->GetEntriesFast()]) BmnSiliconDigit(stat, mod, layer, strip, signal);
        }

    // TRIGGERS (Si, VETO, BC1, BC2, BD)
    if (isTrig)
        ConvertTriggers(fTriggers);

    fEventNo++;
}

void BmnDigiConverter::Finish() {
    ioman->GetOutTree()->SetName("cbmsim");

    delete fDetectorSI;
    delete fDetectorGEM;
}

void BmnDigiConverter::ConvertTriggers(map <TClonesArray*, TClonesArray*> trig) {
    for (auto &it : trig) {
        TClonesArray* in = it.first;
        TClonesArray* out = it.second;
        for (UInt_t iDigi = 0; iDigi < in->GetEntriesFast(); iDigi++) {
            BmnTrigDigit* dig = (BmnTrigDigit*) in->UncheckedAt(iDigi);
            new((*out)[out->GetEntriesFast()]) BmnTrigDigit(dig->GetMod(), dig->GetTime(), dig->GetAmp());
        }
    }
}

BmnDigiConverter::~BmnDigiConverter() {



}



