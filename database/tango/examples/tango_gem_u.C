// Macro for printing TANGO data for GEM high voltage

// for datetime range
void tango_gem_u(const char* date_start = "2016-12-21 12:20:00", const char* date_end = "2016-12-21 12:40:00")
{
    TangoData db_tango;

    const char* detector_name = "gem";
    const char* parameter_name = "u";

    // TObjArray stores array of TangoTimeParameter* (parameter's values) for all time points (uni_db/TangoData.h)
    // tango_data->At(0) -> TangoTimeParameter*: parameter's values for fist time point of the given period (date_start - date_end)
    TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, date_start, date_end);
    if (tango_data == nullptr)
    {
        cout<<"Macro finished with errors"<<endl;
        exit(-1);
    }

    // console output
    db_tango.PrintTangoDataConsole(tango_data);

    // graph output
    //db_tango.PrintTangoDataSurface(tango_data);
    db_tango.PrintTangoDataMultiGraph(tango_data);  //, "U", true);

    delete tango_data;

    cout<<"Macro finished successfully"<<endl;
}

// for a given run
void tango_gem_u(int period, int run)
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
    const char* parameter_name = "u";

    // TObjArray stores array of TangoTimeParameter* (parameter's values) for all time points (uni_db/TangoData.h)
    // tango_data->At(0) -> TangoTimeParameter*: parameter's values for fist time point of the given period (date_start - date_end)
    TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, strDateStart.Data(), strDateEnd.Data());
    if (tango_data == nullptr)
    {
        cout<<"Macro finished with errors"<<endl;
        exit(-1);
    }

    // console output
    db_tango.PrintTangoDataConsole(tango_data);

    // graph output
    //db_tango.PrintTangoDataSurface(tango_data);
    db_tango.PrintTangoDataMultiGraph(tango_data);

    delete tango_data;

    cout<<"Macro finished successfully"<<endl;
}
