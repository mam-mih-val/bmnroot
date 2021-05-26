// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void ReadAndPut(TString fName, vector<UniValue*>& pValues);

void AssignMapStructure(vector<UniValue*>& pArray, unsigned int ser, int ch_l, int ch_h, int gemId, int st, int hz)
{
    GemMapValue* pValue = new GemMapValue;
    pValue->serial = ser;
    pValue->id = gemId;
    pValue->station = st;
    pValue->hotZone = hz;
    pValue->channel_low = ch_l;
    pValue->channel_high = ch_h;

    pArray.push_back(pValue);
}

void add_GEM_mapping()
{
    bool return_error = false;

    vector<UniValue*> pValues;
    
    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    ReadAndPut(path + TString("GEM_map_run7.txt"), pValues);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("GEM", "GEM_global_mapping", 7, 1, 7, 10000, pValues);
    if (pDetectorParameter == NULL)
        return_error = true;

    // clean memory after work
    for (int i = 0; i < pValues.size(); i++) delete pValues[i];
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

void ReadAndPut(TString fName, vector<UniValue*>& pValues)
{
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
    
    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_l >> ch_h >> gem_id >> stat >> mod;
        if (!inFile.good()) break;
        printf("%X\t%d\t%d\t%d\t%d\t%d\n", ser, ch_l, ch_h, gem_id, stat, mod);
        AssignMapStructure(pValues, ser, ch_l, ch_h, gem_id, stat, mod);
    }    
}
