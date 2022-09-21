
const Int_t CutMinNTracksPV = 2;

//void FilterPV_CBM(TString BaseName, TString TempBaseName, Double_t minZ = -5.45, Double_t maxZ = 6.55) {
void FilterPV(TString BaseName, TString TempBaseName, Double_t minZ = -7.15, Double_t maxZ = 4.85) {
    printf("\nCloning selected exp events:");
    TFile *BaseHits = new TFile(BaseName, "READ");
    if (BaseHits->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", BaseName.Data());
        return;
    }
    TTree *TreeBase = (TTree *) BaseHits->Get("bmndata");
    //    DigiRunHeader* RHBase = (DigiRunHeader*) BaseHits->Get(RHDigiName.Data());
    TFile *DestHitsFile = new TFile(TempBaseName, "RECREATE");
    if (DestHitsFile->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", TempBaseName.Data());
        return;
    }
    printf("\nOUT FILE: ");
    printf("%s\n", TempBaseName.Data());
    TTree * DestTree = TreeBase->CloneTree(0); //-1, "fast");
    TClonesArray * vertex = nullptr;
    TreeBase->SetBranchAddress("BmnVertex", &vertex);
    UInt_t iBaseEvent = 0;
    UInt_t NBaseEvents = TreeBase->GetEntries();
    for (UInt_t iEv = 0; iEv < NBaseEvents; iEv++) {
        //        DrawBar(iEv, NBaseEvents);
        TreeBase->GetEntry(iEv);
        BmnVertex *vtx = static_cast<BmnVertex*> (vertex->UncheckedAt(0));
        if ((vtx->GetZ() < minZ) || (vtx->GetZ() > maxZ) || (vtx->GetNTracks() < CutMinNTracksPV))
            continue;
        DestTree->Fill();
    }
    DestTree->Write();
    //    DestHitsFile->WriteObject(RHBase, RHDigiName.Data());
    DestHitsFile->Write();
    DestHitsFile->Close();
    DestHitsFile = nullptr;
    BaseHits->Close();
    printf("\nCloning finished!\n");
    return;
}

void DSTConv(
        TString inFile = "~/filesbmn/4649-cbm-full.root",
        TString outFile = "$VMCWORKDIR/macro/run/dst-bmn-4649.root",
        Int_t nStartEvent = 0,
        Int_t nEvents = 0,
        TString inFileToClone = "/eos/nica/bmn/exp/dst/run7/prerelease/3590-4707_BMN_Argon/bmn_run4649_dst.root",
        vector<TString> branchesToClone = {
    "BmnTof400Hit", "BmnTof700Hit",
    "BmnDchTrack", "BmnDchHit",
    "BmnUpstreamHit", "BmnMwpcSegment", "BmnMwpcTrack"
}) {
    TStopwatch timer;
    timer.Start();
    gDebug = 0;
    Int_t fVerbose = 2;
    Int_t periodId = 7;
    Int_t runId = -1;
    // if nEvents is equal 0 then all events of the given file starting with "nStartEvent" should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*) inFile.Data()) - nStartEvent;

    //    if (!UniRun::GetRun(fPeriodId, runId)) {
    //        printf("Run not found in ELOG. Exit.\n");
    //        return;
    //    }

    FairRunAna* fRunAna = new FairRunAna();
    //    BmnFairRunSim* fRunAna = new BmnFairRunSim();
    FairSource* src = new BmnFileSource(inFile);
    fRunAna->SetSource(src);
    //    TString inFileTemp = inFile + ".tmp.root";


    fRunAna->SetInputFile(inFile);
    //    if (inFileToF400.Length() > 0)
    //        fRunAna->AddFriend(inFileToF400);
    fRunAna->SetOutputFile(outFile.Data());
    fRunAna->SetGenerateRunInfo(kFALSE);

    BmnTrackConv * conv = new BmnTrackConv(periodId, runId, kBMNSETUP, inFileToClone, branchesToClone);
    fRunAna->AddTask(conv);
    //    conv->SetRunSimInst(fRunAna);
    //    BmnEventGetter * eg = new BmnEventGetter(inFileToF700, 0, {"BmnTof700Hit", "BmnDchTrack"});
    //    fRunAna->AddTask(eg);    

    fRunAna->Init();
    fRunAna->Run(nStartEvent, nStartEvent + nEvents);
    delete fRunAna;

    TString outFileTemp = outFile + ".pv.root";
//    FilterPV(outFile, outFileTemp);
    //    std::remove(outFile);

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout << "Macro finished successfully." << endl; // marker of successfully execution for software testing systems
}
