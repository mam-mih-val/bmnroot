// macro for getting parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void get_parameter_value_complex()
{
    bool return_error = false;

    // get noise parameter values presented by IIStructure: Int+Int (slot:channel)
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("DCH1", "noise", 1, 133);
    if (pDetectorParameter != NULL)
    {
        vector<UniValue*> pValues;
        pDetectorParameter->GetValue(pValues);

        // YOUR CODE (e.g print values)
        for (int i = 0; i < pValues.size(); i++)
            cout<<"Slot:Channel "<<((IIValue*)pValues[i])->value1<<":"<<((IIValue*)pValues[i])->value2<<endl;

        // clean memory after work
        for (int i = 0; i < pValues.size(); i++) delete pValues[i];
        if (pDetectorParameter)
            delete pDetectorParameter;
    }
    else
        return_error = true;

    if (return_error)
        cout<<"\nMacro finished with errors"<<endl;
    else
        cout<<"\nMacro finished successfully"<<endl;
}
