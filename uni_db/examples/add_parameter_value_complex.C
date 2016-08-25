// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void add_parameter_value_complex()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    bool return_error = false;

    // add noise parameter value presented by IIStructure: Int+Int (slot:channel)
    IIStructure* pValues = new IIStructure[32];

    int count = 48 - 33 + 1;    // slot: 15, channel: 33-48
    for (int i = 0; i < count; i++)
    {
        pValues[i].int_1 = 15;
        pValues[i].int_2 = 33 + i;
    }
    count = 64 - 49 + 1;        // slot: 16, channel: 49-64
    for (int i = 0; i < count; i++)
    {
        pValues[16+i].int_1 = 16;
        pValues[16+i].int_2 = 49 + i;
    }

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("DCH1", "noise", 1, 77, 1, 77, pValues, 32);
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
