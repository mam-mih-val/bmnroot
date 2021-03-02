void get_lorShift(Int_t runPeriod = 6, Int_t runNumber = 1217) {
    bool return_error = false;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("GEM", "lorentz_shift", runPeriod, runNumber);
    if (pDetectorParameter == NULL)
        return_error = true;

    vector<UniValue*> shifts;
    pDetectorParameter->GetValue(shifts);

    for (int i = 0; i < shifts.size(); i++) {
        LorentzShiftValue* pValue = (LorentzShiftValue*) shifts[i];
        cout << "Stat. number: " << pValue->number << endl;
        for (int j = 0; j < 3; j++)
            cout << pValue->ls[j] << endl;
    }
    
    // clean memory
    for (int i = 0; i < shifts.size(); i++) delete shifts[i];
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
