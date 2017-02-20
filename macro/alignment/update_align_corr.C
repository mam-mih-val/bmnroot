// This macro updates the cumulative mis-alignment parameters; it is only called
// when the "previous" misalignment file already exists someway: either we use
// the defaults, or start using this macro only from the second iteration...

#include   <TClonesArray.h>
#include   <TFile.h>
#include   <TString.h>
#include   <TTree.h>

void update_align_corr(TString preAlignCorrFileName
                      ,TString newAlignCorrFileName
                      ,TString sumAlignCorrFileName
                      )
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    TFile* preFile = new TFile(preAlignCorrFileName.Data());
    TTree* preTree = (TTree*)preFile->Get("cbmsim");
    TClonesArray* preCorrs = NULL;
    preTree->GetBranch("BmnGemAlignmentCorrections")->SetAutoDelete(kFALSE);
    preTree->SetBranchAddress("BmnGemAlignmentCorrections", &preCorrs);

    TFile* newFile = new TFile(newAlignCorrFileName.Data());
    TTree* newTree = (TTree*)newFile->Get("cbmsim");
    TClonesArray* newCorrs = NULL;
    newTree->GetBranch("BmnGemAlignmentCorrections")->SetAutoDelete(kFALSE);
    newTree->SetBranchAddress("BmnGemAlignmentCorrections", &newCorrs);

    TFile* sumFile = new TFile(sumAlignCorrFileName.Data(),"recreate");
    TTree* sumTree = preTree->CloneTree();
    TClonesArray* sumCorrs = NULL;
    sumTree->SetBranchAddress("BmnGemAlignmentCorrections", &sumCorrs);

    for (Int_t iEntry=0; iEntry<(preTree->GetEntries()); iEntry++) {
        preTree->GetEntry(iEntry);
        newTree->GetEntry(iEntry);
        sumTree->GetEntry(iEntry);
        for (Int_t iCorr=0; iCorr<(preCorrs->GetEntriesFast()); iCorr++) {
            BmnGemAlignmentCorrections* preCo = (BmnGemAlignmentCorrections*)preCorrs->UncheckedAt(iCorr);
            BmnGemAlignmentCorrections* newCo = (BmnGemAlignmentCorrections*)newCorrs->UncheckedAt(iCorr);
            BmnGemAlignmentCorrections* sumCo = (BmnGemAlignmentCorrections*)sumCorrs->UncheckedAt(iCorr);
            
            sumCo->SetCorrections(preCo->GetCorrections().X() + newCo->GetCorrections().X(),
                                  preCo->GetCorrections().Y() + newCo->GetCorrections().Y(),   
                                  preCo->GetCorrections().Z() + newCo->GetCorrections().Z());
        }
        sumTree->Fill();
    }
    preTree->Print();
    newTree->Print();
    sumTree->Print();
    sumFile->cd();
    sumTree->Write();
    delete preFile;
    delete newFile;
    delete sumFile;
}
