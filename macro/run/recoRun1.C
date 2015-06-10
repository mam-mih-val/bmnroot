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

void recoRun1(Int_t runId = 650, Int_t nEvents = 0) {

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

    TChain *bmnRawTree = new TChain("BMN_RAW");
    bmnRawTree->Add(TString::Format("bmn_run0%d_eb+hrb.root", runId));

    TClonesArray *dchDigits;
    TClonesArray *mwpcDigits;
    TClonesArray *tof400Digits;
    TClonesArray *tof700Digits;
    TClonesArray *zdcDigits;
    bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);
    //    bmnTree->SetBranchAddress("bmn_tof1_digit", &tof400Digits);
    //    bmnTree->SetBranchAddress("bmn_tof2_digit", &tof700Digits);
    bmnTree->SetBranchAddress("bmn_zdc_digit", &zdcDigits);
    bmnRawTree->SetBranchAddress("bmn_mwpc", &mwpcDigits);

    Int_t startEvent = 0;
    Int_t events = (nEvents == 0) ? bmnTree->GetEntries() : nEvents;

    TFile* fReco = new TFile(TString::Format("bmndst_run%d.root", runId), "RECREATE");
    TTree* tReco = new TTree("cbmsim", TString::Format("bmndst_run%d", runId));

    TClonesArray* dchHits = new TClonesArray("BmnDchHit");
    TClonesArray* mwpcHits = new TClonesArray("BmnMwpcHit");
    TClonesArray* dchTracks = new TClonesArray("CbmTrack");
    TClonesArray* mwpcTracks0 = new TClonesArray("CbmTrack");
    TClonesArray* mwpcTracks1 = new TClonesArray("CbmTrack");
    TClonesArray* mwpcTracks2 = new TClonesArray("CbmTrack");
    TClonesArray* mwpcMatchedTracks = new TClonesArray("CbmTrack");

    tReco->Branch("BmnDchHit", &dchHits);
    tReco->Branch("BmnMwpcHit", &mwpcHits);
    tReco->Branch("DchTracks", &dchTracks);
    tReco->Branch("Mwpc0Seeds", &mwpcTracks0);
    tReco->Branch("Mwpc1Seeds", &mwpcTracks1);
    tReco->Branch("Mwpc2Seeds", &mwpcTracks2);
    tReco->Branch("MwpcMatchedTracks", &mwpcMatchedTracks);

    cout << "Time Selection: START" << endl;

    TH1F* h_times0 = new TH1F("h_times0", "mwpc0_times", 40, 0.0, 40.0);
    TH1F* h_times1 = new TH1F("h_times1", "mwpc1_times", 40, 0.0, 40.0);
    TH1F* h_times2 = new TH1F("h_times2", "mwpc2_times", 40, 0.0, 40.0);
    for (Int_t iEv = startEvent; iEv < startEvent + events; iEv++) {
        if (iEv % 1000 == 0) cout << "TIME SEECTION: \tRUN#" << runId << "\tEvent: " << iEv + 1 << "/" << startEvent + events << endl;
        bmnRawTree->GetEntry(iEv);
        for (Int_t iDig = 0; iDig < mwpcDigits->GetEntriesFast(); ++iDig) {
            BmnMwpcDigit* digi = (BmnMwpcDigit*) mwpcDigits->At(iDig);
            Short_t plane = digi->GetPlane();
            if (plane < 6)
                h_times0->Fill(digi->GetTime());
            else if (plane < 12)
                h_times1->Fill(digi->GetTime());
            else if (plane < 18)
                h_times2->Fill(digi->GetTime());
        }
    }

    DigitsTimeSelection(h_times0, mwpc0_leftTime, mwpc0_rightTime);
    DigitsTimeSelection(h_times1, mwpc1_leftTime, mwpc1_rightTime);
    DigitsTimeSelection(h_times2, mwpc2_leftTime, mwpc2_rightTime);

    delete h_times0;
    delete h_times1;
    delete h_times2;

    cout << "Time Selection: FINISH" << endl;

    for (Int_t iEv = startEvent; iEv < startEvent + events; iEv++) {
        bmnTree->GetEntry(iEv);
        bmnRawTree->GetEntry(iEv);

        dchHits->Clear();
        mwpcHits->Clear();
        dchTracks->Clear();
        mwpcTracks0->Clear();
        mwpcTracks1->Clear();
        mwpcTracks2->Clear();
        mwpcMatchedTracks->Clear();

        if (iEv % 1000 == 0) cout << "RECO: \tRUN#" << runId << "\tEvent: " << iEv + 1 << "/" << startEvent + events << endl;

        /* ======= Functions for hits reconstruction ======= */
        ProcessDchDigits(dchDigits, dchHits);
        ProcessMwpcDigits(mwpcDigits, mwpcHits);

        /* ======= Functions for "seeding" ======= */
        DchTrackFinder(dchHits, dchTracks);
        MwpcTrackFinder(mwpcHits, mwpcTracks0, 0);
        MwpcTrackFinder(mwpcHits, mwpcTracks1, 1);
        MwpcTrackFinder(mwpcHits, mwpcTracks2, 2);

        /* ======= Functions for tracks matching ======= */

        //        MwpcTrackMatching(mwpcHits, mwpcMatchedTracks, mwpcTracks0, mwpcTracks1);
        MwpcTrackMatchingByAllHits(mwpcHits, mwpcMatchedTracks);

        tReco->Fill();
    } // event loop

    tReco->Write();
    fReco->Close();

    delete bmnTree;
    delete bmnRawTree;

}
