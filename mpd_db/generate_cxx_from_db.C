// Macro for generating C++ classes - wrappers for dataBase tables
void generate_cxx_from_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    gROOT->LoadMacro("$VMCWORKDIR/macro/mpd/mpdloadlibs.C");
    mpdloadlibs(1, 1); // load main libraries

    MpdDbGenerateClasses gen;
    int res = gen.GenerateClasses("", "MpdDb", true);

    if (res == 0)
        cout<<"\nGenerating C++ classes has completed successfully"<<endl;
    else
        cout<<"\nGenerating C++ classes has completed with code: "<<res<<endl;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    //cout << "Macro finished succesfully." << endl;
}
