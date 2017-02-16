#include "../db_structures.h"

// macro for printing TANGO data for GEM high voltage
void tango_gem_trip()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libUniDb");

    UniDbTangoData db_tango;

    const char* detector_name = "gem";
    const char* parameter_name = "trip";
    const char* date_start = "2016-12-21 15:13:22"; // 930 run
    const char* date_end = "2016-12-21 15:28:23";
    enumConditions condition = conditionEqual;
    bool condition_value = 1;
    // map: channel -> Z-order
    int map_channel[] = {1, 3, 0, 5, 2, 6, 4, 7};

    // TObjArray stores array of TObjArray* (for all channels) with TangoTimeInterval*: time intervals satisfying the condition (uni_db/UniDbTangoData.h)
    // ((TObjArray*)tango_data->At(0))->At(2) -> TangoTimeInterval*: third time interval satisfying the condition for 1 channel
    TObjArray* tango_data = db_tango.SearchTangoIntervals(detector_name, parameter_name, date_start, date_end, condition, condition_value, map_channel);
    if (tango_data == NULL)
    {
        cout<<"Macro finished with errors"<<endl;
        exit(-1);
    }

    // console output
    cout<<"Information about "<<parameter_name<<" for "<<detector_name<<" (from "<<date_start<<" to "<<date_end<<"):"<<endl;
    db_tango.PrintTangoIntervalConsole(tango_data, "GEM");

    delete tango_data;

    cout<<"Macro finished successfully"<<endl;
}
