// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void add_parameter_value()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // set 'on' parameter value (boolean value)
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("DCH1", "on", 1, 77, 1, 77, true);
    if (pDetectorParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    // clean memory after work
    delete pDetectorParameter;

    cout << "\nMacro finished successfully" << endl;
}
