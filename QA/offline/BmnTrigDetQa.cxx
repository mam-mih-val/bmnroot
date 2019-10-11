#include "BmnTrigDetQa.h"

BmnTrigDetQa::BmnTrigDetQa(vector <TString> triggers, UInt_t id) :
fHistoManager(new BmnQaHistoManager()), 
fSteering(new BmnOfflineQaSteering()) {
    
    pair <Int_t, TString> periodSetup = fSteering->GetRunAndSetupByRunId(id);
    TString prefix = TString::Format("RUN%d_SETUP_%s_", periodSetup.first, periodSetup.second.Data());

    for (Int_t iDet = 0; iDet < triggers.size(); iDet++)
        CommonInfo(triggers[iDet], prefix);
}

BmnTrigDetQa::BmnTrigDetQa(map <TClonesArray*, TString> triggers, UInt_t id) :
fHistoManager(new BmnQaHistoManager()),
fSteering(new BmnOfflineQaSteering()) {
    
    pair <Int_t, TString> periodSetup = fSteering->GetRunAndSetupByRunId(id);
    TString prefix = TString::Format("RUN%d_SETUP_%s_", periodSetup.first, periodSetup.second.Data());
    
     for (auto it : triggers)
        CommonInfo(it.second, prefix);
}