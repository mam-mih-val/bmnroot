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

    bool is_on = pDetectorParameter->GetBool();
    if (is_on)
        cout<<"Detector DCH1 was turned on in run n.77"<<endl;
    else
        cout<<"Detector DCH1 was turned off in run n.77"<<endl;

    // clean memory after work
    delete pDetectorParameter;

    cout << "\nMacro finished successfully" << endl;
}
