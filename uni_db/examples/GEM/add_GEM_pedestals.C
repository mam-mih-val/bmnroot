// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void ReadAndPut(TString fName, vector<UniValue*>& pValues);
Int_t CalcSize(TString fName);

void AssignMapStructure(vector<UniValue*>& pArray, unsigned int ser, int ch, double ped, double noise)
{
    GemPedestalValue* pValue = new GemPedestalValue;
    pValue->serial = ser;
    pValue->channel = ch;
    pValue->pedestal = ped;
    pValue->noise = noise;

    pArray.push_back(pValue);
}

void add_GEM_pedestals()
{
    bool return_error = false;

    //UniDbParameter::CreateParameter("GEM_pedestal", GemPedestalType);
    //UniDbParameter::CreateParameter("GEM_size_ped_map", IntType);

    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    Int_t size = CalcSize(path + TString("GEM_pedestals.txt"));
    vector<UniValue*> Ped;
    ReadAndPut(path + TString("GEM_pedestals.txt"), Ped);

    IntValue iValue; iValue.value = size;
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_size_ped_map", 4, 61, 4, 84, &iValue);
    UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_pedestal", 4, 61, 4, 84, Ped);

    // clean memory after work
    for (int i = 0; i < Ped.size(); i++) delete Ped[i];

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

Int_t CalcSize(TString fName)
{
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

void ReadAndPut(TString fName, vector<UniValue*>& pValues)
{
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
    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch >> ped >> rms;
        if (!inFile.good()) break;
        AssignMapStructure(pValues, ser, ch, ped, rms);
    }
}
