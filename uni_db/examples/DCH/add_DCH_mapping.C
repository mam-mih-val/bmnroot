
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

    bool return_error = false;

    const int kNitems = 72;

    DchMapStructure* pValues1 = new DchMapStructure[kNitems];
    DchMapStructure* pValues2 = new DchMapStructure[kNitems];

//    //VA_1
    AssignMapStructure(pValues1, 0, 0, 1, 0x46f1f79, 1, 0, 15);
    AssignMapStructure(pValues1, 1, 0, 2, 0x46f1f79, 1, 16, 31);
    AssignMapStructure(pValues1, 2, 0, 3, 0x46f1f79, 1, 32, 47);
    AssignMapStructure(pValues1, 3, 0, 4, 0x46f1f79, 1, 48, 63);
    AssignMapStructure(pValues1, 4, 0, 5, 0x46f1f79, 2, 0, 15);
    AssignMapStructure(pValues1, 5, 0, 6, 0x46f1f79, 2, 16, 31);
    AssignMapStructure(pValues1, 6, 0, 7, 0x46f1f79, 2, 32, 47);
    AssignMapStructure(pValues1, 7, 0, 15, 0x46f1f79, 2, 48, 63);
    AssignMapStructure(pValues1, 8, 0, 8, 0x46f1f79, 3, 0, 15);
    //VB_1
    AssignMapStructure(pValues1, 9, 1, 1, 0x46f1f79, 3, 16, 31);
    AssignMapStructure(pValues1, 10, 1, 2, 0x46f1f79, 3, 32, 47);
    AssignMapStructure(pValues1, 11, 1, 3, 0x46f1f79, 3, 48, 63);
    AssignMapStructure(pValues1, 12, 1, 4, 0x46f1f79, 4, 0, 15);
    AssignMapStructure(pValues1, 13, 1, 5, 0x46f1f79, 4, 16, 31);
    AssignMapStructure(pValues1, 14, 1, 6, 0x46f1f79, 4, 32, 47);
    AssignMapStructure(pValues1, 15, 1, 7, 0x46f1f79, 4, 48, 63);
    AssignMapStructure(pValues1, 16, 1, 15, 0x46f1f79, 5, 0, 15);
    AssignMapStructure(pValues1, 17, 1, 8, 0x46f1f79, 5, 16, 31);
    //UA_1 
    AssignMapStructure(pValues1, 18, 2, 6, 0x46f1f79, 15, 32, 47);
    AssignMapStructure(pValues1, 19, 2, 7, 0x46f1f79, 15, 48, 63);
    AssignMapStructure(pValues1, 20, 2, 15, 0x46f1f79, 16, 0, 15);
    AssignMapStructure(pValues1, 21, 2, 8, 0x46f1f79, 16, 16, 31);
    AssignMapStructure(pValues1, 22, 2, 9, 0x46f1f79, 16, 32, 47);
    AssignMapStructure(pValues1, 23, 2, 10, 0x46f1f79, 16, 48, 63);
    AssignMapStructure(pValues1, 24, 2, 11, 0x46f1f79, 17, 0, 15);
    AssignMapStructure(pValues1, 25, 2, 12, 0x46f1f79, 17, 16, 31);
    AssignMapStructure(pValues1, 26, 2, 13, 0x46f1f79, 17, 32, 47);
    //UB_1
    AssignMapStructure(pValues1, 27, 3, 6, 0x46f1f79, 17, 48, 63);
    AssignMapStructure(pValues1, 28, 3, 7, 0x46f1f79, 18, 0, 15);
    AssignMapStructure(pValues1, 29, 3, 15, 0x46f1f79, 18, 16, 31);
    AssignMapStructure(pValues1, 30, 3, 8, 0x46f1f79, 18, 32, 47);
    AssignMapStructure(pValues1, 31, 3, 9, 0x46f1f79, 18, 48, 63);
    AssignMapStructure(pValues1, 32, 3, 10, 0x46f1f79, 19, 0, 15);
    AssignMapStructure(pValues1, 33, 3, 11, 0x46f1f79, 19, 16, 31);
    AssignMapStructure(pValues1, 34, 3, 12, 0x46f1f79, 19, 32, 47);
    AssignMapStructure(pValues1, 35, 3, 13, 0x46f1f79, 19, 48, 63);
    //YA_1
    AssignMapStructure(pValues1, 36, 4, 4, 0x46f1f79, 5, 32, 47);
    AssignMapStructure(pValues1, 37, 4, 5, 0x46f1f79, 5, 48, 63);
    AssignMapStructure(pValues1, 38, 4, 6, 0x46f1f79, 6, 0, 15);
    AssignMapStructure(pValues1, 39, 4, 7, 0x46f1f79, 6, 16, 31);
    AssignMapStructure(pValues1, 40, 4, 15, 0x46f1f79, 6, 32, 47);
    AssignMapStructure(pValues1, 41, 4, 8, 0x46f1f79, 6, 48, 63);
    AssignMapStructure(pValues1, 42, 4, 9, 0x46f1f79, 7, 0, 15);
    AssignMapStructure(pValues1, 43, 4, 10, 0x46f1f79, 7, 16, 31);
    //YB_1
    AssignMapStructure(pValues1, 44, 5, 4, 0x46f1f79, 7, 32, 47);
    AssignMapStructure(pValues1, 45, 5, 5, 0x46f1f79, 7, 48, 63);
    AssignMapStructure(pValues1, 46, 5, 6, 0x46f1f79, 8, 0, 15);
    AssignMapStructure(pValues1, 47, 5, 7, 0x46f1f79, 8, 16, 31);
    AssignMapStructure(pValues1, 48, 5, 15, 0x46f1f79, 8, 32, 47);
    AssignMapStructure(pValues1, 49, 5, 8, 0x46f1f79, 8, 48, 63);
    AssignMapStructure(pValues1, 50, 5, 9, 0x46f1f79, 9, 0, 15);
    AssignMapStructure(pValues1, 51, 5, 10, 0x46f1f79, 9, 16, 31);
    //XA_1
    AssignMapStructure(pValues1, 52, 6, 0, 0x46f1f79, 9, 32, 47);
    AssignMapStructure(pValues1, 53, 6, 1, 0x46f1f79, 9, 48, 63);
    AssignMapStructure(pValues1, 54, 6, 2, 0x46f1f79, 10, 0, 15);
    AssignMapStructure(pValues1, 55, 6, 3, 0x46f1f79, 10, 16, 31);
    AssignMapStructure(pValues1, 56, 6, 4, 0x46f1f79, 10, 32, 47);
    AssignMapStructure(pValues1, 57, 6, 5, 0x46f1f79, 10, 48, 63);
    AssignMapStructure(pValues1, 58, 6, 6, 0x46f1f79, 12, 0, 15);
    AssignMapStructure(pValues1, 59, 6, 7, 0x46f1f79, 12, 16, 31);
    AssignMapStructure(pValues1, 60, 6, 15, 0x46f1f79, 12, 32, 47);
    AssignMapStructure(pValues1, 61, 6, 8, 0x46f1f79, 12, 48, 63);
    //XB_1
    AssignMapStructure(pValues1, 62, 7, 0, 0x46f1f79, 13, 0, 15);
    AssignMapStructure(pValues1, 63, 7, 1, 0x46f1f79, 13, 16, 31);
    AssignMapStructure(pValues1, 64, 7, 2, 0x46f1f79, 13, 32, 47);
    AssignMapStructure(pValues1, 65, 7, 3, 0x46f1f79, 13, 48, 63);
    AssignMapStructure(pValues1, 66, 7, 4, 0x46f1f79, 14, 0, 15);
    AssignMapStructure(pValues1, 67, 7, 5, 0x46f1f79, 14, 16, 31);
    AssignMapStructure(pValues1, 68, 7, 6, 0x46f1f79, 14, 32, 47);
    AssignMapStructure(pValues1, 69, 7, 7, 0x46f1f79, 14, 48, 63);
    AssignMapStructure(pValues1, 70, 7, 15, 0x46f1f79, 15, 0, 15);
    AssignMapStructure(pValues1, 71, 7, 8, 0x46f1f79, 15, 16, 31);
    //VA_2
    AssignMapStructure(pValues2, 0, 8, 1, 0x168e5c5, 1, 0, 15);
    AssignMapStructure(pValues2, 1, 8, 2, 0x168e5c5, 1, 16, 31);
    AssignMapStructure(pValues2, 2, 8, 3, 0x168e5c5, 1, 32, 47);
    AssignMapStructure(pValues2, 3, 8, 4, 0x168e5c5, 1, 48, 63);
    AssignMapStructure(pValues2, 4, 8, 6, 0x168e5c5, 2, 0, 15);
    AssignMapStructure(pValues2, 5, 8, 5, 0x168e5c5, 2, 16, 31);
    AssignMapStructure(pValues2, 6, 8, 7, 0x168e5c5, 2, 32, 47);
    AssignMapStructure(pValues2, 7, 8, 15, 0x168e5c5, 2, 48, 63);
    AssignMapStructure(pValues2, 8, 8, 8, 0x168e5c5, 3, 0, 15);
    //VB_2
    AssignMapStructure(pValues2, 9, 9, 1, 0x168e5c5, 3, 16, 31);
    AssignMapStructure(pValues2, 10, 9, 2, 0x168e5c5, 3, 32, 47);
    AssignMapStructure(pValues2, 11, 9, 3, 0x168e5c5, 3, 48, 63);
    AssignMapStructure(pValues2, 12, 9, 4, 0x168e5c5, 4, 0, 15);
    AssignMapStructure(pValues2, 13, 9, 5, 0x168e5c5, 4, 16, 31);
    AssignMapStructure(pValues2, 14, 9, 6, 0x168e5c5, 4, 32, 47);
    AssignMapStructure(pValues2, 15, 9, 7, 0x168e5c5, 4, 48, 63);
    AssignMapStructure(pValues2, 16, 9, 15, 0x168e5c5, 5, 0, 15);
    AssignMapStructure(pValues2, 17, 9, 8, 0x168e5c5, 5, 16, 31);
    //UA_2
    AssignMapStructure(pValues2, 18, 10, 6, 0x168e5c5, 15, 32, 47);
    AssignMapStructure(pValues2, 19, 10, 7, 0x168e5c5, 15, 48, 63);
    AssignMapStructure(pValues2, 20, 10, 15, 0x168e5c5, 16, 0, 15);
    AssignMapStructure(pValues2, 21, 10, 8, 0x168e5c5, 16, 16, 31);
    AssignMapStructure(pValues2, 22, 10, 9, 0x168e5c5, 16, 32, 47);
    AssignMapStructure(pValues2, 23, 10, 10, 0x168e5c5, 16, 48, 63);
    AssignMapStructure(pValues2, 24, 10, 11, 0x168e5c5, 17, 0, 15);
    AssignMapStructure(pValues2, 25, 10, 12, 0x168e5c5, 17, 16, 31);
    AssignMapStructure(pValues2, 26, 10, 13, 0x168e5c5, 17, 32, 47);
    //UB_2
    AssignMapStructure(pValues2, 27, 11, 6, 0x168e5c5, 17, 48, 63);
    AssignMapStructure(pValues2, 28, 11, 7, 0x168e5c5, 18, 0, 15);
    AssignMapStructure(pValues2, 29, 11, 15, 0x168e5c5, 18, 16, 31);
    AssignMapStructure(pValues2, 30, 11, 8, 0x168e5c5, 18, 32, 47);
    AssignMapStructure(pValues2, 31, 11, 9, 0x168e5c5, 18, 48, 63);
    AssignMapStructure(pValues2, 32, 11, 10, 0x168e5c5, 19, 0, 15);
    AssignMapStructure(pValues2, 33, 11, 11, 0x168e5c5, 19, 16, 31);
    AssignMapStructure(pValues2, 34, 11, 12, 0x168e5c5, 19, 32, 47);
    AssignMapStructure(pValues2, 35, 11, 13, 0x168e5c5, 19, 48, 63);
    //YA_2
    AssignMapStructure(pValues2, 36, 12, 4, 0x168e5c5, 5, 32, 47);
    AssignMapStructure(pValues2, 37, 12, 5, 0x168e5c5, 5, 48, 63);
    AssignMapStructure(pValues2, 38, 12, 6, 0x168e5c5, 6, 0, 15);
    AssignMapStructure(pValues2, 39, 12, 7, 0x168e5c5, 6, 16, 31);
    AssignMapStructure(pValues2, 40, 12, 15, 0x168e5c5, 6, 32, 47);
    AssignMapStructure(pValues2, 41, 12, 8, 0x168e5c5, 6, 48, 63);
    AssignMapStructure(pValues2, 42, 12, 9, 0x168e5c5, 7, 0, 15);
    AssignMapStructure(pValues2, 43, 12, 10, 0x168e5c5, 7, 16, 31);
    //YB_2
    AssignMapStructure(pValues2, 44, 13, 4, 0x168e5c5, 7, 32, 47);
    AssignMapStructure(pValues2, 45, 13, 5, 0x168e5c5, 7, 48, 63);
    AssignMapStructure(pValues2, 46, 13, 6, 0x168e5c5, 8, 0, 15);
    AssignMapStructure(pValues2, 47, 13, 7, 0x168e5c5, 8, 16, 31);
    AssignMapStructure(pValues2, 48, 13, 15, 0x168e5c5, 8, 32, 47);
    AssignMapStructure(pValues2, 49, 13, 8, 0x168e5c5, 8, 48, 63);
    AssignMapStructure(pValues2, 50, 13, 9, 0x168e5c5, 9, 0, 15);
    AssignMapStructure(pValues2, 51, 13, 10, 0x168e5c5, 9, 16, 31);
    //XA_2
    AssignMapStructure(pValues2, 52, 14, 0, 0x168e5c5, 9, 32, 47);
    AssignMapStructure(pValues2, 53, 14, 1, 0x168e5c5, 9, 48, 63);
    AssignMapStructure(pValues2, 54, 14, 2, 0x168e5c5, 10, 0, 15);
    AssignMapStructure(pValues2, 55, 14, 3, 0x168e5c5, 10, 16, 31);
    AssignMapStructure(pValues2, 56, 14, 4, 0x168e5c5, 10, 32, 47);
    AssignMapStructure(pValues2, 57, 14, 5, 0x168e5c5, 10, 48, 63);
    AssignMapStructure(pValues2, 58, 14, 6, 0x168e5c5, 12, 0, 15);
    AssignMapStructure(pValues2, 59, 14, 7, 0x168e5c5, 12, 16, 31);
    AssignMapStructure(pValues2, 60, 14, 15, 0x168e5c5, 12, 32, 47);
    AssignMapStructure(pValues2, 61, 14, 8, 0x168e5c5, 12, 48, 63);
    //XB_2
    AssignMapStructure(pValues2, 62, 15, 0, 0x168e5c5, 13, 0, 15);
    AssignMapStructure(pValues2, 63, 15, 1, 0x168e5c5, 13, 16, 31);
    AssignMapStructure(pValues2, 64, 15, 2, 0x168e5c5, 13, 32, 47);
    AssignMapStructure(pValues2, 65, 15, 3, 0x168e5c5, 13, 48, 63);
    AssignMapStructure(pValues2, 66, 15, 4, 0x168e5c5, 14, 0, 15);
    AssignMapStructure(pValues2, 67, 15, 5, 0x168e5c5, 14, 16, 31);
    AssignMapStructure(pValues2, 68, 15, 6, 0x168e5c5, 14, 32, 47);
    AssignMapStructure(pValues2, 69, 15, 7, 0x168e5c5, 14, 48, 63);
    AssignMapStructure(pValues2, 70, 15, 15, 0x168e5c5, 15, 0, 15);
    AssignMapStructure(pValues2, 71, 15, 8, 0x168e5c5, 15, 16, 31);

    UniDbDetectorParameter::CreateDetectorParameter("DCH1", "DCH_mapping", 5, 419, 5, 592, pValues1, kNitems);
    UniDbDetectorParameter::CreateDetectorParameter("DCH2", "DCH_mapping", 5, 419, 5, 1014, pValues2, kNitems);
//    if (pDetectorParameter == NULL)
//        return_error = true;

    // clean memory after work
    delete [] pValues1;
    delete [] pValues2;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
