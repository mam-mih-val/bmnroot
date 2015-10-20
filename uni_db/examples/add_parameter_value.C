// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void add_parameter_value()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // add 'on' parameter value (boolean value)
    UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(77, "DCH1", "on", true); //(run_number, detector_name, parameter_name, parameter_value)
    if (pRunParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    // clean memory after work
    delete pRunParameter;

    cout << "\nMacro finished successfully" << endl;
}
