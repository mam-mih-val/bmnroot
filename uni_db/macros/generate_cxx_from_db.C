// Macro for generating C++ classes - wrappers for dataBase tables
void generate_cxx_from_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load main libraries

    UniDbGenerateClasses gen;
    int res = gen.GenerateClasses("", "UniDb", true); //(TString connection_string, TString class_prefix, bool isOnlyUpdate)

    if (res == 0)
        cout<<"\nGenerating C++ classes has completed successfully"<<endl;
    else
        cout<<"\nGenerating C++ classes has completed with code: "<<res<<endl;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
