#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void sp41FieldMapCreator(TString in = "field_sp41v2_ascii_noExtrap.dat", TString out = "testMap_200x200.dat") {
    bmnloadlibs(); // load libraries    

    // field_sp41v2_ascii_noExtrap.dat - is an initial field map to be read and extrapolated ($VMCWORKDIR/input)
    BmnSP41FieldMapCreator* newField = new BmnSP41FieldMapCreator(in);
    newField->SetDebug(kTRUE);
    newField->SetNodeNumber(200);
    newField->SetDrawProfiles(kTRUE);
   
    const Int_t nParts = 2; // Left and right side ( < -28. and > 360. cm)
    const Int_t nComponents = 3; // x -> 0, y -> 1, z -> 2
    
    for (Int_t iPart = 0; iPart < nParts; iPart++)
        for (Int_t iComp = 0; iComp < nComponents; iComp++)
            newField->AnalyzeFieldProfiles(iPart, iComp);
    
    newField->CreateNewMap(out);
    delete newField;
}
