void get_lorShift(Int_t runPeriod = 6, Int_t runNumber = 1217) {
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("GEM", "lorentz_shift", runPeriod, runNumber);
    if (pDetectorParameter == NULL)
        return_error = true;

    LorentzShiftStructure* shifts;
    int element_count = 0;
    pDetectorParameter->GetLorentzShiftArray(shifts, element_count);

    for (int i = 0; i < element_count; i++) {
        cout << "Stat. number: " << shifts[i].number << endl;
        for (int j = 0; j < 3; j++)
            cout << shifts[i].ls[j] << endl;
    }
    
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
