void pull_event()
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

    BmnEventPull pull;
    pull.SetPartitionName("bmn");
    pull.SetSamplingType("raw");
    pull.SetSamplingName("file");
    pull.Exec();
}
