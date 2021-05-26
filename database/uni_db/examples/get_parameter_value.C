// macro for getting parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void get_parameter_value()
{
    // get 'on' parameter value (boolean value)
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("DCH1", "on", 1, 77); //(detector_name, parameter_name, period_number, run_number)
    if (pDetectorParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    BoolValue* is_on = (BoolValue*) pDetectorParameter->GetValue();
    if (is_on->value)
        cout<<"Detector DCH1 was turned on in run n.77"<<endl;
    else
        cout<<"Detector DCH1 was turned off in run n.77"<<endl;

    // clean memory after work
    delete is_on;
    delete pDetectorParameter;

    cout << "\nMacro finished successfully" << endl;
}
