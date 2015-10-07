// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void add_parameter_value()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // add 'on' parameter value (boolean value)
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(77, "DCH1", "on", true); //(run_number, detector_name, parameter_name, parameter_value)
    if (pDetectorParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    // clean memory after work
    delete pDetectorParameter;

    cout << "\nMacro finished successfully" << endl;
}
