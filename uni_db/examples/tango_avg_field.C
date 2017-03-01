// macro for getting average magnetic field for 930 run (5-th session)
void tango_avg_field(int period = 5, int run = 930)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniDbTangoData db_tango;

    // get run time (period 5, run 930)
    UniDbRun* pRun = UniDbRun::GetRun(period, run);
    if (pRun == NULL)
    {
        cout<<"Macro finished with errors: no experimental run was found for given numbers"<<endl;
        exit(-1);
    }

    TString strDateStart = pRun->GetStartDatetime().AsSQLString();
    TDatime* dateEnd = pRun->GetEndDatetime();
    if (dateEnd == NULL)
    {
        cout<<"Macro finished with errors: no end datetime in the database for this run"<<endl;
        delete pRun;
        exit(-2);
    }
    TString strDateEnd = dateEnd->AsSQLString();
    delete pRun;

    const char* detector_name = "bmn";
    const char* parameter_name = "ch1";

    TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, strDateStart.Data(), strDateEnd.Data());
    if (tango_data == NULL)
    {
        cout<<"Macro finished with errors: return data is null"<<endl;
        exit(-3);
    }

    vector<double> vec_average = db_tango.GetAverageTangoData(tango_data);
    if (vec_average.empty())
    {
        cout<<"Macro finished with errors: Tango data is empty or average value is wrong"<<endl;
        exit(-4);
    }
    double average_field = vec_average[0];
    delete tango_data;

    cout<<"Average magnetic field for run "<<period<<"-"<<run<<": "<<average_field<<" mv"<<endl;
    cout<<"Macro finished successfully"<<endl;
}


class UniqueRunNumber;
// additional function for getting and writing average magnetic field for all runs of the given period/session
void tango_avg_field_write_db(int period = 5)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniqueRunNumber* run_numbers;
    int run_count = UniDbRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    for (int i = 0; i < run_count; i++)
    {
        UniDbTangoData db_tango;

        // get run time
        UniDbRun* pRun = UniDbRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == NULL)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        TString strDateStart = pRun->GetStartDatetime().AsSQLString();
        TDatime* dateEnd = pRun->GetEndDatetime();
        if (dateEnd == NULL)
        {
            cout<<"The function encountered with errors: no end datetime in the database ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete pRun;
            continue;
        }
        TString strDateEnd = dateEnd->AsSQLString();
        delete pRun;

        const char* detector_name = "bmn";
        const char* parameter_name = "ch1";

        TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, strDateStart.Data(), strDateEnd.Data());
        if (tango_data == NULL)
        {
            cout<<"The function encountered with errors: return data is null ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        vector<double> vec_average = db_tango.GetAverageTangoData(tango_data);
        if (vec_average.empty())
        {
            cout<<"The function encountered with errors: Tango data is empty or average value is wrong ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete tango_data;
            continue;
        }

        double average_field = vec_average[0];
        delete tango_data;

        cout<<"Average magnetic field for run "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<": "<<average_field<<" mv"<<endl;

        // check if magnet exists in the database
        if (!UniDbDetector::CheckDetectorExists("magnet"))
        {
            UniDbDetector* pDetector = UniDbDetector::CreateDetector("magnet", NULL);
            if (pDetector == NULL)
            {
                cout << "\nMacro finished with errors" << endl;
                return;
            }

            cout<<"The magnet was added to detectors in the database"<<endl;
            delete pDetector;
        }

        // check if 'average_field_mv' parameter exists in the database
        if (!UniDbParameter::CheckParameterExists("average_field_mv"))
        {
            UniDbParameter* pParameter = UniDbParameter::CreateParameter("average_field_mv", DoubleType);
            if (pParameter == NULL)
            {
                cout << "\nMacro finished with errors" << endl;
                return;
            }

            cout<<"The 'average_field_mv' parameter was added to the database"<<endl;
            delete pParameter;
        }

        // writing the average magnetic field to the Unified Database
        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("magnet", "average_field_mv",
                                                     run_numbers[i].period_number, run_numbers[i].run_number, run_numbers[i].period_number, run_numbers[i].run_number, average_field);
        if (pDetectorParameter == NULL)
        {
            cout << "\nMacro finished with errors" << endl;
            return;
        }

        cout<<"Average magnetic field was written to the database"<<endl;
        delete pDetectorParameter;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}

// additional function to compare magnetic field value (current, A) of the run info and average magnetic field (voltage, mv) from the Tango
void compare_avg_field(int period = 5)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniqueRunNumber* run_numbers;
    int run_count = UniDbRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    for (int i = 0; i < run_count; i++)
    {
        // get run
        UniDbRun* pRun = UniDbRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == NULL)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        int* dFieldCurrent = pRun->GetFieldCurrent();
        delete pRun;

        // read the average magnetic field from the Unified Database
        double* dAvgFieldVoltage = NULL;
        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter("magnet", "average_field_mv", run_numbers[i].period_number, run_numbers[i].run_number);

        if (pDetectorParameter != NULL)
            dAvgFieldVoltage = new double(pDetectorParameter->GetDouble());
        delete pDetectorParameter;

        cout<<"Run "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<endl;
        if (dFieldCurrent)
            cout<<"Magnetic field current from detector group: "<<*dFieldCurrent<<" A"<<endl;
        else
            cout<<"WARNING: magnetic field current from detector group was not set in the database"<<endl;
        if (dAvgFieldVoltage)
            cout<<"Average magnetic field voltage from Tango: "<<*dAvgFieldVoltage<<" mv"<<endl<<endl;
        else
            cout<<"WARNING: average magnetic field voltage was not calculated from Tango"<<endl<<endl;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}
