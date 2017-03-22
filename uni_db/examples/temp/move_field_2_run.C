// macro for moving magnetic field voltage from parameter values to table RUN_
class UniqueRunNumber;
void move_field_2_run(int period)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniqueRunNumber* run_numbers;
    int run_count = UniDbRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    const char* detector_name = "magnet";
    const char* parameter_name = "average_field_mv";

    for (int i = 0; i < run_count; i++)
    {
        // get the average magnetic field from Runs of the Unified Database
        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter(detector_name, parameter_name,
                                                     run_numbers[i].period_number, run_numbers[i].run_number);
        if (pDetectorParameter == NULL)
        {
            cout<<"No field voltage was found for run ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<")"<<endl;
            continue;
        }
        double field_voltage = pDetectorParameter->GetDouble();
        delete pDetectorParameter;

        // get run
        UniDbRun* pRun = UniDbRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == NULL)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }
        pRun->SetFieldVoltage(&field_voltage);
        delete pRun;

        cout<<"Average magnetic field for run "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<": "<<field_voltage<<" mv"<<endl;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}
