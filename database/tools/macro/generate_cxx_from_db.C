// Macro for generating C++ classes - wrappers for all database tables
void generate_cxx_from_db(TString json_configuration_file = "$VMCWORKDIR/database/tools/scheme/uni_db.json")
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    int res = UniGenerateClasses::GenerateClasses(json_configuration_file);
    if (res == 0)
        cout<<"\nGenerating C++ interface classes has completed successfully"<<endl;
    else
        cout<<"\nGenerating C++ interfaqce classes has failed with code: "<<res<<endl;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
