// @(#)bmnroot/macro/howTo:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-07-18, edited by KG

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// readDst.C                                                                  //
//                                                                            //
// An example how to read data (RECO) from bmndst.root                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void readDst(TString fileName = "") {
    if (fileName == "") {
        cout << "File not specified!" << endl;
        return;
    }

    // open file with 'bmndata' tree
    TChain* out = new TChain("bmndata");
    out->Add(fileName.Data());
    cout << "#recorded entries = " << out->GetEntries() << endl;

    // read first Run Header if present
    DstRunHeader* run_header = (DstRunHeader*) out->GetCurrentFile()->Get("DstRunHeader");
    if (run_header)
    {
        cout << "\n|||||||||||||||| RUN SUMMARY |||||||||||||||" << endl;
        cout << "||\t\t\t\t\t  ||" << endl;
        cout << "||   Period:        " << run_header->GetPeriodNumber() << "\t\t\t  ||" << endl;
        cout << "||   Number:        " << run_header->GetRunNumber() << "\t\t  ||" << endl;
        cout << "||   Start Time:    " << run_header->GetStartTime().AsString("s") << "\t  ||" << endl;
        cout << "||   End Time:      " << run_header->GetFinishTime().AsString("s") << "\t  ||" << endl;
        cout << "||   Beam:          A = " << run_header->GetBeamA() << ", Z = " << run_header->GetBeamA() << "\t  ||" << endl;
        cout << "||   Beam energy:   " << run_header->GetBeamEnergy() << " GeV\t\t  ||" << endl;
        cout << "||   Target:        A = " << run_header->GetTargetA() << ", Z = " << run_header->GetTargetZ() << "\t  ||" << endl;
        cout << "||   Field voltage: " << setprecision(4) << run_header->GetMagneticField()  << " mV\t\t  ||" << endl;
        cout << "||\t\t\t\t\t  ||" << endl;
        cout << "||||||||||||||||||||||||||||||||||||||||||||\n" << endl;
    }

    // assign TClonesArrays to tree branches
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

    // event loop
    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++)
    {
        out->GetEntry(iEv);

        for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); iTrack++)
        {
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
                for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++)
                {
                    BmnGemStripHit* hit = (BmnGemStripHit*) gemPoints->UncheckedAt(gemTr->GetHitIndex(iHit));

                    // Put here your code ...
                
                }

            // SILICON track
            if (track->GetSilTrackIndex() != -1)
                for (Int_t iHit = 0; iHit < silTr->GetNHits(); iHit++)
                {
                    BmnSiliconHit* hit = (BmnSiliconHit*) silPoints->UncheckedAt(silTr->GetHitIndex(iHit));
                               
                    // Put here your code ...
                
                }
        }// for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); iTrack++)
    }// for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++)
}
