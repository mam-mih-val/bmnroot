void add_lorShift(Int_t run = 6) {
    bool return_error = false;

    const int nStats = (run == 5) ? 7 : (run == 6) ? 6 : -1;
    LorentzShiftStructure* lorShiftValues = new LorentzShiftStructure[nStats];

    for (int iStat = 0; iStat < nStats; iStat++) {
        lorShiftValues[iStat].number = iStat;

        // Normal voltage, RUN5
        if (run == 5) {
            if (iStat == 0) {
                lorShiftValues[iStat].ls[0] = 0.;
                lorShiftValues[iStat].ls[1] = 0.;
                lorShiftValues[iStat].ls[2] = 0.;
            } else if (iStat == 1) {
                lorShiftValues[iStat].ls[0] = 2.0164;
                lorShiftValues[iStat].ls[1] = -0.5504;
                lorShiftValues[iStat].ls[2] = 0.0399;

            } else if (iStat == 2) {
                lorShiftValues[iStat].ls[0] = -2.9895;
                lorShiftValues[iStat].ls[1] = 0.9824;
                lorShiftValues[iStat].ls[2] = -0.0563;

            } else if (iStat == 3) {
                lorShiftValues[iStat].ls[0] = 0.0011;
                lorShiftValues[iStat].ls[1] = 0.0194;
                lorShiftValues[iStat].ls[2] = -0.0006;

            } else if (iStat == 4) {
                lorShiftValues[iStat].ls[0] = 0.0940;
                lorShiftValues[iStat].ls[1] = 0.0204;
                lorShiftValues[iStat].ls[2] = 0.0024;

            } else if (iStat == 5) {
                lorShiftValues[iStat].ls[0] = 0.0023;
                lorShiftValues[iStat].ls[1] = 0.0559;
                lorShiftValues[iStat].ls[2] = -0.0016;

            } else if (iStat == 6) {
                lorShiftValues[iStat].ls[0] = 0.1027;
                lorShiftValues[iStat].ls[1] = -0.0195;
                lorShiftValues[iStat].ls[2] = 0.0026;
            }
        }

        // Normal voltage, RUN6
        if (run == 6) {
            lorShiftValues[iStat].ls[0] = 0.000250;
            lorShiftValues[iStat].ls[1] = 0.019198;
            lorShiftValues[iStat].ls[2] = 0.000051;

            // Low voltage
            //        lorShiftValues[iStat].ls[0] = 0.024322;
            //        lorShiftValues[iStat].ls[1] = 0.019031;
            //        lorShiftValues[iStat].ls[2] = 0.000505;
        }
    }

    //    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::DeleteDetectorParameter("GEM", "lorentz_shift", 6, 1208, 6, 1992);

    // Low voltage
    //    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("GEM", "lorentz_shift", 6, 1208, 6, 1217, lorShiftValues, nStats);

    // Normal voltage, RUN5
    //UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("GEM", "lorentz_shift", 5, 430, 5, 1014, lorShiftValues, nStats);

    // Normal voltage, RUN6
     UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("GEM", "lorentz_shift", 6, 1219, 6, 1992, lorShiftValues, nStats);
    if (pDetectorParameter == NULL)
        return_error = true;

    // clean memory after work
    delete [] lorShiftValues;
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
