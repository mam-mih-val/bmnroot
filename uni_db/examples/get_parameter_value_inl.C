// function: convert integer (hexadecimal value) to string with hexadecimal presentation without "0x"
string int_to_hex_string(int int_number)
{
    stringstream stream;
    stream<<std::hex<<int_number;
    return stream.str();
}

// macro for getting parameter value (if parameter exist - you could check existing parameters by 'UniDbParameter::PrintAll()' function)
void get_parameter_value_inl()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    // get 'INL' parameter value (int array) for 72 channels
    bool is_error = false;
    int TDC_SERIAL = (int)0x0168fdca;

    int channel_count = UniDbDetectorParameter::GetChannelCount("TOF1", "inl", 12, TDC_SERIAL); //(detector_name, parameter_name, run_number, dc_serial)
    if (channel_count == 0)
        cout<<"The detector parameter wasn't found"<<endl;
    else
        cout<<"Channel number is equal "<<channel_count<<endl;

    for (int i = 1; i <= channel_count; i++)
    {
        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("TOF1", "inl", 12, TDC_SERIAL, i); //(detector_name, parameter_name, run_number, dc_serial, channel)
        if (pDetectorParameter == NULL)
        {
            is_error = true;
            continue;
        }

        int inl_size = -1;
        double* inl_for_channel = NULL;
        int res_code = pDetectorParameter->GetDoubleArray(inl_for_channel, inl_size);
        if (res_code != 0)
        {
            is_error = true;
            continue;
        }

        cout<<"TDC: "<<int_to_hex_string(TDC_SERIAL)<<". Channel: "<<i<<endl<<"INL:";
        for (int j = 0; j < inl_size; j++)
            cout<<" "<<inl_for_channel[j];
        cout<<endl<<endl;

        // clean memory after work
        delete pDetectorParameter;
        delete [] inl_for_channel;
    }

    if (is_error)
        cout << "\nMacro finished with errors" << endl;
    else
        cout << "\nMacro finished successfully" << endl;
}
