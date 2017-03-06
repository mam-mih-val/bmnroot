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

// additional function to compare magnetic field value (current, A) of the run info and average magnetic field (voltage, mv) from the Tango and show in TGraph object
void compare_avg_field_graph(int period = 5)
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniqueRunNumber* run_numbers;
    int run_count = UniDbRun::GetRunNumbers(period, 1, period, 100000, run_numbers);
    if (run_count <= 0)
        return;

    double* x = new double[run_count];
    double* y = new double[run_count];
    bool* run_error = new bool[run_count];
    for (int i = 0; i < run_count; i++)
    {
        // get run
        UniDbRun* pRun = UniDbRun::GetRun(run_numbers[i].period_number, run_numbers[i].run_number);
        if (pRun == NULL)
        {
            cout<<"The function encountered with unexpected error: no experimental run was found ("<<run_numbers[i].period_number<<":"<<run_numbers[i].run_number<<"). This run will be skipped!"<<endl;
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
        run_error[i] = 0;
        if (dFieldCurrent)
            cout<<"Magnetic field current from detector group: "<<*dFieldCurrent<<" A"<<endl;
        else
        {
            cout<<"WARNING: magnetic field current from detector group was not set in the database"<<endl;
            run_error[i] = 1;
        }
        if (dAvgFieldVoltage)
            cout<<"Average magnetic field voltage from Tango: "<<*dAvgFieldVoltage<<" mv"<<endl<<endl;
        else
        {
            cout<<"WARNING: average magnetic field voltage was not calculated from Tango"<<endl<<endl;
            if (run_error[i] == 1) run_error[i] = 3;
            else run_error[i] = 2;
        }

        if ((dFieldCurrent) && (dAvgFieldVoltage))
        {
            x[i] = *dFieldCurrent;
            y[i] = *dAvgFieldVoltage;
        }
    }

    TCanvas* c1 = new TCanvas("c1","U/I dependence",100,70,700,500);

    TGraph* gr = new TGraph(run_count,x,y);
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle("I, A");
    gr->GetYaxis()->SetTitle("U, mv");
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
    grN->GetYaxis()->SetTitle("#DeltaU, mv");
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
