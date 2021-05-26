void add_lorShift(Int_t run = 6)
{
    bool return_error = false;

    const int nStats = (run == 5) ? 7 : (run == 6) ? 6 : -1;
    vector<UniValue*> lorShiftValues;

    for (int iStat = 0; iStat < nStats; iStat++)
    {
        LorentzShiftValue* pValue = new LorentzShiftValue;
        pValue->number = iStat;

        // Normal voltage, RUN5
        if (run == 5) {
            if (iStat == 0) {
                pValue->ls[0] = 0.;
                pValue->ls[1] = 0.;
                pValue->ls[2] = 0.;
            } else if (iStat == 1) {
                pValue->ls[0] = 2.0164;
                pValue->ls[1] = -0.5504;
                pValue->ls[2] = 0.0399;

            } else if (iStat == 2) {
                pValue->ls[0] = -2.9895;
                pValue->ls[1] = 0.9824;
                pValue->ls[2] = -0.0563;

            } else if (iStat == 3) {
                pValue->ls[0] = 0.0011;
                pValue->ls[1] = 0.0194;
                pValue->ls[2] = -0.0006;

            } else if (iStat == 4) {
                pValue->ls[0] = 0.0940;
                pValue->ls[1] = 0.0204;
                pValue->ls[2] = 0.0024;

            } else if (iStat == 5) {
                pValue->ls[0] = 0.0023;
                pValue->ls[1] = 0.0559;
                pValue->ls[2] = -0.0016;

            } else if (iStat == 6) {
                pValue->ls[0] = 0.1027;
                pValue->ls[1] = -0.0195;
                pValue->ls[2] = 0.0026;
            }
        }

        // Normal voltage, RUN6
        if (run == 6) {
            pValue->ls[0] = 0.000250;
            pValue->ls[1] = 0.019198;
            pValue->ls[2] = 0.000051;

            // Low voltage
            //        pValue->ls[0] = 0.024322;
            //        pValue->ls[1] = 0.019031;
            //        pValue->ls[2] = 0.000505;
        }

        lorShiftValues.push_back(pValue);
    }

    //    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::DeleteDetectorParameter("GEM", "lorentz_shift", 6, 1208, 6, 1992);

    // Low voltage
    //    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("GEM", "lorentz_shift", 6, 1208, 6, 1217, lorShiftValues);

    // Normal voltage, RUN5
    //UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("GEM", "lorentz_shift", 5, 430, 5, 1014, lorShiftValues);

    // Normal voltage, RUN6
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("GEM", "lorentz_shift", 6, 1219, 6, 1992, lorShiftValues);
    if (pDetectorParameter == NULL)
        return_error = true;

    // clean memory after work
    for (int i = 0; i < lorShiftValues.size(); i++) delete lorShiftValues[i];
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
