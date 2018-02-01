#include "BmnTriggersCheck.h"

BmnTriggersCheck::BmnTriggersCheck(Bool_t isExp) {
    fIsExp = isExp;
    
    fT0Branch = "T0";
    fVetoBranch = "VETO";
    fBC2Branch = "BC2";
    fBDBranch = "BD";
    fBmnEventHeaderBranchName = "EventHeader";
    fBmnEvQualityBranchName = "BmnEventQuality";
}

InitStatus BmnTriggersCheck::Init() { 
    FairRootManager* ioman = FairRootManager::Instance();
      
    fBmnEventHeader = (TClonesArray*) ioman->GetObject(fBmnEventHeaderBranchName);
    fT0Array = (TClonesArray*) ioman->GetObject(fT0Branch.Data());
    fVetoArray = (TClonesArray*) ioman->GetObject(fVetoBranch.Data());
    fBC2Array = (TClonesArray*) ioman->GetObject(fBC2Branch.Data());
    fBDArray = (TClonesArray*) ioman->GetObject(fBDBranch.Data());
    
    fBmnEvQuality = new TClonesArray(fBmnEvQualityBranchName);
    ioman->Register(fBmnEvQualityBranchName, "QUALITY", fBmnEvQuality, true);
}

void BmnTriggersCheck::Exec(Option_t* opt) {
    if (!fIsExp)
        return;
    
    fBmnEvQuality->Delete();
    
    BmnEventQuality* evQual = new ((*fBmnEvQuality)[fBmnEvQuality->GetEntriesFast()]) BmnEventQuality("GOOD");
        
    const Int_t kEnergies = 4;
    const Int_t kTargets = 5;
    Double_t energies[kEnergies] = {3.5, 4., 4.5, 5.14};
    TString targets[kTargets] = {"C", "Al", "Cu", "Pb", "C2H4"};

    BmnEventHeader* evHeader = (BmnEventHeader*) fBmnEventHeader->UncheckedAt(0);
    if (!evHeader)
        return;
               
    UniDbRun* runInfo = UniDbRun::GetRun(6, evHeader->GetRunId());

    BmnTriggerType trigType = evHeader->GetTrig();
    if (trigType == kBMNMINBIAS) {
        // Setup BD-threshold
        // To be read subsequently from the UniDB directly when processing RUN7, FIXME
        TString target = *runInfo->GetTargetParticle();
        Double_t energy = *runInfo->GetEnergy();
        Int_t bdThresh;
        if (Abs(energy - energies[0]) < FLT_EPSILON) // 3.5 GeV/n, all targets
            bdThresh = 2;
        else if (Abs(energy - energies[1]) < FLT_EPSILON && (target == targets[0] || target == targets[4])) // 4 GeV/n, C, C2H4
            bdThresh = 2;
        else if (Abs(energy - energies[1]) < FLT_EPSILON && target != targets[0] && target != targets[4]) // 4 GeV/n, Al, Cu, Pb
            bdThresh = 3;
        else if (Abs(energy - energies[2]) < FLT_EPSILON && (target == targets[0] || target == targets[4])) // 4.5 GeV/n, C, C2H4
            bdThresh = 2;
        else if (Abs(energy - energies[2]) < FLT_EPSILON && target != targets[0] && target != targets[4]) // 4 GeV/n, Al, Cu, Pb
            bdThresh = 3;
        else if (Abs(energy - energies[3]) < FLT_EPSILON) // 5.14 GeV/n, all targets
            bdThresh = 2;
        else
            bdThresh = 0;             
        
        if (fT0Array->GetEntriesFast() != 1 || fBC2Array->GetEntriesFast() != 1 || fVetoArray->GetEntriesFast() != 0 || fBDArray->GetEntriesFast() < bdThresh)
            evQual->SetIsGoodEvent("BAD");
    }

    else if (trigType == kBMNBEAM) {
        if (fVetoArray->GetEntriesFast() > 1 || fBDArray->GetEntriesFast() > 0)
            evQual->SetIsGoodEvent("BAD");
    }

    else
        evQual->SetIsGoodEvent("BAD");
}

void BmnTriggersCheck::Finish() {
 
}

