#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void sp41FieldMapCreator(TString in = "", 
        TString out1 = "", 
        TString out2 = "") {
    
    // x
    BmnSP41FieldMapCreator* x = new BmnSP41FieldMapCreator(in, out1);
    x->SetDebug(kTRUE);   
    x->SetExtrapolationAxis("X");
    x->SetNewRanges(-250., +250.);
    x->Extrapolation();    
    delete x;
    
    // z
    BmnSP41FieldMapCreator* z = new BmnSP41FieldMapCreator(out1, out2);
    z->SetDebug(kTRUE);   
    z->SetExtrapolationAxis("Z");
    z->SetNewRanges(-200, +500.);
    z->Extrapolation();    
    delete z;  
}
