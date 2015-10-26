// macro for setting 'on' detectors for all runs
void set_detector_run_on()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniDbRunParameter* pRunParameter;
    for (int i = 12; i <= 688; i++)
    {
        // add 'on' parameter value for detectors
        pRunParameter = UniDbRunParameter::CreateRunParameter(i, "DCH1", "on", true); //(run_number, detector_name, parameter_name, parameter_value)
        // clean memory
        if (pRunParameter) delete pRunParameter;
        pRunParameter = UniDbRunParameter::CreateRunParameter(i, "DCH2", "on", true);
        // clean memory
        if (pRunParameter) delete pRunParameter;
        pRunParameter = UniDbRunParameter::CreateRunParameter(i, "TOF1", "on", true);
        // clean memory
        if (pRunParameter) delete pRunParameter;
        pRunParameter = UniDbRunParameter::CreateRunParameter(i, "TOF2", "on", true);
        // clean memory
        if (pRunParameter) delete pRunParameter;
        pRunParameter = UniDbRunParameter::CreateRunParameter(i, "ZDC", "on", true);
        // clean memory
        if (pRunParameter) delete pRunParameter;
    }

    cout << "\nMacro finished" << endl;
}
