// function for getting average magnetic field for a given run (1886 run of 6-th period by default)
// returns average magnetic field coefficient (in case of errors the return value <= -1)
double tango_avg_field(int period = 6, int run = 1886)
{
    TangoData db_tango;

    // get run time
    UniRun* pRun = UniRun::GetRun(period, run);
    if (pRun == nullptr)
    {
        cout<<"Macro finished with errors: no experimental run was found for given numbers"<<endl;
        return -1;
    }

    TDatime dateStart = pRun->GetStartDatetime();
    TString strDateStart = dateStart.AsSQLString();

    TDatime* dateEnd = pRun->GetEndDatetime();
    if (dateEnd == nullptr)
    {
        cout<<"The function encountered with errors: no end datetime in the database ("<<period<<":"<<run<<"). This run will be skipped!"<<endl;
        delete pRun;
        return -2;
    }
    TString strDateEnd = dateEnd->AsSQLString();
    delete pRun;

    const char* detector_name = "bmn";
    const char* parameter_name = "ch1";

    TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, strDateStart.Data(), strDateEnd.Data());
    if (tango_data == nullptr)
    {
        cout<<"Macro finished with errors: return data is null"<<endl;
        return -3;
    }

    vector<double> vec_average = db_tango.GetAverageTangoData(tango_data);
    if (vec_average.empty())
    {
        cout<<"Macro finished with errors: Tango data is empty or average value is wrong"<<endl;
        delete tango_data;
        return -4;
    }
    double average_field = vec_average[0];
    size_t data_count = tango_data->GetEntriesFast();
    delete tango_data;

    double average_current = average_field * 900 / 55.87;
    double average_coeff = average_field / 55.87;
    cout<<"Average magnetic field from TangoDB for run "<<period<<"-"<<run<<": "<<average_field<<" mV. Calculated current: "
        <<average_current<<" A. Ratio: "<<average_coeff<<". Number of counts: "<<data_count<<endl<<"Macro finished successfully"<<endl;

    return average_coeff;
}

//class UniqueRunNumber;
// additional function for calculating Tango average magnetic field for all runs of the given period/session and writing it to the BM@N Database
void tango_avg_field_write_db(int period = 7)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRunPeriod::GetRunNumbers(period, run_numbers);
    if (run_count <= 0)
        return;

    for (int i = 0; i < run_count; i++)
    {
        TangoData db_tango;

        // get run time
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        TDatime dateStart = pRun->GetStartDatetime();
        TString strDateStart = dateStart.AsSQLString();

        TDatime* dateEnd = pRun->GetEndDatetime();
        if (dateEnd == nullptr)
        {
            cout<<"The function encountered with errors: no end datetime in the database ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete pRun;
            continue;
        }
        TString strDateEnd = dateEnd->AsSQLString();

        const char* detector_name = "bmn";
        const char* parameter_name = "ch1";

        TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, strDateStart.Data(), strDateEnd.Data());
        if (tango_data == nullptr)
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
        size_t data_count = tango_data->GetEntriesFast();
        delete tango_data;

        cout<<"Average magnetic field for run "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<": "<<average_field
            <<" mV ("<<data_count<<" counts)."<<endl;

        // write average magnetic field to run_ in DB
        pRun->SetFieldVoltage(&average_field);

        cout<<"Average magnetic field was written to the database"<<endl;
        delete pRun;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}

// additional function for displaying  magnetic field for the given period of tjme from start run to end run
// if period_end == -1 and run_end == -1 then only magnetic field for one run will be showed
// if period_end == -1 and run_end != -1 then the end period is the same as begin period
int show_field_graph(int period_begin = 6, int run_begin = 1886, int period_end = -1, int run_end = -1)
{
    TangoData db_tango;

    // get time of the begin run
    UniRun* pRunBegin = UniRun::GetRun(period_begin, run_begin);
    if (pRunBegin == nullptr)
    {
        cout<<"Macro finished with errors: no experimental run was found - "<<period_begin<<":"<<run_begin<<" (period:run)"<<endl;
        return -1;
    }

    TDatime dateStart = pRunBegin->GetStartDatetime();
    TString strDateStart = dateStart.AsSQLString();
    cout<<"strDateStart: "<<strDateStart.Data()<<endl;

    TDatime* dateEnd = nullptr;
    if ((period_end == -1) && (run_end == -1))
        dateEnd = pRunBegin->GetEndDatetime();
    else
    {
        if (period_end == -1) period_end = period_begin;

        // get time of the end run
        UniRun* pRunEnd = UniRun::GetRun(period_end, run_end);
        if (pRunEnd == nullptr)
        {
            cout<<"Macro finished with errors: no experimental run was found - "<<period_end<<":"<<run_end<<" (period:run)"<<endl;
            delete pRunBegin;
            return -1;
        }
        dateEnd = pRunEnd->GetEndDatetime();
        delete pRunEnd;
    }

    if (dateEnd == nullptr)
    {
        cout<<"Macro finished with errors: no end datetime in the database for this run"<<endl;
        delete pRunBegin;
        return -2;
    }
    TString strDateEnd = dateEnd->AsSQLString();
    cout<<"strDateEnd: "<<strDateEnd.Data()<<endl;
    delete pRunBegin;

    // get Tango data
    const char* detector_name = "bmn";
    const char* parameter_name = "ch1";
    TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, strDateStart.Data(), strDateEnd.Data());
    if (tango_data == nullptr)
    {
        cout<<"Macro finished with errors: return Tango data is null"<<endl;
        return -3;
    }

    // print
    db_tango.PrintTangoDataMultiGraph(tango_data, "hall sensor voltage, mV");

    delete tango_data;
    return 0;
}

// additional function to compare magnetic field value (current, A) from ELog database and
// average magnetic field (voltage, mV) from the Tango database
void compare_avg_field(int period = 7, bool isOnlyDifferent = false)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    TangoData db_tango;
    const char* detector_name = "bmn";
    const char* parameter_name = "ch1";

    for (int i = 0; i < run_count; i++)
    {
        // get run time
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        TDatime dateStart = pRun->GetStartDatetime();
        TString strDateStart = dateStart.AsSQLString();

        TDatime* dateEnd = pRun->GetEndDatetime();
        if (dateEnd == nullptr)
        {
            cout<<"The function encountered with errors: no end datetime in the database for the run ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete dateEnd;
            delete pRun;
            continue;
        }
        TString strDateEnd = dateEnd->AsSQLString();
        delete dateEnd;
        delete pRun;

        // get elog info
        TObjArray* arrayRecords = ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number);
        if (arrayRecords == nullptr)
        {
            cout<<"The function encountered with errors: no ELOG record was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }
        int* pField = nullptr;
        for (int j = 0; j < arrayRecords->GetEntriesFast(); j++)
        {
            ElogRecord* pRecord = (ElogRecord*) arrayRecords->At(j);
            pField = pRecord->GetSp41();
            if (pField != nullptr)
                break;
        }

        if (pField == nullptr)
        {
            cout<<"The function encountered with errors: no SP-41 field was set in ELOG ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete arrayRecords;
            continue;
        }

        double elog_voltage = (*pField) * 55.87 / 900;
        delete arrayRecords;

        // calculate average magnetic field (voltage, mV) from the Tango database
        TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, strDateStart.Data(), strDateEnd.Data());
        if (tango_data == nullptr)
        {
            cout<<"The function encountered with errors: return Tango data is null ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete pField;
            continue;
        }

        vector<double> vec_average = db_tango.GetAverageTangoData(tango_data);
        if (vec_average.empty())
        {
            cout<<"The function encountered with errors: Tango data is empty or average value is wrong ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete pField;
            delete tango_data;
            continue;
        }
        double average_field = vec_average[0];
        size_t data_count = tango_data->GetEntriesFast();
        delete tango_data;

        // compare ELog magnet field and average field from Tango database
        bool isDifferent = false;
        if (fabs(average_field - elog_voltage) > 5) isDifferent = true;
        if ((isOnlyDifferent) && (!isDifferent))
        {
            delete pField;
            continue;
        }

        cout<<"Run "<<run_numbers[i].period_number<<"-"<<run_numbers[i].run_number<<endl;
        cout<<"Average Tango magnetic field: "<<average_field<<" mV ("<<data_count<<" counts)."<<endl;
        cout<<"ELOG magnetic field: "<<elog_voltage<<" mV ("<<(*pField)<<" A)"<<endl;
        if (fabs(average_field - elog_voltage) > 5)
            cout<<"ERROR: ELOG and Tango magnetic fields differ by more than 5 mV!"<<endl;
        cout<<endl;

        delete pField;
    }

    delete run_numbers;
    cout<<"Macro finished successfully"<<endl;
}

// additional function to compare magnetic field value (current, A) from Elog database
// and average magnetic field (voltage, mV) from UniDb saved from the Tango, and show in TGraph object
void compare_avg_field_graph(int period = 7)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    double* x = new double[run_count];
    double* y = new double[run_count];
    bool* run_error = new bool[run_count];
    for (int i = 0; i < run_count; i++)
    {
        // get ELOG info
        TObjArray* arrayRecords = ElogRecord::GetRecords(run_numbers[i].period_number, run_numbers[i].run_number);
        if (arrayRecords == nullptr)
        {
            cout<<"The function encountered with errors: no ELOG record was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }

        int* pField = nullptr;
        for (int j = 0; j < arrayRecords->GetEntriesFast(); j++)
        {
            ElogRecord* pRecord = (ElogRecord*) arrayRecords->At(j);
            pField = pRecord->GetSp41();
            if (pField != nullptr)
                break;
        }
        if (pField == nullptr)
        {
            cout<<"The function encountered with errors: no SP-41 field was set in ELOG ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            delete arrayRecords;
            run_error[i] = 1;
            continue;
        }
        double elog_voltage = (*pField) * 55.87 / 900;  // 0,062077778
        delete arrayRecords;

        // get UniDb average field for run
        UniRun* pRun = UniRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == nullptr)
        {
            cout<<"The function encountered with errors: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            continue;
        }
        double* avg_field = pRun->GetFieldVoltage();
        delete pRun;
        if (avg_field == nullptr)
        {
            cout<<"The function encountered with errors: no magnet field in the BM@N database ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
            if (run_error[i] == 1) run_error[i] = 3;
            else run_error[i] = 2;
            continue;
        }
        double average_field = *avg_field;

        cout<<"Magnetic field current from detector group: "<<(*pField)<<" A, "<<elog_voltage<<" mV"<<endl<<endl;
        cout<<"Average magnetic field voltage from Tango: "<<average_field<<" mV"<<endl;


        x[i] = *pField;
        y[i] = average_field;
    }

    TCanvas* c1 = new TCanvas("c1","U/I dependence",100,70,700,500);

    TGraph* gr = new TGraph(run_count,x,y);
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle("Ielog, A");
    gr->GetYaxis()->SetTitle("Utango, mV");
    gr->Draw("AP*");

    TFitResultPtr fit_result = gr->Fit("pol1", "S");
    //TMatrixDSym cov = fit_result->GetCovarianceMatrix();
    //Double_t chi2   = fit_result->Chi2();
    Double_t par0   = fit_result->Value(0);
    Double_t par1   = fit_result->Value(1);
    Double_t err0   = fit_result->Error(0);
    Double_t err1   = fit_result->Error(1);
    cout<<"p1 = "<<par1<<". p1_error = "<<err1<<". p0 = "<<par0<<". p0_error = "<<err0<<endl;

    // calculate residuals for all runs
    double* x2 = new double[run_count];
    double* y2 = new double[run_count];
    for (int i = 0; i < run_count; i++)
    {
        if (run_error[i] == 0)
        {
            x2[i] = i;
            y2[i] = y[i] - par1*x[i]+par0;
        }
    }

    TCanvas* c2 = new TCanvas("c2","The search of wrong field values", 200,250,1600,600);

    TGraph* grN = new TGraph(run_count, x2, y2);
    grN->SetTitle("");
    grN->GetXaxis()->SetTitle("run number");
    grN->GetYaxis()->SetTitle("#DeltaU, mV");
    grN->SetMarkerStyle(2);
    grN->GetXaxis()->Set(run_count, 0, run_count-1);

    for (int i = 0; i < run_count; i++)
    {
        if (run_error[i] != 0)
            grN->GetXaxis()->SetBinLabel(i+1, (TString::Format("Error %d", run_numbers[i].run_number).Data()));
        else
        {
            if ((y2[i] > 7) || (y2[i] < -7))
                grN->GetXaxis()->SetBinLabel(i+1, (TString::Format("%d", run_numbers[i].run_number).Data()));
        }
    }
    //for (int j = 0; j < run_count; j++)
    //    grN->GetXaxis()->SetBinLabel(j+1, (TString::Format("%d", run_numbers[j].run_number).Data()));

    grN->Draw("AP");
}
