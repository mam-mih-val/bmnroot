// macro for setting 'on' detectors for all current runs
void set_detector_run_on()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // add 'on' parameter value for detectors
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("DCH1", "on", 12, 688, true);
    // clean memory
    if (pDetectorParameter) delete pDetectorParameter;

    pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("DCH2", "on", 12, 688, true); //(detector_name, parameter_name, start_run_number, end_run_number, parameter_value)
    // clean memory
    if (pDetectorParameter) delete pDetectorParameter;

    pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("TOF1", "on", 12, 688, true);
    // clean memory
    if (pDetectorParameter) delete pDetectorParameter;

    pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("TOF2", "on", 12, 688, true);
    // clean memory
    if (pDetectorParameter) delete pDetectorParameter;

    pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("ZDC", "on", 12, 688, true);
    // clean memory
    if (pDetectorParameter) delete pDetectorParameter;

    cout << "\nMacro finished" << endl;
}
