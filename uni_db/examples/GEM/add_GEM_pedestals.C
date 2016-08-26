#include "../../db_structures.h"
#include "TString.h"

int AssignMapStructure(GemPedestalStructure* pArray, int id, unsigned int ser, int ch, double ped, double noise) {
    pArray[id].serial = ser;
    pArray[id].channel = ch;
    pArray[id].pedestal = ped;
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
    //UniDbParameter::CreateParameter("GEM_size_ped_map", IntType);

    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    Int_t size = CalcSize(path + TString("GEM_pedestals.txt"));
    GemPedestalStructure* Ped = new GemPedestalStructure[size];
    ReadAndPut(path + TString("GEM_pedestals.txt"), Ped);

    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_size_ped_map", 4, 61, 4, 84, size);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_pedestal", 4, 61, 4, 84, Ped, size);

    // clean memory after work
    delete [] Ped;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

Int_t CalcSize(TString fName) {
    TString dummy;
    ifstream inFile(fName.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << fName << ")!" << endl;
    inFile >> dummy >> dummy >> dummy >> dummy;
    inFile >> dummy;
    Int_t i = 0;
    while (!inFile.eof()) {
        inFile >> dummy >> dummy >> dummy >> dummy;
        if (!inFile.good()) break;
        i++;
    }
    inFile.close();
    return i;
}

void ReadAndPut(TString fName, GemPedestalStructure* pValues) {
    UInt_t ser = 0;
    Int_t ch = 0;
    double ped = 0;
    double rms = 0;
    TString dummy;
    ifstream inFile(fName.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << fName << ")!" << endl;
    inFile >> dummy >> dummy >> dummy >> dummy;
    inFile >> dummy;
    Int_t i = 0;
    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch >> ped >> rms;
        if (!inFile.good()) break;
        AssignMapStructure(pValues, i++, ser, ch, ped, rms);
    }
}