//#include "../run/bmnloadlibs.C"
//
//void digiAnalysis() {
//    bmnloadlibs();
//    TChain *eveTree = new TChain("cbmsim");
//    //1
//    eveTree->Add("bmn_run3742_digi.root");
//
//    TClonesArray *GemDigits;
//    eveTree->SetBranchAddress("GEM", &GemDigits);
//
//
//    Int_t records = eveTree->GetEntries();
//    if (records > 0) {
//        eveTree->GetEntry(0);
//    }
//    Long64_t nEvents = 10000; //eveTree->GetEntries();
//    cout << nEvents << endl;
//
//
//    //    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
//    //        if (iEv % 100 == 0) cout << "EVENT: " << iEv << endl;
//    //        
//    //        eveTree->GetEntry(iEv);
//    ////        for (Int_t iDig = 0; iDig < GemDigits->GetEntriesFast(); ++iDig) {
//    ////            BmnGemStripDigit* digX = (BmnGemStripDigit*) GemDigits->At(iDig);
//    ////            Int_t lay = digX->GetStripLayer();
//    ////            Int_t st = digX->GetStation();
//    ////            Int_t str = digX->GetStripNumber();
//    ////            Int_t mod = digX->GetModule();
//    ////            
//    ////        }
//    //    }
//
//}

#include "../run/bmnloadlibs.C"

void digiAnalysis() {
    bmnloadlibs();
    TChain *eveTree = new TChain("cbmsim");
    eveTree->Add("bmn_run3742_digi.root");

    TClonesArray *GemDigits;
    eveTree->SetBranchAddress("GEM", &GemDigits);

    Int_t records = eveTree->GetEntries();
    if (records > 0) {
        eveTree->GetEntry(0);
    }
}