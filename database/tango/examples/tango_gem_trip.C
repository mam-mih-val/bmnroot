// macro for printing TANGO data for GEM stations tripped in runs

// map: № Tango channel -> GEMs Z-order (starts from 0)
vector<int> map_channel_run5 {1, 3, 0, 5, 2, 6, 4, 7};
vector<int> map_channel_run6_b1529 {1, 3, 0, 5, 2, 6, 4};
vector<int> map_channel_run6_a1529 {1, 3, 0, 5, 6, 4, 2};

void tango_gem_trip(int period, int run, bool isShowOnlyTrip = false)
{
    TangoData db_tango;

    // get run time
    UniRun* pRun = UniRun::GetRun(period, run);
    if (pRun == nullptr)
    {
        cout<<"Macro finished with errors: no experimental run was found for period = "<<period<<", run = "<<run<<endl;
        exit(-2);
    }

    TString strDateStart = pRun->GetStartDatetime().AsSQLString();
    TDatime* dateEnd = pRun->GetEndDatetime();
    if (dateEnd == nullptr)
    {
        cout<<"Macro finished with errors: no end datetime in the database for period = "<<period<<", run = "<<run<<endl;
        delete pRun;
        exit(-3);
    }
    TString strDateEnd = dateEnd->AsSQLString();
    delete pRun;

    const char* detector_name = "gem";
    const char* parameter_name = "trip";
    vector<int>* map_channel = nullptr;
    switch (period)
    {
        case 5:
            map_channel = &map_channel_run5;
            break;
        case 6:
        {
            if (run < 1529) map_channel = &map_channel_run6_b1529;
            else map_channel = &map_channel_run6_a1529;
            break;
        }
        default:
            cout<<"Macro finished with errors: no channel map was found for the period "<<period<<endl;
            exit(-4);
    }

    enumConditions condition = conditionEqual;
    bool condition_value = 1;

    // TObjArray stores array of TObjArray* (for all channels) with TangoTimeInterval*: time intervals satisfying the condition (uni_db/TangoData.h)
    // ((TObjArray*)tango_data->At(0))->At(2) -> TangoTimeInterval*: third time interval satisfying the condition for the first channel 0
    TObjArray* tango_data = db_tango.SearchTangoIntervals(detector_name, parameter_name, strDateStart.Data(), strDateEnd.Data(),
                                                          condition, condition_value, map_channel);
    if (tango_data == nullptr)
    {
        cout<<"Macro finished with errors"<<endl;
        exit(-1);
    }

    // console output
    cout<<"Information on "<<parameter_name<<" parameter for "<<detector_name<<" (period = "<<period<<", run = "<<run<<"):"<<endl;
    db_tango.PrintTangoIntervalConsole(tango_data, "GEM Z-order", isShowOnlyTrip);
    cout<<endl;

    delete tango_data;
}

void tango_gem_trip(int period, bool isShowOnlyTrip = false)
{
    UniqueRunNumber* run_numbers;
    int run_count = UniRunPeriod::GetRunNumbers(period, run_numbers);
    if (run_count <= 0) return;

    for (int i = 0; i < run_count; i++)
        tango_gem_trip(run_numbers[i].period_number, run_numbers[i].run_number, isShowOnlyTrip);

    cout<<"Macro finished successfully"<<endl;
}
