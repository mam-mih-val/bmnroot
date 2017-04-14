// -----------------------------------------------------------------------------
// This macro updates the cumulative mis-alignment parameters; it is only called
// when the "previous" misalignment file already exists someway: either we use
// the defaults, or start using this macro only from the second iteration...
//
// Anatoly.Solomin@jinr.ru 2017-02-16

void update_align_corrections_bmn(TString preAlignCorrFileName,
                                  TString newAlignCorrFileName,
                                  TString sumAlignCorrFileName)
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    // get run period
    TString runPeriod = newAlignCorrFileName; // just to start with
    TPRegexp("bmn_run([\\d]+)[\\w\\d\\.]+").Substitute(runPeriod, "$1");
    int run_period  = runPeriod.Atoi();
    cout <<"run_period                     = "<<run_period<< endl;

    // Define GEM config. to be used: RunWinter2016 or RunSpring2017
    BmnGemStripStationSet* stationSet;
    TString GemAlignCorrBranchName;
    if      (run_period == 6) {
        stationSet = new BmnGemStripStationSet_RunSpring2017(BmnGemStripConfiguration::RunSpring2017);
        GemAlignCorrBranchName = "BmnGemAlignCorrections";
    }
    else if (run_period == 5) {
        stationSet = new BmnGemStripStationSet_RunWinter2016(BmnGemStripConfiguration::RunWinter2016);
        GemAlignCorrBranchName = "BmnGemAlignCorrections";
    }
    TFile* preFile = new TFile(preAlignCorrFileName.Data());
    TTree* preTree = (TTree*)preFile->Get("cbmsim");
    cout <<"preTree->Print()"<< endl;    preTree->Print();
    TClonesArray* preCorrs = new   TClonesArray(GemAlignCorrBranchName);
    TBranch*    b_preCorrs = preTree->GetBranch(GemAlignCorrBranchName);
    b_preCorrs->SetAddress(&preCorrs);

    TFile* newFile = new TFile(newAlignCorrFileName.Data());
    TTree* newTree = (TTree*)newFile->Get("cbmsim");
    cout <<"newTree->Print()"<< endl;    newTree->Print();
    TClonesArray* newCorrs = new TClonesArray(GemAlignCorrBranchName);
    newTree->SetBranchAddress(GemAlignCorrBranchName,  &newCorrs);

    TFile* sumFile = new TFile(sumAlignCorrFileName.Data(),"RECREATE");
    TTree* sumTree = new TTree("cbmsim", "sum");
    TClonesArray* sumCorrs    = new TClonesArray(GemAlignCorrBranchName);
    TClonesArray& sumCorrsRef = *sumCorrs;
    sumTree->Branch(GemAlignCorrBranchName, &sumCorrs);
    cout <<"sumTree->Print()"<< endl;    sumTree->Print();

    const Int_t nStat = stationSet->GetNStations();
    const Int_t nParams = 3;
    Double_t*** corr = new Double_t**[nStat];
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = stationSet->GetGemStation(iStat)->GetNModules();
        corr[iStat] = new Double_t*[nModul];
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            corr[iStat][iMod] = new Double_t[nParams];
            for (Int_t iPar = 0; iPar < nParams; iPar++) {
                corr[iStat][iMod][iPar] = 0.;
            }
        }
    }
    for (Int_t iEntry=0; iEntry < (Int_t)preTree->GetEntries(); iEntry++) {
      //cout <<"iEntry = "<<iEntry<< endl;
        preTree->GetEntry(iEntry);
      //cout <<"(Int_t)preCorrs->GetEntriesFast() = "<<(Int_t)preCorrs->GetEntriesFast()<< endl;
        if ((Int_t)preCorrs->GetEntriesFast() > 0) break;
    }
    for (Int_t iEntry=0; iEntry < (Int_t)newTree->GetEntries(); iEntry++) {
      //cout <<"iEntry = "<<iEntry<< endl;
        newTree->GetEntry(iEntry);
      //cout <<"(Int_t)newCorrs->GetEntriesFast() = "<<(Int_t)newCorrs->GetEntriesFast()<< endl;
        if ((Int_t)newCorrs->GetEntriesFast() > 0) break;
    }
    for (Int_t iCorr=0; iCorr < (Int_t)newCorrs->GetEntriesFast(); iCorr++) {
        cout <<"iCorr = "<<iCorr<< endl;
        BmnGemAlignCorrections* preCorr = (BmnGemAlignCorrections*)preCorrs->At(iCorr);
        Int_t iStat = preCorr->GetStation();
        Int_t iMod  = preCorr->GetModule();
        BmnGemAlignCorrections* newCorr = (BmnGemAlignCorrections*)newCorrs->At(iCorr);
        BmnGemAlignCorrections* sumCorr = new(sumCorrsRef[iCorr]) BmnGemAlignCorrections();
        sumCorr->SetStation(iStat);
        sumCorr->SetModule(iMod);
        sumCorr->SetCorrections(preCorr->GetCorrections().X() + newCorr->GetCorrections().X(),
                                preCorr->GetCorrections().Y() + newCorr->GetCorrections().Y(),
                                preCorr->GetCorrections().Z() + newCorr->GetCorrections().Z());
        cout <<"preCorr: Stat "<<iStat<<" Module "<<iMod<<" Param. "<<0<<" Value (in cm.) "<<TString::Format("% 14.11f",  preCorr->GetCorrections().X())<< endl;
        cout <<"newCorr: Stat "<<iStat<<" Module "<<iMod<<" Param. "<<0<<" Value (in cm.) "<<TString::Format("% 14.11f",  newCorr->GetCorrections().X())<< endl;
        cout <<"sumCorr: Stat "<<iStat<<" Module "<<iMod<<" Param. "<<0<<" Value (in cm.) "<<TString::Format("% 14.11f",  sumCorr->GetCorrections().X())<< endl;
        cout << endl;
        cout <<"preCorr: Stat "<<iStat<<" Module "<<iMod<<" Param. "<<1<<" Value (in cm.) "<<TString::Format("% 14.11f",  preCorr->GetCorrections().Y())<< endl;
        cout <<"newCorr: Stat "<<iStat<<" Module "<<iMod<<" Param. "<<1<<" Value (in cm.) "<<TString::Format("% 14.11f",  newCorr->GetCorrections().Y())<< endl;
        cout <<"sumCorr: Stat "<<iStat<<" Module "<<iMod<<" Param. "<<1<<" Value (in cm.) "<<TString::Format("% 14.11f",  sumCorr->GetCorrections().Y())<< endl;
        cout << endl;
        cout <<"preCorr: Stat "<<iStat<<" Module "<<iMod<<" Param. "<<2<<" Value (in cm.) "<<TString::Format("% 14.11f",  preCorr->GetCorrections().Z())<< endl;
        cout <<"newCorr: Stat "<<iStat<<" Module "<<iMod<<" Param. "<<2<<" Value (in cm.) "<<TString::Format("% 14.11f",  newCorr->GetCorrections().Z())<< endl;
        cout <<"sumCorr: Stat "<<iStat<<" Module "<<iMod<<" Param. "<<2<<" Value (in cm.) "<<TString::Format("% 14.11f",  sumCorr->GetCorrections().Z())<< endl;
        cout << endl;
        corr[iStat][iMod][0] =  sumCorr->GetCorrections().X();
        corr[iStat][iMod][1] =  sumCorr->GetCorrections().Y();
        corr[iStat][iMod][2] =  sumCorr->GetCorrections().Z();
    }
    sumTree->Fill();
    sumFile->cd();
    sumTree->Write("", TObject::kOverwrite);
    delete preFile;
    delete newFile;
    delete sumFile;
    delete stationSet;
    delete corr;
}
