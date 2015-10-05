void parse_data_to_db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    gSystem->Load("/usr/lib/x86_64-linux-gnu/libpugixml");

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load main libraries

    MpdDbParser pars;
    //pars.ParseXml2Db("/home/soul/run.xml", "/home/soul/run.xslt");
    pars.ParseCsv2Db("/home/soul/tmp/run2_log.csv", "/home/soul/tmp/csv.xslt", true);

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
