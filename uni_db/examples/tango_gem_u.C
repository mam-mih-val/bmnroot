// macro for printing TANGO data for GEM high voltage
void tango_gem_u()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniDbTangoData db_tango;

    const char* detector_name = "gem";
    const char* parameter_name = "u";
    const char* date_start = "2016-12-21 12:20:00";
    const char* date_end = "2016-12-21 12:40:00";

    // TObjArray stores array of TangoTimeParameter* (parameter's values) for all time points (uni_db/UniDbTangoData.h)
    // tango_data->At(0) -> TangoTimeParameter*: parameter's values for fist time point of the given period (date_start - date_end)
    TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, date_start, date_end);
    if (tango_data == NULL)
    {
        cout<<"Macro finished with errors"<<endl;
        exit(-1);
    }

    // console output
    db_tango.PrintTangoDataConsole(tango_data);

    // surface graph output
    db_tango.PrintTangoDataSurface(tango_data);

    delete tango_data;

    cout<<"Macro finished successfully"<<endl;
}
