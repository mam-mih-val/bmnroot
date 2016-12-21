 #include   <TString.h>
 #include   <TStopwatch.h>

 void gemAlignment(UInt_t nEvents = 1e6, TString recoFileName = "bmndst_573_all.root", TString addInfo = "", UInt_t startAfterIter = 0)
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // Output file
    TString outputFile = recoFileName;
    TRegexp re("bmndst");
    if (addInfo == "") {
        outputFile(re) = "align"; // replace bmndst with align
    }
    else {
        outputFile(re) = addInfo+"_align";
    }
    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
}
