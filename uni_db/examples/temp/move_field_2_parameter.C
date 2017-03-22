// macro for moving magnetic field current from table RUN_ to parameter values
class UniqueRunNumber;
void move_field_2_parameter(int period)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniqueRunNumber* run_numbers = NULL;
    int run_count = UniDbRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    const char* detector_name = "magnet";
    const char* parameter_name = "field_current";

    for (int i = 0; i < run_count; i++)
    {
        // get run time
        UniDbRun* pRun = UniDbRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == NULL)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        int* field_current = pRun->GetFieldCurrent();
        if (field_current == NULL)
        {
            cout<<"No field current in the database for this run ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete pRun;
            continue;
        }
        delete pRun;

        // check if magnet exists in the database
        if (!UniDbDetector::CheckDetectorExists(detector_name))
        {
            UniDbDetector* pDetector = UniDbDetector::CreateDetector(detector_name, NULL);
            if (pDetector == NULL)
            {
                cout << "\nMacro finished with errors" << endl;
                return;
            }

            cout<<"The magnet was added to detectors in the database"<<endl;
            delete pDetector;
        }

        // check if 'field_current' parameter exists in the database
        if (!UniDbParameter::CheckParameterExists(parameter_name))
        {
            UniDbParameter* pParameter = UniDbParameter::CreateParameter(parameter_name, DoubleType);
            if (pParameter == NULL)
            {
                cout << "\nMacro finished with errors" << endl;
                return;
            }

            cout<<"The 'average_field_mv' parameter was added to the database"<<endl;
            delete pParameter;
        }

        // writing magnetic field current as parameter to the Unified Database
        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,
                                                     run_numbers[i].period_number, run_numbers[i].run_number, run_numbers[i].period_number, run_numbers[i].run_number, *field_current);
        if (pDetectorParameter == NULL)
        {
            cout << "\nMacro finished with errors" << endl;
            return;
        }

        cout<<"Field current was written to the database ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<")"<<endl;
        delete pDetectorParameter;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}

void print_parameter(int period)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniqueRunNumber* run_numbers = NULL;
    int run_count = UniDbRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    const char* detector_name = "magnet";
    const char* parameter_name = "field_current";

    for (int i = 0; i < run_count; i++)
    {
        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter(detector_name, parameter_name,
                                                     run_numbers[i].period_number, run_numbers[i].run_number);
        if (pDetectorParameter == NULL)
        {
            cout<<"No field current was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<")"<<endl;
            continue;
        }

        cout<<"Field current for run "<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<" "<<pDetectorParameter->GetInt()<<endl;
        delete pDetectorParameter;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}
