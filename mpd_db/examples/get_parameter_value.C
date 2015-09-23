// macro for getting parameter value (if parameter exist - you could check existing parameters by 'MpdDbParameter::PrintAll()' function)
void get_parameter_value()
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/mpd/mpdloadlibs.C");
    mpdloadlibs(1, 1); // load main libraries

    // get 'on' parameter value (boolean value)
    MpdDbDetectorParameter* pDetectorParameter = MpdDbDetectorParameter::GetDetectorParameter(77, "DCH1", "on"); //(run_number, detector_name, parameter_name)
    if (pDetectorParameter == NULL)
    {
        cout << "\nMacro finished with errors" << endl;
        return;
    }

    bool is_on = pDetectorParameter->GetBool();
    if (is_on)
        cout<<"Detector DCH1 was turned on in run №77"<<endl;
    else
        cout<<"Detector DCH1 was turned off in run №77"<<endl;

    // clean memory after work
    delete pDetectorParameter;

    cout << "\nMacro finished successfully" << endl;
}
