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
// It demonstrates how to select GEM hits which belong to the GEM track       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void readDst(TString fileName = "") {
    if (fileName == "") {
        cout << "File not specified!" << endl;
        return;
    }

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/geometry.C");

    TChain* out = new TChain("cbmsim");
    out->Add(fileName.Data());
    cout << "#recorded entries = " << out->GetEntries() << endl;

    TClonesArray* gemTracks = NULL;
    out->SetBranchAddress("BmnGemTrack", &gemTracks);

    TClonesArray* gemPoints = NULL;
    out->SetBranchAddress("BmnGemStripHit", &gemPoints);

    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);

        for (Int_t iTrack = 0; iTrack < gemTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*) gemTracks->UncheckedAt(iTrack);
            FairTrackParam* parFirst = track->GetParamFirst();
            FairTrackParam* parLast = track->GetParamLast();

            // Put here your code ...

            for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
                BmnGemStripHit* hit =
                        (BmnGemStripHit*) gemPoints->UncheckedAt(track->GetHitIndex(iHit));

                // Put here your code ...
            }
        }
    }
}
