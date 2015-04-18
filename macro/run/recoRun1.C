#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include <vector>
#include "TVector3.h"
#include "TFile.h"
#include "TGeoManager.h"


using namespace TMath;


//
//  runType - "run1", "run2", "run3"
//

void recoRun1(Int_t runId = 166) {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TString geoName = (runId >= 12 && runId <= 188) ? "geometry_run1" :
            (runId >= 220 && runId <= 395) ? "geometry_run2" :
            (runId >= 403 && runId <= 688) ? "geometry_run3" :
            "WRONG runId = " + runId + "! NO GEOMETRY FOUND!!!";

    cout << "INFO: Geometry type: " << geoName << endl;
    if (geoName.Contains("NO GEOMETRY FOUND!!!")) return;

    TGeoManager::Import(geoName + ".root");

    TChain *bmnTree = new TChain("BMN_DIGIT");
    bmnTree->Add(TString::Format("bmn_run0%d_digit.root", runId));

    TChain *bmnRawTree = new TChain("BMN_RAW"); //needed for mwpc digit //why?!?!?!??!
    bmnRawTree->Add("bmn_run0607_eb+hrb.root");

    TClonesArray *dchDigits;
    TClonesArray *mwpcDigits;
    TClonesArray *tof400Digits;
    TClonesArray *tof700Digits;
    TClonesArray *zdcDigits;
    bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);
    bmnTree->SetBranchAddress("bmn_tof1_digit", &tof400Digits);
    bmnTree->SetBranchAddress("bmn_tof2_digit", &tof700Digits);
    bmnTree->SetBranchAddress("bmn_zdc_digit", &zdcDigits);
    bmnRawTree->SetBranchAddress("bmn_mwpc", &mwpcDigits);

    Int_t startEvent = 789;
    Int_t nEvents = 1;//bmnTree->GetEntries();
    TClonesArray* dchHits = new TClonesArray("BmnDchHit");
    TClonesArray* mwpcHits = new TClonesArray("BmnMwpcHit");
    TClonesArray* hitsOrig = new TClonesArray("BmnDchHitOriginal");
    TClonesArray* recoTracks = new TClonesArray("CbmTrack");

    TFile* fReco = new TFile("bmndst_test.root", "RECREATE");
    TTree* tReco = new TTree("cbmsim", "test_bmn");
    tReco->Branch("BmnDchHit", &dchHits);
    tReco->Branch("BmnMwpcHit", &mwpcHits);
    tReco->Branch("BmnDchHitOriginal", &hitsOrig);
    tReco->Branch("RecoTracks", &recoTracks);

    for (Int_t iEv = startEvent; iEv < startEvent + nEvents; iEv++) {
        bmnTree->GetEntry(iEv);
        bmnRawTree->GetEntry(iEv);

        dchHits->Clear();
        mwpcHits->Clear();
        hitsOrig->Clear();
        recoTracks->Clear();

        if (iEv % 1000 == 0) cout << "Event: " << iEv + 1 << "/" << startEvent + nEvents << endl;

        /* ======= Functions for hits reconstruction ======= */

        //FIXME! Calling of this functions should depend on runType
        ProcessDchDigits(dchDigits, dchHits);
        //        ProcessMwpcDigits(mwpcDigits, mwpcHits);
      
        /* ======= Functions for "seeding" ======= */
        //Make here coordinates transformation to find hits corresponded same track
//        BmnStatus status = FindSeed(dchHits, recoTracks);
        FindSeed(dchHits, recoTracks);
        
        /* ======= Functions for tracks reconstruction ======= */
        //        TVector3 vertex;
        //        TVector3 direction;
        //        LineFit3D(dchHits, vertex, direction);
        //        cout << dchHits->GetEntriesFast() << endl;
        //        vertex.Print();
        //        direction.Print();

        tReco->Fill();
    } // event loop

    tReco->Write();
    fReco->Close();

}
