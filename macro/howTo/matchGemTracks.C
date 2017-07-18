#include <TChain.h>
#include <TString.h>
#include <TClonesArray.h>

// @(#)bmnroot/macro/howTo:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// matchGemTracks.C                                                           //
// Macro allows one to make a correspondance (GEM) between reco and MC tracks //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void matchGemTracks(TString eveName = "", TString dstName = "") {
    if (eveName == "" || dstName == "") {
        cout << "Specify MC and reco data correctly!" << endl;
        return;
    }

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/geometry.C");

    TChain* recoChain = new TChain("cbmsim");
    recoChain->Add(dstName);

    TChain* simChain = new TChain("cbmsim");
    simChain->Add(eveName);

    TClonesArray* recoTracks = NULL;
    TClonesArray* simTracks = NULL;
    TClonesArray* matchLink = NULL;

    recoChain->SetBranchAddress("BmnGemTrackMatch", &matchLink);
    recoChain->SetBranchAddress("BmnGemTrack", &recoTracks);
    simChain->SetBranchAddress("MCTrack", &simTracks);

    for (Int_t iEv = 0; iEv < recoChain->GetEntries(); iEv++) {
        recoChain->GetEntry(iEv);
        simChain->GetEntry(iEv);

        for (Int_t iTrack = 0; iTrack < recoTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*) recoTracks->UncheckedAt(iTrack);
            BmnTrackMatch* gemTrackMatch = (BmnTrackMatch*) matchLink->UncheckedAt(iTrack);

            if (!track || !gemTrackMatch || gemTrackMatch->GetNofLinks() == 0) continue;

            Int_t gemMCId = gemTrackMatch->GetMatchedLink().GetIndex();

            CbmMCTrack* mcTrack = (CbmMCTrack*) simTracks->UncheckedAt(gemMCId);
            cout << "P_reco = " << 1. / TMath::Abs(track->GetParamFirst()->GetQp()) << " P_sim = " << mcTrack->GetP() << endl;
        }
    }
}