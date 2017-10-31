#include <stdio.h>
class GemMapStructure;

int AssignMapStructure(GemMapStructure* pArray, int id, unsigned int ser, int ch_l, int ch_h, int gemId, int st, int hz) {
    pArray[id].serial = ser;
    pArray[id].id = gemId;
    pArray[id].station = st;
    pArray[id].hotZone = hz;
    pArray[id].channel_low = ch_l;
    pArray[id].channel_high = ch_h;
    return 0;
}

// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)

void add_GEM_mapping() {
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

    const int kNitems = 28;

    GemMapStructure* pValues = new GemMapStructure[kNitems];
    
    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    ReadAndPut(path + TString("GEM_map_run7.txt"), pValues);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_global_mapping", 7, 1, 7, 10000, pValues, kNitems);
    if (pDetectorParameter == NULL)
        return_error = true;

    // clean memory after work
    delete [] pValues;
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

void ReadAndPut(TString fName, GemMapStructure* pValues) {
    UInt_t ser = 0;
    Int_t ch_l = 0;
    Int_t ch_h = 0;
    Int_t gem_id = 0;
    Int_t stat = 0;
    Int_t mod = 0;
    string dummy;

    ifstream inFile(fName.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << fName << ")!" << endl;
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file   
    getline(inFile, dummy); //comment line in input file   
    getline(inFile, dummy); //comment line in input file   
    
    Int_t i = 0;
    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_l >> ch_h >> gem_id >> stat >> mod;
        if (!inFile.good()) break;
        printf("%X\t%d\t%d\t%d\t%d\t%d\n", ser, ch_l, ch_h, gem_id, stat, mod);
        AssignMapStructure(pValues, i++, ser, ch_l, ch_h, gem_id, stat, mod);
    }
    
}