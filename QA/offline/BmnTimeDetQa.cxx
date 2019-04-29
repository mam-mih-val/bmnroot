#include "BmnTimeDetQa.h"

BmnTimeDetQa::BmnTimeDetQa(TString detName) :
fHistoManager(new BmnQaHistoManager()) {

    CommonInfo(detName);
    
    if (detName.Contains("TOF")) 
       TofInfo(detName);
  
    else if (detName.Contains("DCH") || detName.Contains("MWPC")) 
           MwpcDchInfo(detName);
   
    else
        throw;
}
