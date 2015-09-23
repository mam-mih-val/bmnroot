// macro for adding new parameter (if parameter exist - you could check existing parameters by 'MpdDbParameter::PrintAll()' function)
// parameter type: 0 - bool, 1-int, 2 - double, 3 - string, 4 - int+int array
void add_new_parameter()
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/mpd/mpdloadlibs.C");
    mpdloadlibs(1, 1); // load main libraries

    // add 'voltage' parameter, for example (double value)
    MpdDbParameter* pParameter = MpdDbParameter::CreateParameter("voltage", 2); //(TString parameter_name, int parameter_type)
    if (pParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    // clean memory after work
    delete pParameter;

    cout << "\nMacro finished successfully" << endl;
}
