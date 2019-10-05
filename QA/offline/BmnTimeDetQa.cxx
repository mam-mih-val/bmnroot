#include "BmnTimeDetQa.h"

BmnTimeDetQa::BmnTimeDetQa(TString detName, UInt_t id) :
fSteering(new BmnOfflineQaSteering()),
fHistoManager(new BmnQaHistoManager()) {
    
    pair <Int_t, TString> periodSetup = fSteering->GetRunAndSetupByRunId(id);
    TString prefix = TString::Format("RUN%d_SETUP_%s_", periodSetup.first, periodSetup.second.Data());

    CommonInfo(prefix + detName);
    
    if (detName.Contains("TOF")) 
       TofInfo(prefix + detName);
  
    else if (detName.Contains("DCH") || detName.Contains("MWPC")) 
           MwpcDchInfo(prefix + detName);
   
    else
        throw;
}
