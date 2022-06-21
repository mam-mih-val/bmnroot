void AssignMapStructure(vector<UniValue*>& pArray, unsigned int ser, unsigned int slot, int ch)
{
    TriggerMapValue* pValue = new TriggerMapValue;
    pValue->serial = ser;
    pValue->slot = slot;
    pValue->channel = ch;

    pArray.push_back(pValue);
}

void add_TRIG_mapping() {
    bool return_error = false;
    
    //needed only to put new detector in dataBase
    //UniDbDetector* pDetector = UniDbDetector::CreateDetector("BD", nullptr);

    vector<UniValue*> pT0;
    vector<UniValue*> pBC1;
    vector<UniValue*> pBC2;
    vector<UniValue*> pVETO;
    vector<UniValue*> pFD;
    vector<UniValue*> pBD;

    AssignMapStructure(pT0, 0x6EA9711, 21, 33);
    AssignMapStructure(pT0, 0x6EA9711, 21, 42);

    AssignMapStructure(pBC1, 0x6EA9711, 21, 31);

    AssignMapStructure(pBC2, 0x6EA9711, 21, 24);
    AssignMapStructure(pBC2, 0x6EA9711, 21, 34);

    AssignMapStructure(pVETO, 0x6EA9711, 21, 28);
    AssignMapStructure(pVETO, 0x6EA9711, 21, 38);

    AssignMapStructure(pFD, 0x6EA9711, 21, 32);
    AssignMapStructure(pFD, 0x6EA9711, 21, 22);

    AssignMapStructure(pBD, 0x6EA9711, 21, 15);
    AssignMapStructure(pBD, 0x6EA9711, 21, 22);
    AssignMapStructure(pBD, 0x6EA9711, 21, 18);
    AssignMapStructure(pBD, 0x6EA9711, 21, 36);
    AssignMapStructure(pBD, 0x6EA9711, 21, 14);
    AssignMapStructure(pBD, 0x6EA9711, 21, 16);
    AssignMapStructure(pBD, 0x6EA9711, 21, 12);
    AssignMapStructure(pBD, 0x6EA9711, 21, 23);
    AssignMapStructure(pBD, 0x6EA9711, 21, 40);
    AssignMapStructure(pBD, 0x6EA9711, 21, 19);
    AssignMapStructure(pBD, 0x6EA9711, 21, 21);
    AssignMapStructure(pBD, 0x6EA9711, 21, 17);
    AssignMapStructure(pBD, 0x6EA9711, 21, 13);
    AssignMapStructure(pBD, 0x6EA9711, 21, 44);
    AssignMapStructure(pBD, 0x6EA9711, 21, 20);
    AssignMapStructure(pBD, 0x6EA9711, 21, 0);
    AssignMapStructure(pBD, 0x6EA9711, 21, 9);
    AssignMapStructure(pBD, 0x6EA9711, 21, 5);
    AssignMapStructure(pBD, 0x6EA9711, 21, 37);
    AssignMapStructure(pBD, 0x6EA9711, 21, 1);
    AssignMapStructure(pBD, 0x6EA9711, 21, 7);
    AssignMapStructure(pBD, 0x6EA9711, 21, 3);
    AssignMapStructure(pBD, 0x6EA9711, 21, 8);
    AssignMapStructure(pBD, 0x6EA9711, 21, 41);
    AssignMapStructure(pBD, 0x6EA9711, 21, 4);
    AssignMapStructure(pBD, 0x6EA9711, 21, 10);
    AssignMapStructure(pBD, 0x6EA9711, 21, 6);
    AssignMapStructure(pBD, 0x6EA9711, 21, 2);
    AssignMapStructure(pBD, 0x6EA9711, 21, 45);
    AssignMapStructure(pBD, 0x6EA9711, 21, 11);
    AssignMapStructure(pBD, 0x6EA9711, 21, 63);
    AssignMapStructure(pBD, 0x6EA9711, 21, 70);
    AssignMapStructure(pBD, 0x6EA9711, 21, 66);
    AssignMapStructure(pBD, 0x6EA9711, 21, 25);
    AssignMapStructure(pBD, 0x6EA9711, 21, 62);
    AssignMapStructure(pBD, 0x6EA9711, 21, 64);
    AssignMapStructure(pBD, 0x6EA9711, 21, 60);
    AssignMapStructure(pBD, 0x6EA9711, 21, 71);
    AssignMapStructure(pBD, 0x6EA9711, 21, 29);
    AssignMapStructure(pBD, 0x6EA9711, 21, 67);

    Int_t period = 6;
    Int_t startRun = 1;
    Int_t finishRun = 1e4;
    
    UniDbDetectorParameter* pDetectorParameterT0 = UniDbDetectorParameter::CreateDetectorParameter("T0", "T0_global_mapping", period, startRun, period, finishRun, pT0);
    UniDbDetectorParameter* pDetectorParameterBC1 = UniDbDetectorParameter::CreateDetectorParameter("BC1", "BC1_global_mapping", period, startRun, period, finishRun, pBC1);
    UniDbDetectorParameter* pDetectorParameterBC2 = UniDbDetectorParameter::CreateDetectorParameter("BC2", "BC2_global_mapping", period, startRun, period, finishRun, pBC2);
    UniDbDetectorParameter* pDetectorParameterVET0 = UniDbDetectorParameter::CreateDetectorParameter("VETO", "VETO_global_mapping", period, startRun, period, finishRun, pVETO);
    UniDbDetectorParameter* pDetectorParameterBD = UniDbDetectorParameter::CreateDetectorParameter("BD", "BD_global_mapping", period, startRun, period, finishRun, pBD);

    // clean memory after work
    for (int i = 0; i < pT0.size(); i++) delete pT0[i];
    for (int i = 0; i < pBC1.size(); i++) delete pBC1[i];
    for (int i = 0; i < pBC2.size(); i++) delete pBC2[i];
    for (int i = 0; i < pVETO.size(); i++) delete pVETO[i];
    for (int i = 0; i < pFD.size(); i++) delete pFD[i];
    for (int i = 0; i < pBD.size(); i++) delete pBD[i];
    if (pDetectorParameterT0) delete pDetectorParameterT0;
    if (pDetectorParameterBC1) delete pDetectorParameterBC1;
    if (pDetectorParameterBC2) delete pDetectorParameterBC2;
    if (pDetectorParameterVET0) delete pDetectorParameterVET0;
    if (pDetectorParameterBD) delete pDetectorParameterBD;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
