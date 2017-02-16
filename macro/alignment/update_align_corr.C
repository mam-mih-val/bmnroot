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
    TFile* preF = new TFile(preAlignCorrFileName.Data());
    TTree* preT = (TTree*)preF->Get("cbmsim");
    TClonesArray* preCorrs = NULL;
    preT->GetBranch("BmnGemAlignmentCorrections")->SetAutoDelete(kFALSE);
    preT->SetBranchAddress("BmnGemAlignmentCorrections", &preCorrs);

    TFile* newF = new TFile(newAlignCorrFileName.Data());
    TTree* newT = (TTree*)newF->Get("cbmsim");
    TClonesArray* newCorrs = NULL;
    newT->GetBranch("BmnGemAlignmentCorrections")->SetAutoDelete(kFALSE);
    newT->SetBranchAddress("BmnGemAlignmentCorrections", &newCorrs);

    TFile* sumF = new TFile(sumAlignCorrFileName.Data(),"recreate");
    TTree* sumT = preT->CloneTree();
    TClonesArray* sumCorrs = NULL;
    sumT->SetBranchAddress("BmnGemAlignmentCorrections", &sumCorrs);

    for (Int_t iEntry=0; iEntry<(preT->GetEntries()); iEntry++) {
      //preCorrs->Clear();
      //newCorrs->Clear();
      //sumCorrs->Clear();
        preT->GetEntry(iEntry);
        newT->GetEntry(iEntry);
        sumT->GetEntry(iEntry);
        for (Int_t iCorr=0; iCorr<(preCorrs->GetEntriesFast()); iCorr++) {
            BmnGemAlignmentCorrections* preCo = (BmnGemAlignmentCorrections*)preCorrs->UncheckedAt(iCorr);
            BmnGemAlignmentCorrections* newCo = (BmnGemAlignmentCorrections*)newCorrs->UncheckedAt(iCorr);
            BmnGemAlignmentCorrections* sumCo = (BmnGemAlignmentCorrections*)sumCorrs->UncheckedAt(iCorr);
            
            sumCo->SetCorrections(preCo->GetCorrections().X() + newCo->GetCorrections().X(),
                                  preCo->GetCorrections().Y() + newCo->GetCorrections().Y(),   
                                  preCo->GetCorrections().Z() + newCo->GetCorrections().Z());
        }
    }
    preT->Print();
    newT->Print();
    sumT->Print();
    sumF->Write();
    delete preF;
    delete newF;
    delete sumF;
}
