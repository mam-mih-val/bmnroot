#include <TChain.h>
#include <TString.h>
#include <TClonesArray.h>

// @(#)bmnroot/macro/howTo:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// readDst.C                                                                  //
//                                                                            //
// An example how to read data (RECO) from bmndst.root                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <Rtypes.h>
#include <FairHit.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void readDst(TString fileName = "") {
    if (fileName == "") {
        cout << "File not specified!" << endl;
        return;
    }

    bmnloadlibs(); // load libraries

    TChain* out = new TChain("cbmsim");
    out->Add(fileName.Data());
    cout << "#recorded entries = " << out->GetEntries() << endl;

    TClonesArray* globTracks = nullptr;
    out->SetBranchAddress("BmnGlobalTrack", &globTracks);

    TClonesArray* gemTrack = nullptr;
    out->SetBranchAddress("BmnGemTrack", &gemTrack);
    
    TClonesArray* gemPoints = nullptr;
    out->SetBranchAddress("BmnGemStripHit", &gemPoints);
    
    TClonesArray* silTrack = nullptr;
    out->SetBranchAddress("BmnSiliconTrack", &silTrack);
    
    TClonesArray* silPoints = nullptr;
    out->SetBranchAddress("BmnSiliconHit", &silPoints);

    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);

        for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); iTrack++) {
            BmnGlobalTrack* track = (BmnGlobalTrack*) globTracks->UncheckedAt(iTrack);
            FairTrackParam* parFirst = track->GetParamFirst();
            FairTrackParam* parLast = track->GetParamLast();

            // Put here your code ...
            
            // Inner tracker contains GEM and SILICon detectors
            BmnTrack* gemTr = nullptr;
            BmnTrack* silTr = nullptr;
        
        if (track->GetGemTrackIndex() != -1)
            gemTr = (BmnTrack*) gemTrack->UncheckedAt(track->GetGemTrackIndex());
        
        if (track->GetSilTrackIndex() != -1)
            silTr = (BmnTrack*) silTrack->UncheckedAt(track->GetSilTrackIndex());

        // GEM track
        if (track->GetGemTrackIndex() != -1)
            for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++) {
                BmnGemStripHit* hit = (BmnGemStripHit*) gemPoints->UncheckedAt(gemTr->GetHitIndex(iHit));

                // Put here your code ...
                
            }

        // SILICON track
        if (track->GetSilTrackIndex() != -1)
            for (Int_t iHit = 0; iHit < silTr->GetNHits(); iHit++) {
                BmnSiliconHit* hit = (BmnSiliconHit*) silPoints->UncheckedAt(silTr->GetHitIndex(iHit));
                               
                // Put here your code ...
                
            }
        }
    }
}
