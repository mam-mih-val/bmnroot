#include "../db_structures.h"

// macro for getting parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void get_complex_parameter_value()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

    // get noise parameter values presented by IIStructure: Int+Int (slot:channel)
    UniDbRunParameter* pRunParameter = UniDbRunParameter::GetRunParameter(77, "DCH1", "noise"); //(run_number, detector_name, parameter_name)
    if (pRunParameter != NULL)
    {
        IIStructure* pValues;
        int element_count = 0;
        pRunParameter->GetIIArray(pValues, element_count);

        // YOUR CODE (e.g print values)
        for (int i = 0; i < element_count; i++)
            cout<<"Slot:Channel "<<pValues[i].int_1<<":"<<pValues[i].int_2<<endl;

        // clean memory after work
        delete pValues;
        if (pRunParameter)
            delete pRunParameter;
    }
    else
        return_error = true;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
