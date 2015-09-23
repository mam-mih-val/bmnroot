#include "../db_classes/MpdDbParameter.h"

int AssignIIStructure(IIStructure* pIIArray, int start_index, int first_int, int start_second_int, int end_second_int)
{
    int count = end_second_int - start_second_int+1;
    if (count < 0)
    {
        cout<<"Error: end index of second integer parameter should be greater or equal start index"<<endl;
        return -1;
    }

    for (int i = 0; i < count; i++)
    {
        pIIArray[start_index+i].int_1 = first_int;
        pIIArray[start_index+i].int_2 = start_second_int + i;
    }

    return 0;
}

// macro for adding parameter value (if parameter exist - you could check existing parameters by 'MpdDbParameter::PrintAll()' function)
void add_complex_parameter_value()
{
    bool return_error = false;
    gROOT->LoadMacro("$VMCWORKDIR/macro/mpd/mpdloadlibs.C");
    mpdloadlibs(1, 1); // load main libraries

    // add noise parameter value presented by IIStructure: Int+Int (slot:channel)
    IIStructure* pValues = new IIStructure[32];
    //pValues[0].int_1 = 15; pValues[0].int_2 = 33; //etc.
    AssignIIStructure(pValues, 0, 15, 33, 48); // slot: 15, channel: 33-48
    AssignIIStructure(pValues, 16, 16, 49, 64); // slot: 16, channel: 49-64

    MpdDbDetectorParameter* pDetectorParameter = MpdDbDetectorParameter::CreateDetectorParameter(77, "DCH1", "noise", pValues, 32); //(run_number, detector_name, parameter_name, IIStructure_value, element_count)
    if (pDetectorParameter == NULL)
        return_error = true;

    // clean memory after work
    delete [] pValues;
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
