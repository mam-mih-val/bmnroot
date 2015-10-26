#include "../db_structures.h"

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

// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void add_parameter_value_complex()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

    // add noise parameter value presented by IIStructure: Int+Int (slot:channel)
    IIStructure* pValues = new IIStructure[32];
    //pValues[0].int_1 = 15; pValues[0].int_2 = 33; //etc.
    AssignIIStructure(pValues, 0, 15, 33, 48); // slot: 15, channel: 33-48
    AssignIIStructure(pValues, 16, 16, 49, 64); // slot: 16, channel: 49-64

    UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(77, "DCH1", "noise", pValues, 32); //(run_number, detector_name, parameter_name, IIStructure_value, element_count)
    if (pRunParameter == NULL)
        return_error = true;

    // clean memory after work
    delete [] pValues;
    if (pRunParameter)
        delete pRunParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
