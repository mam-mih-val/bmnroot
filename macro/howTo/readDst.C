#include <Rtypes.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TFile.h>
using namespace std;

// @(#)bmnroot/macro/howTo:$Id$
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// readDst.C                                                                  //
//                                                                            //
// An example how to read data (RECO) from bmndst.root                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int readDst(TString fileName = "") {
    if (fileName == "") {
        cout << "File not specified!" << endl;
        return -1;
    }

    // open file with 'bmndata' tree
    TChain* out = new TChain("bmndata");
    out->Add(fileName.Data());
    cout << "#recorded entries = " << out->GetEntries() << endl;

    // read first Run Header if present
    DstRunHeader* run_header = (DstRunHeader*) out->GetCurrentFile()->Get("DstRunHeader");
    if (run_header) {
        cout << "\n|||||||||||||||| RUN SUMMARY |||||||||||||||" << endl;
        cout << "||\t\t\t\t\t  ||" << endl;
        cout << "||   Period:        " << run_header->GetPeriodNumber() << "\t\t\t  ||" << endl;
        cout << "||   Number:        " << run_header->GetRunNumber() << "\t\t  ||" << endl;
        cout << "||   Start Time:    " << run_header->GetStartTime().AsString("s") << "\t  ||" << endl;
        cout << "||   End Time:      " << run_header->GetFinishTime().AsString("s") << "\t  ||" << endl;
        cout << "||   Beam:          A = " << run_header->GetBeamA() << ", Z = " << run_header->GetBeamA() << "\t  ||" << endl;
        cout << "||   Beam energy:   " << run_header->GetBeamEnergy() << " GeV\t\t  ||" << endl;
        cout << "||   Target:        A = " << run_header->GetTargetA() << ", Z = " << run_header->GetTargetZ() << "\t  ||" << endl;
        cout << "||   Field voltage: " << setprecision(4) << run_header->GetMagneticField() << " mV\t\t  ||" << endl;
        cout << "||\t\t\t\t\t  ||" << endl;
        cout << "||||||||||||||||||||||||||||||||||||||||||||\n" << endl;
    }

    // assign TClonesArrays to tree branches
    TClonesArray* globTracks = nullptr;
    out->SetBranchAddress("BmnGlobalTrack", &globTracks);
    if (globTracks == nullptr)
    {
        LOG(error)<<"'BmnGlobalTrack' branch was not found, please, check the structure of the DST file";
        return -2;
    }

    TClonesArray* gemTrack = nullptr;
    out->SetBranchAddress("BmnGemTrack", &gemTrack);
    if (gemTrack == nullptr)
    {
        LOG(error)<<"'BmnGemTrack' branch was not found, please, check the structure of the DST file";
        return -3;
    }

    TClonesArray* gemHits = nullptr;
    out->SetBranchAddress("BmnGemStripHit", &gemHits);
    if (gemHits == nullptr)
    {
        LOG(error)<<"'BmnGemStripHit' branch was not found, please, check the structure of the DST file";
        return -4;
    }

    TClonesArray* silTrack = nullptr;
    out->SetBranchAddress("BmnSiliconTrack", &silTrack);
    if (silTrack == nullptr)
    {
        LOG(error)<<"'BmnSiliconTrack' branch was not found, please, check the structure of the DST file";
        return -5;
    }

    TClonesArray* silHits = nullptr;
    out->SetBranchAddress("BmnSiliconHit", &silHits);
    if (silHits == nullptr)
    {
        LOG(error)<<"'BmnSiliconHit' branch was not found, please, check the structure of the DST file";
        return -6;
    }

    TClonesArray* tof400Hits = nullptr;
    out->SetBranchAddress("BmnTof400Hit", &tof400Hits);
    if (tof400Hits == nullptr)
    {
        LOG(error)<<"'BmnTof400Hit' branch was not found, please, check the structure of the DST file";
        return -7;
    }

    TClonesArray* tof700Hits = nullptr;
    out->SetBranchAddress("BmnTof700Hit", &tof700Hits);
    if (tof700Hits == nullptr)
    {
        LOG(error)<<"'BmnTof700Hit' branch was not found, please, check the structure of the DST file";
        return -8;
    }

    TClonesArray* cscHits = nullptr;
    out->SetBranchAddress("BmnCSCHit", &cscHits);
    if (cscHits == nullptr)
    {
        LOG(error)<<"'BmnCSCHit' branch was not found, please, check the structure of the DST file";
        return -9;
    }

    TClonesArray* dchTracks = nullptr;
    out->SetBranchAddress("BmnDchTrack", &dchTracks);
    if (dchTracks == nullptr)
    {
        LOG(error)<<"'BmnDchTrack' branch was not found, please, check the structure of the DST file";
        return -10;
    }

    TClonesArray* vertices = nullptr;
    out->SetBranchAddress("BmnVertex", &vertices);
    if (vertices == nullptr)
    {
        LOG(error)<<"'BmnVertex' branch was not found, please, check the structure of the DST file";
        return -11;
    }

    // event loop
    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        if (iEv % 1000 == 0)
            cout << "Event# " << iEv << endl;

        out->GetEntry(iEv);

        // 0 -- primary, 1 -- secondary
        for (Int_t iVertex = 0; iVertex < vertices->GetEntriesFast(); iVertex++) {
            CbmVertex* vertex = (CbmVertex*) vertices->UncheckedAt(iVertex);

            // See $VMCWORKDIR/bmndata/cbm/CbmVertex.h to get more ...
        }

        for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); iTrack++) {
            BmnGlobalTrack* track = (BmnGlobalTrack*) globTracks->UncheckedAt(iTrack);

            // Getting track params. predicted by Kalman filter ...
            FairTrackParam* parFirst = track->GetParamFirst();
            FairTrackParam* parLast = track->GetParamLast();

            // See YOUR_FAIRROOT_INSTALATION/include/FairTrackParam.h to get more ...

            // Inner tracker contains GEM and SILICON detectors
            BmnTrack* gemTr = nullptr;
            BmnTrack* silTr = nullptr;

            // GEM tracks ...
            if (track->GetGemTrackIndex() != -1) {
                gemTr = (BmnTrack*) gemTrack->UncheckedAt(track->GetGemTrackIndex());

                // Hits on GEM track ...
                for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++)
                {
                    BmnGemStripHit* hit = (BmnGemStripHit*) gemHits->UncheckedAt(gemTr->GetHitIndex(iHit));

                    // See $VMCWORKDIR/bmndata/BmnGemStripHit.h to get more ...
                }
            }

            // Silicon tracks ...
            if (track->GetSilTrackIndex() != -1) {
                silTr = (BmnTrack*) silTrack->UncheckedAt(track->GetSilTrackIndex());

                // Hits on SILICON track
                for (Int_t iHit = 0; iHit < silTr->GetNHits(); iHit++) {
                    BmnSiliconHit* hit = (BmnSiliconHit*) silHits->UncheckedAt(silTr->GetHitIndex(iHit));

                    // See $VMCWORKDIR/bmndata/BmnSiliconHit.h to get more ...
                }
            }

            // TOF400 matched hit ...
            if (track->GetTof1HitIndex() != -1) {
                BmnTofHit* hit = (BmnTofHit*) tof400Hits->UncheckedAt(track->GetTof1HitIndex());

                // See $VMCWORKDIR/bmndata/BmnTofHit.h to get more ...
            }

            // TOF700 matched hit ...
            if (track->GetTof2HitIndex() != -1) {
                BmnTofHit* hit = (BmnTofHit*) tof700Hits->UncheckedAt(track->GetTof2HitIndex());

                // See $VMCWORKDIR/bmndata/BmnTofHit.h to get more ...               
            }

            // CSC matched hits ...
            for (Int_t indHit : track->GetVectorCscHitIndices())
                if (track->GetCscHitIndex(indHit) != -1) {
                    BmnCSCHit* hit = (BmnCSCHit*) cscHits->UncheckedAt(track->GetCscHitIndex(indHit));

                    // See $VMCWORKDIR/bmndata/BmnCSCHit.h to get more ...
                }

            // DCH matched track
            if (track->GetDchTrackIndex() != -1) {
                BmnDchTrack* dchTrack = (BmnDchTrack*) dchTracks->UncheckedAt(track->GetDchTrackIndex());

                FairTrackParam* parFirstDch = dchTrack->GetParamFirst();

                // See YOUR_FAIRROOT_INSTALATION/include/FairTrackParam.h to get more ...
            }
        }// for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); iTrack++)
    }// for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++)

    return 0;
}
