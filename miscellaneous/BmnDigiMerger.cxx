#include "BmnDigiMerger.h"

void BmnDigiMerger::Finish() {

    delete fChain;

}

BmnDigiMerger::BmnDigiMerger(TString addFile) :
fSiIn(nullptr),
fVetoIn(nullptr),
fBC1In(nullptr),
fBC2In(nullptr),
fBC3In(nullptr),
fBDIn(nullptr),
fGemDigitsIn(nullptr),
fSiDigitsIn(nullptr),
fGemDigitsOut(nullptr),
fSiDigitsOut(nullptr),
fSiOut(nullptr),
fVetoOut(nullptr),
fBC1Out(nullptr),
fBC2Out(nullptr),
fBC3Out(nullptr),
fBDOut(nullptr),
fSilDigits(nullptr),
fBmnHeaderIn(nullptr),     
fBmnHeaderOut(nullptr),     
fChain(new TChain("cbmsim")) {
    fChain->Add(addFile.Data());
    fChain->SetBranchAddress("SILICON", &fSilDigits);

    // Inner tracker
    fGemBranchIn = "GEM";
    fSiBranchIn = "SILICON";

    fGemBranchOut = "GEM";
    fSiBranchOut = "SILICON";

    // Triggers
    fSiBranch = "Si";
    fVetoBranch = "VC";
    fBC1Branch = "BC1";
    fBC2Branch = "BC2";
    fBC3Branch = "BC3";
    fBDBranch = "BD";

}

InitStatus BmnDigiMerger::Init() {
    cout << " BmnDigiMerger::Init() " << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    ioman->GetBranchNameList()->Print();
    FairEventHeader* h = (FairEventHeader*) ioman->GetObject("EventHeader.");
    h->SetNameTitle("FairEventHeader", "FairEventHeader");

    fBmnHeaderIn = (TClonesArray*) ioman->GetObject("EventHeader");
    
    // Triggers (in)
    fSiIn = (TClonesArray*) ioman->GetObject(fSiBranch.Data());
    fVetoIn = (TClonesArray*) ioman->GetObject(fVetoBranch.Data());
    fBC1In = (TClonesArray*) ioman->GetObject(fBC1Branch.Data());
    fBC2In = (TClonesArray*) ioman->GetObject(fBC2Branch.Data());
    fBC3In = (TClonesArray*) ioman->GetObject(fBC3Branch.Data());
    fBDIn = (TClonesArray*) ioman->GetObject(fBDBranch.Data());

    // Inner tracker (in)
    fGemDigitsIn = (TClonesArray*) ioman->GetObject(fGemBranchIn.Data());
    fSiDigitsIn = (TClonesArray*) ioman->GetObject(fSiBranchIn.Data());

    // Inner tracker (out)
    fGemDigitsOut = new TClonesArray("BmnGemStripDigit");
    fSiDigitsOut = new TClonesArray("BmnSiliconDigit");

    // Triggers (out)
    fSiOut = new TClonesArray("BmnTrigDigit");
    fVetoOut = new TClonesArray("BmnTrigDigit");
    fBC1Out = new TClonesArray("BmnTrigDigit");
    fBC2Out = new TClonesArray("BmnTrigDigit");
    fBC3Out = new TClonesArray("BmnTrigDigit");
    fBDOut = new TClonesArray("BmnTrigDigit");

    ioman->Register(fSiBranch.Data(), "Si_", fSiOut, kTRUE);
    ioman->Register(fVetoBranch.Data(), "VC_", fVetoOut, kTRUE);
    ioman->Register(fBC1Branch.Data(), "BC1_", fBC1Out, kTRUE);
    ioman->Register(fBC2Branch.Data(), "BC2_", fBC2Out, kTRUE);
    ioman->Register(fBC3Branch.Data(), "BC3_", fBC3Out, kTRUE);
    ioman->Register(fBDBranch.Data(), "BD_", fBDOut, kTRUE);

    ioman->Register(fGemBranchIn.Data(), "GEM_", fGemDigitsOut, kTRUE);
    ioman->Register(fSiBranchIn.Data(), "SILICON_", fSiDigitsOut, kTRUE);

    fBmnHeaderOut = new TClonesArray("BmnEventHeader");
    ioman->Register("EventHeader", "EventHeader_", fBmnHeaderOut, kTRUE);

    fCurrEvent = 0;
}

void BmnDigiMerger::Exec(Option_t* opt) {
    fGemDigitsOut->Delete();
    fSiDigitsOut->Delete();

    fSiOut->Delete();
    fVetoOut->Delete();
    fBC1Out->Delete();
    fBC2Out->Delete();
    fBC3Out->Delete();
    fBDOut->Delete();
    
    fBmnHeaderOut->Delete();
    
    // Event Header
    BmnEventHeader* evHeaderIn = (BmnEventHeader*) fBmnHeaderIn->UncheckedAt(0);
    UInt_t runID = evHeaderIn->GetRunId();
    UInt_t eventID = evHeaderIn->GetEventId();
    TDatime eventTime = evHeaderIn->GetEventTime();
    BmnEventType eventType = evHeaderIn->GetType();
    BmnTrigInfo* trigInfo = evHeaderIn->GetTrigInfo();
    Bool_t tripWord = evHeaderIn->GetTripWord();
   
    new((*fBmnHeaderOut)[fBmnHeaderOut->GetEntriesFast()]) BmnEventHeader(runID, eventID, eventTime, eventType, tripWord, trigInfo);    

    // GEM
    for (UInt_t iDigi = 0; iDigi < fGemDigitsIn->GetEntriesFast(); iDigi++) {
        BmnGemStripDigit* gemDig = (BmnGemStripDigit*) fGemDigitsIn->UncheckedAt(iDigi);
        BmnGemStripDigit* newDig = new((*fGemDigitsOut)[fGemDigitsOut->GetEntriesFast()]) BmnGemStripDigit(
                gemDig->GetStation(),
                gemDig->GetModule(),
                gemDig->GetStripLayer(),
                gemDig->GetStripNumber(),
                gemDig->GetStripSignal());
        
        newDig->SetIsGoodDigit(gemDig->IsGoodDigit());
    }

    fChain->GetEntry(fCurrEvent);

    // SILICON
    for (UInt_t iDigi = 0; iDigi < fSilDigits->GetEntriesFast(); iDigi++) {
        BmnSiliconDigit* silDig = (BmnSiliconDigit*) fSilDigits->UncheckedAt(iDigi);
        BmnSiliconDigit* newDig = new((*fSiDigitsOut)[fSiDigitsOut->GetEntriesFast()]) BmnSiliconDigit(
                silDig->GetStation(),
                silDig->GetModule(),
                silDig->GetStripLayer(),
                silDig->GetStripNumber(),
                silDig->GetStripSignal());
        
        newDig->SetIsGoodDigit(silDig->IsGoodDigit());
    }

    fCurrEvent++;
}






