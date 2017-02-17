#include "../db_structures.h"

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
    TString strDateEnd = pRun->GetEndDatetime()->AsSQLString();
    delete pRun;

    const char* detector_name = "bmn";
    const char* parameter_name = "ch1";

    TObjArray* tango_data = db_tango.GetTangoParameter(detector_name, parameter_name, strDateStart.Data(), strDateEnd.Data());
    if (tango_data == NULL)
    {
        cout<<"Macro finished with errors: return data is null"<<endl;
        exit(-1);
    }

    vector<double> vec_average = db_tango.GetAverageTangoData(tango_data);
    if (vec_average.empty())
    {
        cout<<"Macro finished with errors: Tango data is empty or average value is wrong"<<endl;
        exit(-2);
    }
    double average_field = vec_average[0];
    delete tango_data;

    cout<<"Average magnetic field for run "<<period<<"-"<<run<<": "<<average_field<<" mv"<<endl;
    cout<<"Macro finished successfully"<<endl;
}
