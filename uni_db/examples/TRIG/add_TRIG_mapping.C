
class TriggerMapStructure;

int AssignMapStructure(TriggerMapStructure* pArray, unsigned int ser, unsigned int slot, int ch, int id) {
    pArray[id].serial = ser;
    pArray[id].slot = slot;
    pArray[id].channel = ch;
    return 0;
}

void add_TRIG_mapping() {
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;
    
    //needed only to put new detector in dataBase
    //UniDbDetector* pDetector = UniDbDetector::CreateDetector("BD", NULL);

    TriggerMapStructure* pT0 = new TriggerMapStructure();
    TriggerMapStructure* pBC1 = new TriggerMapStructure();
    TriggerMapStructure* pBC2 = new TriggerMapStructure();
    TriggerMapStructure* pVETO = new TriggerMapStructure();
    TriggerMapStructure* pFD = new TriggerMapStructure();

    const UInt_t kNBDCH = 40;
    TriggerMapStructure* pBD = new TriggerMapStructure[kNBDCH];

    AssignMapStructure(pT0, 0x6EA9711, 21, 33, 0);
    AssignMapStructure(pBC1, 0x6EA9711, 21, -1, 0);
    AssignMapStructure(pBC2, 0x6EA9711, 21, 24, 0);
    AssignMapStructure(pVETO, 0x6EA9711, 21, 28, 0);
    AssignMapStructure(pFD, 0x6EA9711, 21, 32, 0);

    AssignMapStructure(pBD, 0x6EA9711, 21, 15, 0);
    AssignMapStructure(pBD, 0x6EA9711, 21, 22, 1);
    AssignMapStructure(pBD, 0x6EA9711, 21, 18, 2);
    AssignMapStructure(pBD, 0x6EA9711, 21, 36, 3);
    AssignMapStructure(pBD, 0x6EA9711, 21, 14, 4);
    AssignMapStructure(pBD, 0x6EA9711, 21, 16, 5);
    AssignMapStructure(pBD, 0x6EA9711, 21, 12, 6);
    AssignMapStructure(pBD, 0x6EA9711, 21, 23, 7);
    AssignMapStructure(pBD, 0x6EA9711, 21, 40, 8);
    AssignMapStructure(pBD, 0x6EA9711, 21, 19, 9);
    AssignMapStructure(pBD, 0x6EA9711, 21, 21, 10);
    AssignMapStructure(pBD, 0x6EA9711, 21, 17, 11);
    AssignMapStructure(pBD, 0x6EA9711, 21, 13, 12);
    AssignMapStructure(pBD, 0x6EA9711, 21, 44, 13);
    AssignMapStructure(pBD, 0x6EA9711, 21, 20, 14);
    AssignMapStructure(pBD, 0x6EA9711, 21, 0, 15);
    AssignMapStructure(pBD, 0x6EA9711, 21, 9, 16);
    AssignMapStructure(pBD, 0x6EA9711, 21, 5, 17);
    AssignMapStructure(pBD, 0x6EA9711, 21, 37, 18);
    AssignMapStructure(pBD, 0x6EA9711, 21, 1, 19);
    AssignMapStructure(pBD, 0x6EA9711, 21, 7, 20);
    AssignMapStructure(pBD, 0x6EA9711, 21, 3, 21);
    AssignMapStructure(pBD, 0x6EA9711, 21, 8, 22);
    AssignMapStructure(pBD, 0x6EA9711, 21, 41, 23);
    AssignMapStructure(pBD, 0x6EA9711, 21, 4, 24);
    AssignMapStructure(pBD, 0x6EA9711, 21, 10, 25);
    AssignMapStructure(pBD, 0x6EA9711, 21, 6, 26);
    AssignMapStructure(pBD, 0x6EA9711, 21, 2, 27);
    AssignMapStructure(pBD, 0x6EA9711, 21, 45, 28);
    AssignMapStructure(pBD, 0x6EA9711, 21, 11, 29);
    AssignMapStructure(pBD, 0x6EA9711, 21, 63, 30);
    AssignMapStructure(pBD, 0x6EA9711, 21, 70, 31);
    AssignMapStructure(pBD, 0x6EA9711, 21, 66, 32);
    AssignMapStructure(pBD, 0x6EA9711, 21, 25, 33);
    AssignMapStructure(pBD, 0x6EA9711, 21, 62, 34);
    AssignMapStructure(pBD, 0x6EA9711, 21, 64, 35);
    AssignMapStructure(pBD, 0x6EA9711, 21, 60, 36);
    AssignMapStructure(pBD, 0x6EA9711, 21, 71, 37);
    AssignMapStructure(pBD, 0x6EA9711, 21, 29, 38);
    AssignMapStructure(pBD, 0x6EA9711, 21, 67, 39);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("T0", "T0_global_mapping", 5, 419, 5, 1e4, pT0, 1);
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("BC1", "BC1_global_mapping", 5, 419, 5, 1e4, pBC1, 1);
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("BC2", "BC2_global_mapping", 5, 419, 5, 1e4, pBC2, 1);
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("VETO", "VETO_global_mapping", 5, 419, 5, 1e4, pVETO, 1);
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("BD", "BD_global_mapping", 5, 419, 5, 1e4, pBD, kNBDCH);

    // clean memory after work
    delete [] pT0;
    delete [] pBC1;
    delete [] pBC2;
    delete [] pVETO;
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
