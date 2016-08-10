#include "../db_structures.h"

// macro for adding new parameter (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void add_new_parameter()
{
    // load basic and UniDb libraries
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // add 'voltage' parameter, for example (double value)
    UniDbParameter* pParameter = UniDbParameter::CreateParameter("DCH_mapping", DchMapArrayType); //(TString parameter_name, int parameter_type)
    if (pParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    // clean memory after work
    delete pParameter;

    cout << "\nMacro finished successfully" << endl;
}
