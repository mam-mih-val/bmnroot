// macro for adding new parameter (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
// parameter type: 0 - bool, 1 - int, 2 - double, 3 - string, 4 - int+int array, 5 - int array, 6 - double array
// lifetime type: 0 - store it for every run, 1 - store it for every session
void add_new_parameter()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // add 'voltage' parameter, for example (double value)
    UniDbParameter* pParameter = UniDbParameter::CreateParameter("voltage", 2, 0); //(TString parameter_name, int parameter_type, int lifetime_type)
    if (pParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    // clean memory after work
    delete pParameter;

    cout << "\nMacro finished successfully" << endl;
}
