
void PutAlign(Int_t st, Int_t mod, Double_t x, Double_t y, Double_t z, vector<UniValue*>& values);

void add_GEM_AlgnShift(Int_t run = 7) {
    bool return_error = false;
    vector<UniValue*> shiftValues;

    //FOR BM@N RUNS (3681 - 5184)
    // PutAlign(0, 0, -0.018, -0.002, -0.024, shiftValues);
    // PutAlign(0, 1, +0.152, -0.070, -0.045, shiftValues);
    // PutAlign(1, 0, +0.002, -0.153, -0.086, shiftValues);
    // PutAlign(1, 1, +0.145, +0.046, -0.047, shiftValues);
    // PutAlign(2, 0, +0.065, -0.064, +0.036, shiftValues);
    // PutAlign(2, 1, +0.031, +0.056, +0.038, shiftValues);
    // PutAlign(3, 0, -0.123, +0.001, -0.002, shiftValues);
    // PutAlign(3, 1, -0.111, -0.196, -0.027, shiftValues);
    // PutAlign(4, 0, +0.076, -0.131, -0.006, shiftValues);
    // PutAlign(4, 1, +0.022, +0.154, +0.026, shiftValues);
    // PutAlign(5, 0, -0.041, +0.171, +0.007, shiftValues);
    // PutAlign(5, 1, +0.000, -0.017, -0.005, shiftValues);
    
    //FOR SRC RUNS (2213 - 3588)
    // For SRC corrections are sum of alignment corrections and lorentz shift corrections, for fieldscale = 2
    PutAlign(0, 0, 0.0, 0.0, 0.0, shiftValues);
    PutAlign(0, 0, 0.0, 0.0, 0.0, shiftValues);
    PutAlign(1, 0, 0.0, 0.0, 0.0, shiftValues);
    PutAlign(1, 0, 0.0, 0.0, 0.0, shiftValues);
    PutAlign(2, 0, 0.0, 0.0, 0.0, shiftValues);
    PutAlign(2, 0, 0.0, 0.0, 0.0, shiftValues);
    PutAlign(3, 0, 0.0, 0.0, 0.0, shiftValues);
    PutAlign(3, 0, 0.0, 0.0, 0.0, shiftValues);
    PutAlign(4, 0, -0.033, +0.0, 0.0, shiftValues);
    PutAlign(4, 1, -0.033, +0.0, 0.0, shiftValues);
    PutAlign(5, 0, +0.183, -0.027, 0.0, shiftValues);
    PutAlign(5, 1, +0.183, -0.027, 0.0, shiftValues);
    PutAlign(6, 0, -0.231, +0.067, 0.0, shiftValues);
    PutAlign(6, 1, -0.231, +0.067, 0.0, shiftValues);
    PutAlign(7, 0, +0.167, +0.035, 0.0, shiftValues);
    PutAlign(7, 1, +0.167, +0.035, 0.0, shiftValues);
    PutAlign(8, 0, -0.252, -0.001, 0.0, shiftValues);
    PutAlign(8, 1, -0.252, -0.001, 0.0, shiftValues);
    PutAlign(9, 0, +0.156, +0.023, 0.0, shiftValues);
    PutAlign(9, 1, +0.156, +0.023, 0.0, shiftValues);


    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("GEM", "alignment_shift", run, 2214, run, 3588, shiftValues);
    if (pDetectorParameter == nullptr)
        return_error = true;
    // clean memory after work
    for (int i = 0; i < shiftValues.size(); i++) delete shiftValues[i];
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}

void PutAlign(Int_t st, Int_t mod, Double_t x, Double_t y, Double_t z, vector<UniValue*>& values) {
    AlignmentValue* pValue = new AlignmentValue;
    pValue->station = st;
    pValue->module = mod;
    pValue->value[0] = x;
    pValue->value[1] = y;
    pValue->value[2] = z;
    values.push_back(pValue);    
}
