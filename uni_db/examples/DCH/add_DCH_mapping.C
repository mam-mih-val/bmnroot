// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void ReadAndPut(TString fName, vector<UniValue*>& pValues1, vector<UniValue*>& pValues2);

void AssignMapStructure(vector<UniValue*>& pArray, int pl, int gr, unsigned int cr, int sl, int ch_l, int ch_h)
{
    DchMapValue* pValue = new DchMapValue;
    pValue->plane = pl;
    pValue->group = gr;
    pValue->crate = cr;
    pValue->slot = sl;
    pValue->channel_low = ch_l;
    pValue->channel_high = ch_h;

    pArray.push_back(pValue);
}

void add_DCH_mapping()
{
    Bool_t return_error = kFALSE;

    vector<UniValue*> pValues1;
    vector<UniValue*> pValues2;
    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    ReadAndPut(path + TString("DCH_map_Run7.txt"), pValues1, pValues2);

    UniDbDetectorParameter::CreateDetectorParameter("DCH1", "DCH_mapping", 7, 1, 7, 10000, pValues1);
    UniDbDetectorParameter::CreateDetectorParameter("DCH2", "DCH_mapping", 7, 1, 7, 10000, pValues2);

    for (int i = 0; i < pValues1.size(); i++) delete pValues1[i];
    for (int i = 0; i < pValues2.size(); i++) delete pValues2[i];

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

void ReadAndPut(TString fName, vector<UniValue*>& pValues1, vector<UniValue*>& pValues2) {
    UInt_t ser = 0;
    Int_t ch_l = 0;
    Int_t ch_h = 0;
    Int_t slot = 0;
    Int_t group = 0;
    TString name;
    Int_t planeId;

    TString planes[16];
    planes[0] = "VA_1";
    planes[1] = "VB_1";
    planes[2] = "UA_1";
    planes[3] = "UB_1";
    planes[4] = "YA_1";
    planes[5] = "YB_1";
    planes[6] = "XA_1";
    planes[7] = "XB_1";
    planes[8] = "VA_2";
    planes[9] = "VB_2";
    planes[10] = "UA_2";
    planes[11] = "UB_2";
    planes[12] = "YA_2";
    planes[13] = "YB_2";
    planes[14] = "XA_2";
    planes[15] = "XB_2";

    TString dummy;

    ifstream inFile(fName.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << fName << ")!" << endl;
    inFile >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    inFile >> dummy;
    while (!inFile.eof()) {
        inFile >> name >> group >> std::hex >> ser >> std::dec >> slot >> ch_l >> ch_h;
        if (!inFile.good()) break;
        Int_t j = 0;
        for (j = 0; j < 16; ++j)
            if (name == planes[j])
                break;
        planeId = j;
        printf("%s\t%d\t%d\t0x%x\t%d\t%d\t%d\n", name.Data(), planeId, group, ser, slot, ch_l, ch_h);
        if (planeId / 8 == 0)
            AssignMapStructure(pValues1, planeId, group, ser, slot, ch_l, ch_h);
        else
            AssignMapStructure(pValues2, planeId, group, ser, slot, ch_l, ch_h);
    }   
}
