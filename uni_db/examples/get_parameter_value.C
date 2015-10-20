// macro for getting parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void get_parameter_value()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // get 'on' parameter value (boolean value)
    UniDbRunParameter* pRunParameter = UniDbRunParameter::GetRunParameter(77, "DCH1", "on"); //(run_number, detector_name, parameter_name)
    if (pRunParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    bool is_on = pRunParameter->GetBool();
    if (is_on)
        cout<<"Detector DCH1 was turned on in run n.77"<<endl;
    else
        cout<<"Detector DCH1 was turned off in run n.77"<<endl;

    // clean memory after work
    delete pRunParameter;

    cout << "\nMacro finished successfully" << endl;
}
