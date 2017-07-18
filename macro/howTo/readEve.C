#include <TChain.h>
#include <TString.h>
#include <TClonesArray.h>

// @(#)bmnroot/macro/howTo:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-07-18

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// readEve.C                                                                  //
//                                                                            //
// An example how to read data (MC) from evetest.root                         //
// It demonstrates how to select GEM hits which belong to the GEM track       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void readEve(TString fileName = "") {
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

    TClonesArray* stsPoints = NULL;
    TClonesArray* stsTracks = NULL;
    out->SetBranchAddress("StsPoint", &stsPoints);
    out->SetBranchAddress("MCTrack", &stsTracks);

    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);

        for (Int_t iTrack = 0; iTrack < stsTracks->GetEntriesFast(); iTrack++) {
            CbmMCTrack* mcTrack = (CbmMCTrack*) stsTracks->UncheckedAt(iTrack);

            // Put here your code ...

            for (Int_t iPoint = 0; iPoint < stsPoints->GetEntriesFast(); iPoint++) {
                CbmStsPoint* stsPoint = (CbmStsPoint*) stsPoints->UncheckedAt(iPoint);

                Int_t TrackID = stsPoint->GetTrackID();

                if (TrackID != iTrack)
                    continue;

                // Put here your code ...
            }
        }
    }
}
