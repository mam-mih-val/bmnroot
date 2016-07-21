// macro for printing TANGO data for GEM high voltage
void tango_gem_u()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniDbTangoData db_tango;
    Tango_Double_Data* TD;

    const char* detector_name = "gem";
    const char* parameter_name = "u";
    const char* date_start = "2016-06-29 12:20:00";
    const char* date_end = "2016-06-29 12:40:00";

    TD = db_tango.GetTangoParameter(detector_name, parameter_name, date_start, date_end);
    if (TD == NULL)
    {
        cout<<"Macro finished with errors"<<endl;
        exit(-1);
    }

    // console output
    db_tango.PrintTangoData(TD, false);

    // surface graph output
    db_tango.PrintTangoData(TD, true);

    cout<<"Macro finished successfully"<<endl;
}
