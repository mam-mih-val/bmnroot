// macro for setting 'on' detectors for all runs
void set_detector_run_on()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniDbDetectorParameter* pDetectorParameter;
    for (int i = 12; i <= 688; i++)
    {
        // add 'on' parameter value for detectors
        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(i, "DCH1", "on", true); //(run_number, detector_name, parameter_name, parameter_value)
        // clean memory
        if (pDetectorParameter) delete pDetectorParameter;
        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(i, "DCH2", "on", true);
        // clean memory
        if (pDetectorParameter) delete pDetectorParameter;
        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(i, "TOF1", "on", true);
        // clean memory
        if (pDetectorParameter) delete pDetectorParameter;
        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(i, "TOF2", "on", true);
        // clean memory
        if (pDetectorParameter) delete pDetectorParameter;
        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(i, "ZDC", "on", true);
        // clean memory
        if (pDetectorParameter) delete pDetectorParameter;
    }

    cout << "\nMacro finished" << endl;
}
