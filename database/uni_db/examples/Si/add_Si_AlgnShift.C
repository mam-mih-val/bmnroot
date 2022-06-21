
void PutAlign(Int_t st, Int_t mod, Double_t x, Double_t y, Double_t z, vector<UniValue*>& values);

void add_Si_AlgnShift(Int_t run = 7) {
    bool return_error = false;
    vector<UniValue*> shiftValues;

    PutAlign(0, 0, -0.124, -0.026, -0.017, shiftValues);
    PutAlign(0, 1, +0.006, -0.010, -0.010, shiftValues);
    PutAlign(0, 2, +0.001, -0.000, -0.001, shiftValues);
    PutAlign(0, 3, -0.006, -0.003, +0.002, shiftValues);
    PutAlign(1, 0, -0.101, +0.015, +0.008, shiftValues);
    PutAlign(1, 1, +0.044, +0.033, +0.009, shiftValues);
    PutAlign(2, 0, +0.042, +0.094, +0.044, shiftValues);
    PutAlign(2, 1, -0.042, +0.051, +0.037, shiftValues);
    PutAlign(2, 2, +0.046, +0.039, +0.023, shiftValues);
    PutAlign(2, 3, -0.063, +0.008, +0.039, shiftValues);
    PutAlign(2, 4, -0.004, +0.003, +0.001, shiftValues);
    PutAlign(2, 5, +0.000, +0.000, -0.001, shiftValues);
    PutAlign(2, 6, +0.000, +0.000, +0.000, shiftValues);
    PutAlign(2, 7, +0.000, +0.000, +0.000, shiftValues);


    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("Silicon", "alignment_shift", run, 3681, run, 5184, shiftValues);
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
