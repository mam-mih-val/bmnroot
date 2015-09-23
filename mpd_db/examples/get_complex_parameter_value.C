#include "../db_classes/MpdDbParameter.h"

// macro for getting parameter value (if parameter exist - you could check existing parameters by 'MpdDbParameter::PrintAll()' function)
void get_complex_parameter_value()
{
    bool return_error = false;
    gROOT->LoadMacro("$VMCWORKDIR/macro/mpd/mpdloadlibs.C");
    mpdloadlibs(1, 1); // load main libraries

    // get noise parameter values presented by IIStructure: Int+Int (slot:channel)
    MpdDbDetectorParameter* pDetectorParameter = MpdDbDetectorParameter::GetDetectorParameter(77, "DCH1", "noise"); //(run_number, detector_name, parameter_name)
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
