#include "../db_structures.h"

// macro for getting parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void get_parameter_value_complex()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

    // get noise parameter values presented by IIStructure: Int+Int (slot:channel)
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("DCH1", "noise", 77); //(detector_name, parameter_name, run_number)
    if (pDetectorParameter != NULL)
    {
        IIStructure* pValues;
        int element_count = 0;
        pDetectorParameter->GetIIArray(pValues, element_count);

        // YOUR CODE (e.g print values)
        for (int i = 0; i < element_count; i++)
            cout<<"Slot:Channel "<<pValues[i].int_1<<":"<<pValues[i].int_2<<endl;

        // clean memory after work
        delete pValues;
        if (pDetectorParameter)
            delete pDetectorParameter;
    }
    else
        return_error = true;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
