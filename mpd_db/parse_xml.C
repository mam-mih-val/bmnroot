void parse_xml()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    gSystem->Load("/usr/lib/x86_64-linux-gnu/libpugixml");

    gROOT->LoadMacro("$VMCWORKDIR/macro/mpd/mpdloadlibs.C");
    mpdloadlibs(1, 1); // load main libraries

    MpdDbParser pars;
    //pars.ParseXml2Db("/home/soul/run.xml", "/home/soul/run.xslt");
    pars.ParseTxtNoise2Db("/home/soul/Downloads/noise_run3.txt","/home/soul/txt.xslt");

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
