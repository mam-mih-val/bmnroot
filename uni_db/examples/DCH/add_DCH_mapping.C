
class DchMapStructure;

int AssignMapStructure(DchMapStructure* pArray, int id, int pl, int gr, unsigned int cr, int sl, int ch_l, int ch_h) {
    pArray[id].plane = pl;
    pArray[id].group = gr;
    pArray[id].crate = cr;
    pArray[id].slot = sl;
    pArray[id].channel_low = ch_l;
    pArray[id].channel_high = ch_h;
    return 0;
}

// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)

void add_DCH_mapping() {
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    Bool_t return_error = kFALSE;

    const int kNitems1 = 120;
    const int kNitems2 = 72;

    DchMapStructure* pValues1 = new DchMapStructure[kNitems1];
    DchMapStructure* pValues2 = new DchMapStructure[kNitems2];
    TString path = TString(getenv("VMCWORKDIR")) + TString("/input/");
    ReadAndPut(path + TString("DCH_map_Run6.txt"), pValues1, pValues2);

    UniDbDetectorParameter::CreateDetectorParameter("DCH1", "DCH_mapping", 6, 1, 6, 10000, pValues1, kNitems1);
    UniDbDetectorParameter::CreateDetectorParameter("DCH2", "DCH_mapping", 6, 1, 6, 10000, pValues2, kNitems2);

    delete [] pValues1;
    delete [] pValues2;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

void ReadAndPut(TString fName, DchMapStructure* pValues1, DchMapStructure* pValues2) {
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
    Int_t i1 = 0;
    Int_t i2 = 0;
    while (!inFile.eof()) {
        inFile >> name >> group >> std::hex >> ser >> std::dec >> slot >> ch_l >> ch_h;
        if (!inFile.good()) break;
        Int_t j = 0;
        for (j = 0; j < 16; ++j)
            if (name == planes[j])
                break;
        planeId = j;
        printf("%s\t%d\t%d\t%d\t%d\t%d\n", name.Data(), planeId, group, ser, slot, ch_l, ch_h);
        if (planeId / 8 == 0)
            AssignMapStructure(pValues1, i1++, planeId, group, ser, slot, ch_l, ch_h);
        else
            AssignMapStructure(pValues2, i2++, planeId, group, ser, slot, ch_l, ch_h);
    }
    
}