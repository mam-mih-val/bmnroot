const Int_t CutMinNTracksPV = 2;

void FilterPV(TString BaseName, TString TempBaseName, Double_t minZ = -5.45, Double_t maxZ = 6.55) {
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

