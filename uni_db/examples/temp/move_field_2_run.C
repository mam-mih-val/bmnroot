// macro for moving magnetic field voltage from parameter values to table RUN_
class UniqueRunNumber;
void move_field_2_run(int period)
{
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
        double field_voltage = ((DoubleValue*)pDetectorParameter->GetValue())->value;
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

        cout<<"Average magnetic field for run "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<": "<<field_voltage<<" mV"<<endl;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}

void convert_current_2_voltage(int period)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniDbRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    const char* detector_name = "magnet";
    const char* parameter_name = "field_current";

    for (int i = 0; i < run_count; i++)
    {
        // get the field current from parameter values of the Unified Database
        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter(detector_name, parameter_name,
                                                     run_numbers[i].period_number, run_numbers[i].run_number);
        if (pDetectorParameter == NULL)
        {
            cout<<"No field current was found for run ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<")"<<endl;
            continue;
        }
        int field_current = ((IntValue*)pDetectorParameter->GetValue())->value;
        delete pDetectorParameter;

        // convert current to voltage approximately
        double field_voltage = field_current / 900.0 * 55.87;

        // get run
        UniDbRun* pRun = UniDbRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == NULL)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        double* current_voltage = pRun->GetFieldVoltage();
        if (current_voltage == NULL)
        {
            pRun->SetFieldVoltage(&field_voltage);
            cout<<"Field voltage was written for run "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<": "<<field_voltage<<" mV"<<endl;
        }
        else
            cout<<"Field voltage exists for run "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<": "<<*current_voltage<<" mV"<<endl;

        delete pRun;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}
