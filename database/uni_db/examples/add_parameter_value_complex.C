// macro for adding parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void add_parameter_value_complex()
{
    bool return_error = false;

    // add noise parameter value presented by IIStructure: Int+Int (slot:channel)
    vector<UniValue*> pValues;

    int count = 48 - 33 + 1;    // slot: 15, channel: 33-48
    for (int i = 0; i < count; i++)
    {
        IIValue* pValue = new IIValue;
        pValue->value1 = 15;
        pValue->value2 = 33 + i;

        pValues.push_back(pValue);
    }
    count = 64 - 49 + 1;        // slot: 16, channel: 49-64
    for (int i = 0; i < count; i++)
    {
        IIValue* pValue = new IIValue;
        pValue->value1 = 16;
        pValue->value2 = 49 + i;

        pValues.push_back(pValue);
    }

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("DCH1", "noise", 1, 77, 1, 77, pValues);
    if (pDetectorParameter == nullptr)
        return_error = true;

    // clean memory after work
    for (int i = 0; i < pValues.size(); i++) delete pValues[i];
    if (pDetectorParameter)
        delete pDetectorParameter;

    if (return_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
