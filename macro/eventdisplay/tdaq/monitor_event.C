void monitor_event()
{
    gDebug = 0;

    // load main and detectors libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();

    // load Event Display libraries
    gSystem->Load("libEve");
    gSystem->Load("libEventDisplay");

    // load TDAQ
    gSystem->Load("libemon");
    gSystem->Load("libemon-dal");
    gSystem->Load("libcmdline");
    gSystem->Load("libipc");
    gSystem->Load("libowl");
    gSystem->Load("libomniORB4");
    gSystem->Load("libomnithread");
    gSystem->Load("libers");

    BmnEventMonitor mon;
    mon.SetVerbose(2);
    mon.SetPartitionName("bmn");
    mon.SetSamplingType("raw");
    mon.SetSamplingName("file");
    mon.SetMaxEvents(10000);
    mon.Exec();
}
