#include "../../db_structures.h"
#include "TString.h"

int AssignMapStructure(GemPedestalStructure* pArray, int id, unsigned int ser, int ch, int ped, int cm, int noise) {
    pArray[id].serial = ser;
    pArray[id].channel = ch;
    pArray[id].pedestal = ped;
    //pArray[id].common_mode = cm;
    pArray[id].noise = noise;
    return 0;
}

// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)

void add_GEM_pedestals() {
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

    //UniDbParameter::CreateParameter("GEM_pedestal", GemPedestalArrayType);

    const int kNch = 20482;

    GemPedestalStructure* Ped = new GemPedestalStructure[kNch];

    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    ReadAndPut(path + TString("GEM_pedestals.txt"), Ped);

    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_pedestal", 4, 61, 4, 84, Ped, kNch);

    // clean memory after work
    delete [] Ped;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

void ReadAndPut(TString fName, GemPedestalStructure* pValues) {
    UInt_t ser = 0;
    Int_t ch = 0;
    Int_t p = 0;
    Int_t cm = 0;
    Int_t n = 0;
    TString dummy;
    ifstream inFile(fName.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << fName << ")!" << endl;
    inFile >> dummy >> dummy >> dummy >> dummy >> dummy;
    inFile >> dummy;
    Int_t i = 0;
    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch >> p >> cm >> n;
        if (!inFile.good()) break; 
        AssignMapStructure(pValues, i++, ser, ch, p, cm, n);
    }
}