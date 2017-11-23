// Macro for generating C++ classes - wrappers for dataBase tables
void generate_cxx_from_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load main libraries
    gSystem->Load("libUniDb");

    UniDbGenerateClasses gen;
    int res = gen.GenerateClasses(UNIFIED_DB, "UniDb", true); //(UniConnectionType connection_type, TString class_prefix, bool isOnlyUpdate)
    //int res = gen.GenerateClasses(ELOG_DB, "UniDb", true);

    if (res == 0)
        cout<<"\nGenerating C++ classes has completed successfully"<<endl;
    else
        cout<<"\nGenerating C++ classes has completed with code: "<<res<<endl;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
