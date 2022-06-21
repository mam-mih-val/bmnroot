// macro for getting file ID corresponding the given run number
void get_slewing_file_id(int period_number, int run_number)
{
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("TOF2", "slewing_file_id", period_number, run_number); //(detector_name, parameter_name, period_number, run_number)
    if (pDetectorParameter == nullptr)
    {
        cout<<"There is no File ID for "<<period_number<<":"<<run_number<<endl;
        return;
    }

    int file_id = ((IntValue*)pDetectorParameter->GetValue())->value;
    cout<<"File ID for "<<period_number<<":"<<run_number<<": "<<file_id<<endl;

    // clean memory after work
    delete pDetectorParameter;
}
