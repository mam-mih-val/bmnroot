// This macro updates the cumulative mis-alignment parameters; it is only called
// when the "previous" misalignment file already exists someway: either we use
// the defaults, or start using this macro only from the second iteration...

#include   <TClonesArray.h>
#include   <TFile.h>
#include   <TString.h>
#include   <TTree.h>

void update_align_corr(TString preAlignCorrFileName,
                       TString newAlignCorrFileName,
                       TString sumAlignCorrFileName)
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    TFile* preFile = new TFile(preAlignCorrFileName.Data());
    TTree* preTree = (TTree*)preFile->Get("cbmsim");
    TClonesArray* preCorrs = NULL;
  //preTree->GetBranch("BmnGemAlignmentCorrections")->SetAutoDelete(kFALSE);
    preTree->SetBranchAddress("BmnGemAlignmentCorrections", &preCorrs);

    TFile* newFile = new TFile(newAlignCorrFileName.Data());
    TTree* newTree = (TTree*)newFile->Get("cbmsim");
    TClonesArray* newCorrs = NULL;
    newTree->GetBranch("BmnGemAlignmentCorrections")->SetAutoDelete(kFALSE);
    newTree->SetBranchAddress("BmnGemAlignmentCorrections", &newCorrs);

    TFile* sumFile = new TFile(sumAlignCorrFileName.Data(),"RECREATE");
  //TTree* sumTree = newTree->CloneTree();
    TTree* sumTree = new TTree("cbmsim");
    TClonesArray* sumCorrs = NULL;
  //sumTree->SetBranchAddress("BmnGemAlignmentCorrections", &sumCorrs);
    sumTree->Branch("BmnGemAlignmentCorrections", &sumCorrs);

    BmnGemStripStationSet* StationSet = new BmnGemStripStationSet_RunWinter2016(BmnGemStripConfiguration::RunWinter2016);
    const Int_t nStat = StationSet->GetNStations();
    const Int_t nParams = 3;
    Double_t*** corr = new Double_t**[nStat];
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
        corr[iStat] = new Double_t*[nModul];
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            corr[iStat][iMod] = new Double_t[nParams];
            for (Int_t iPar = 0; iPar < nParams; iPar++) {
                corr[iStat][iMod][iPar] = 0.;
            }
        }
    }
    cout <<"New additional alignment corrections obtained:"<< endl;
    for (Int_t iEntry=0; iEntry<(preTree->GetEntries()); iEntry++) {
        cout <<"iEntry = "<<iEntry<< endl;
        preTree->GetEntry(iEntry);
        newTree->GetEntry(iEntry);
        sumTree->GetEntry(iEntry);
        for (Int_t iCorr=0; iCorr<(preCorrs->GetEntriesFast()); iCorr++) {
            cout <<"iCorr = "<<iCorr<< endl;
            BmnGemAlignmentCorrections* preCo = (BmnGemAlignmentCorrections*)preCorrs->UncheckedAt(iCorr);
            BmnGemAlignmentCorrections* newCo = (BmnGemAlignmentCorrections*)newCorrs->UncheckedAt(iCorr);
            BmnGemAlignmentCorrections* sumCo = (BmnGemAlignmentCorrections*)sumCorrs->UncheckedAt(iCorr);
            
            sumCo->SetCorrections(preCo->GetCorrections().X() + newCo->GetCorrections().X(),
                                  preCo->GetCorrections().Y() + newCo->GetCorrections().Y(),   
                                  preCo->GetCorrections().Z() + newCo->GetCorrections().Z());

            Int_t iStat = newCo->GetStation();
            Int_t iMod  = newCo->GetModule();
            cout <<"Stat "<<iStat<<" Module "<<iMod<<" Param. "<<0<<" Value (in cm.) "<<TString::Format("% 14.5E", -newCo->GetCorrections().X())<< endl;
            cout <<"Stat "<<iStat<<" Module "<<iMod<<" Param. "<<1<<" Value (in cm.) "<<TString::Format("% 14.5E", -newCo->GetCorrections().Y())<< endl;
            cout <<"Stat "<<iStat<<" Module "<<iMod<<" Param. "<<2<<" Value (in cm.) "<<TString::Format("% 14.5E", -newCo->GetCorrections().Z())<< endl;
            corr[iStat][iMod][0] = -newCo->GetCorrections().X();
            corr[iStat][iMod][1] = -newCo->GetCorrections().Y();
            corr[iStat][iMod][2] = -newCo->GetCorrections().Z();
        }
        sumTree->Fill();
    }
    cout <<"Updated total alignment corrections to be used further:"<< endl;
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = StationSet->GetGemStation(iStat)->GetNModules();
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            cout <<"Stat "<<iStat<<" Module "<<iMod<<" Param. "<<0<<" Value (in cm.) "<<TString::Format("% 14.5E", corr[iStat][iMod][0])<< endl;
            cout <<"Stat "<<iStat<<" Module "<<iMod<<" Param. "<<1<<" Value (in cm.) "<<TString::Format("% 14.5E", corr[iStat][iMod][1])<< endl;
            cout <<"Stat "<<iStat<<" Module "<<iMod<<" Param. "<<2<<" Value (in cm.) "<<TString::Format("% 14.5E", corr[iStat][iMod][2])<< endl;
        }
    }
    preTree->Print();
    newTree->Print();
    sumTree->Print();
    sumFile->cd();
    sumTree->Write("", TObject::kOverwrite);
    delete preFile;
    delete newFile;
    delete sumFile;
    delete StationSet;
    delete corr;
}
