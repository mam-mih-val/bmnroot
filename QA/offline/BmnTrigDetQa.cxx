#include "BmnTrigDetQa.h"

BmnTrigDetQa::BmnTrigDetQa(vector <TString> triggers) :
fHistoManager(new BmnQaHistoManager()) {

    for (Int_t iDet = 0; iDet < triggers.size(); iDet++)
        CommonInfo(triggers[iDet]);
}

BmnTrigDetQa::BmnTrigDetQa(map <TClonesArray*, TString> triggers) :
fHistoManager(new BmnQaHistoManager()) {
    
     for (auto it : triggers)
        CommonInfo(it.second);
}