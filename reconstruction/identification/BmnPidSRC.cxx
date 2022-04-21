#include "BmnPidSRC.h"

static Float_t workTime = 0.0;

using namespace std;
using namespace TMath;

Int_t nFoundAZ = 0;
Int_t nFoundPZ = 0;


    const Double_t AzUpBorder[8][14] = {    // so called "pupochki"
//A  0     1     2     3     4     5     6     7     8     9    10    11    12    13
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}, // for Z=0
    {0, 1.11, 2.17,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}, // for Z=1
    {0,    0,    0, 1.53, 2.15,    0,    0,    0,    0,    0,    0,    0,    0,    0}, // for Z=2
    {0,    0,    0,    0,    0, 1.55, 2.15,    0,    0,    0,    0,    0,    0,    0}, // for Z=3
    {0,    0,    0,    0,    0,    0,    0, 1.93,    0, 2.50,    0,    0,    0,    0}, // for Z=4
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 2.15, 2.30,    0,    0}, // for Z=5
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 2.00, 2.08,    0}, // for Z=6
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}  // for Z=7
    };
    const Double_t AzDownBorder[8][14] = {
//A  0     1     2     3     4     5     6     7     8     9     10    11    12    13
               
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=0
    {0,    0, 0.83, 1.83,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=1
    {0,    0,    0, 1.01, 1.85,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=2
    {0,    0,    0,    0,    0, 1.03, 1.85,    0,    0,    0,     0,    0,    0,    0}, // for Z=3
    {0,    0,    0,    0,    0,    0,    0, 1.45,    0, 2.06,     0,    0,    0,    0}, // for Z=4
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  1.91,  2.1,    0,    0}, // for Z=5
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 1.68, 1.92,    0}, // for Z=6
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}  // for Z=7
    };

    const Double_t ZAOutUpBorder[8][14] = {
//A  0     1     2     3     4     5     6     7     8     9     10    11    12    13
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=0
    {0,  1.3,  1.3,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=1
    {0,    0,    0,  2.6,  2.6,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=2
    {0,    0,    0,    0,    0,  3.3,  3.3,    0,    0,    0,     0,    0,    0,    0}, // for Z=3
    {0,    0,    0,    0,    0,    0,    0,  4.2,    0,  4.2,     0,    0,    0,    0}, // for Z=4
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  5.25, 5.25,    0,    0}, // for Z=5
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,  6.3,  6.3,    0}, // for Z=6
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}  // for Z=7
    };
    const Double_t ZAOutDownBorder[8][14] = {
//A  0     1     2     3     4     5     6     7     8     9     10    11    12    13
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=0
    {0,  0.9,  0.9,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=1
    {0,    0,    0,   2.,   2.,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=2
    {0,    0,    0,    0,    0,  2.7,  2.7,    0,    0,    0,     0,    0,    0,    0}, // for Z=3
    {0,    0,    0,    0,    0,    0,    0,  2.6,    0,  2.6,     0,    0,    0,    0}, // for Z=4
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  4.75, 4.75,    0,    0}, // for Z=5
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,  5.7,  5.7,    0}, // for Z=6
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}  // for Z=7
    };        

    const Double_t RigidityUpBorder[8][14] = {
//A  0     1     2     3     4     5     6     7     8     9     10    11    12    13
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=0
    {0,    0,  9.1,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=1
    {0,    0,    0,  6.9,  8.9,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=2
    {0,    0,    0,    0,    0,  6.9,    7,    0,    0,    0,     0,    0,    0,    0}, // for Z=3
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=4
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=5
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,  8.7,    0}, // for Z=6
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}  // for Z=7
    };
    const Double_t RigidityDownBorder[8][14] = {
//A  0     1     2     3     4     5     6     7     8     9     10    11    12    13
               
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=0
    {0,    0,  7.3,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=1
    {0,    0,    0,  5.5,  7.5,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=2
    {0,    0,    0,    0,    0,  5.5,  7.6,    0,    0,    0,     0,    0,    0,    0}, // for Z=3
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=4
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=5
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,  7.5,    0}, // for Z=6
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}  // for Z=7
    };
    const Double_t ZOutUpBorder[8][14] = {
//A  0     1     2     3     4     5     6     7     8     9     10    11    12    13
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=0
    {0,    0,  1.7,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=1
    {0,    0,    0,  2.6,  2.6,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=2
    {0,    0,    0,    0,    0,  3.3,  3.3,    0,    0,    0,     0,    0,    0,    0}, // for Z=3
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=4
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=5
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,  6.3,    0}, // for Z=6
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}  // for Z=7
    };
    const Double_t ZOutDownBorder[8][14] = {
//A  0     1     2     3     4     5     6     7     8     9     10    11    12    13
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=0
    {0,    0,  1.1,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=1
    {0,    0,    0,   2.,   2.,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=2
    {0,    0,    0,    0,    0,  2.7,  2.7,    0,    0,    0,     0,    0,    0,    0}, // for Z=3
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=4
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}, // for Z=5
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,  5.7,    0}, // for Z=6
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0,    0,    0,    0}  // for Z=7
    };        

BmnPidSRC::BmnPidSRC() {
    fEventNo = 0;
    fGlobalTracksArray = nullptr;
    fDstEventHeader = nullptr;
    fBmnDchTrack=nullptr;
    fBmnTofHit=nullptr;
    fKalman = nullptr;
    fZout=0.0;
    fGlobalTracksBranchName = "BmnGlobalTrack";
    fDstEventHeaderBranchName = "DstEventHeader.";
    fDchTrackBranchName = "BmnDchTrack";
    fTofHitBranchName = "BmnTof700Hit";   
}

InitStatus BmnPidSRC::Init() {

    if (fVerbose > 1) cout << "=========================== Pid init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    fGlobalTracksArray = (TClonesArray*) ioman->GetObject(fGlobalTracksBranchName); //in
    if (!fGlobalTracksArray) {
        cout << "BmnPidSRC::Init(): branch " << fGlobalTracksBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnDchTrack = (TClonesArray*) ioman->GetObject(fDchTrackBranchName); //in
    if (!fBmnDchTrack) {
        cout << "BmnPidSRC::Init(): branch " << fDchTrackBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnTofHit = (TClonesArray*) ioman->GetObject(fTofHitBranchName); //in
    if (!fBmnTofHit) {
        cout << "BmnPidSRC::Init(): branch " << fTofHitBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fDstEventHeader=(DstEventHeader*) ioman->GetObject(fDstEventHeaderBranchName);
    if (!fDstEventHeader) {
        cout << "BmnPidSRC::Init(): branch " << fDstEventHeaderBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    //fField = FairRunAna::Instance()->GetField();
    fKalman = new BmnKalmanFilter();

    hPIDdch = new TH2F("PIDdch", "; total charge of the event Z; A/z", 500, 0, 10, 500, -2, 7);
    hPIDgem = new TH2F("PIDgem", "; total charge of the event Z; P/q", 500, 0, 10, 500, 0, 15);
    
    if (fVerbose > 1) cout << "=========================== Pid init finished ===================" << endl<<endl<<endl<<endl<<endl;

    return kSUCCESS;
}

void BmnPidSRC::Exec(Option_t* opt) {
    if (!IsActive())
        return;
    clock_t tStart = clock();
    
    AzPID();

//    RigidityPID();

    DrawPID();
        

    if (fVerbose > 1) cout << "\n======================== Pid exec finished ======================" << endl;

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnPidSRC::RigidityPID(){
    if (fDstEventHeader->GetZ2out() > 0) {
            fZout=sqrt(fDstEventHeader->GetZ2out());
        //	fZin=sqrt(fDstEventHeader->GetZ2in());
            
        //    Double_t fZoutTrue=fZout;//*0.797+0.235;
        // cout << "fZoutTrue = " << fZoutTrue << endl;
            for (int i=0; i<8; i++){
                for (int j=0; j<14; j++){
                    if (RigidityDownBorder[i][j]==0) continue;
                    if (fZout>ZOutUpBorder[i][j] || fZout<ZOutDownBorder[i][j]) continue; // Charge cut for isotopes
                    for (int k=0; k<fGlobalTracksArray->GetEntriesFast(); k++){
                        BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracksArray->At(k);
                        //if (track->GetA() > -1) continue;
                        Double_t rigid = 1/track->GetParamLast()->GetQp();
                        //hPIDgem->Fill(fZout, rigid);
                        if (rigid>RigidityUpBorder[i][j] || rigid<RigidityDownBorder[i][j]) continue; // Rigidity cut
                        track->SetA(j);
                        track->SetZ(i);
                        //nFoundPZ++;
                    }
                }
            
        }
    }
    
}

void BmnPidSRC::AzPID(){
    
    if (fDstEventHeader->GetZ2out() > 0) {
        //Double_t fZoutTrue=fZout;//*0.797+0.235;
        // cout << "fZoutTrue = " << fZoutTrue << endl;
        for (int i=0; i<8; i++){
            for (int j=0; j<14; j++){

                if (AzDownBorder[i][j]==0) continue; //unfounded isotopes cut

                fZout=sqrt(fDstEventHeader->GetZ2out());
                if (fZout>ZAOutUpBorder[i][j] || fZout<ZAOutDownBorder[i][j]) continue; // Charge cut for isotopes
                //if(fGlobalTracksArray->GetEntriesFast()>1) continue; // no more than one track in entry
                for (int k=0; k<fGlobalTracksArray->GetEntriesFast(); k++){

                    BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracksArray->At(k);
                    
                    //Int_t index=track->GetTof2HitIndex();
                    //Int_t index1=track->GetDch1TrackIndex();
                    //if(index==-1 || index1==-1) continue; //DCH Tof cut
                    //fZin=sqrt(fDstEventHeader->GetZ2in());
                    //if (fZin>6.5 || fZin<5.5) continue; // Zin only Carbon
                    
                    //for(int m=0; m<fBmnDchTrack->GetEntries(); m++){
                        //BmnDchTrack *dchTrack = (BmnDchTrack *)fBmnDchTrack->At(m);
                        //if(dchTrack->GetParamFirst()->GetZ()<550 || dchTrack->GetParamFirst()->GetZ()>650) continue; // global dch tracks
                        FairTrackParam parPrev(*(track->GetParamLast()));
                        fKalman->TGeoTrackPropagate(&parPrev, 750, 2212, NULL, NULL, kTRUE);
                        Double_t xdch=parPrev.GetX();
                        Double_t Txdch=parPrev.GetTx();
                        Double_t Az=(xdch - Txdch * 1470.88) * 0.026106 + 4.33385;
                        //hPIDdch->Fill(fZout, Az); 
                        if (Az>AzUpBorder[i][j] || Az<AzDownBorder[i][j]) continue; // Az cut
                        track->SetA(j);
                        track->SetZ(i);
                        //nFoundAZ++;
                    
                }
            }
        }
    }
    
}

void BmnPidSRC::DrawPID(){

    if (fDstEventHeader->GetZ2out() > 0) {
        fZout=sqrt(fDstEventHeader->GetZ2out());
        // draw Gem-based PID
        for (int k=0; k<fGlobalTracksArray->GetEntriesFast(); k++){
            BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracksArray->At(k);
            Double_t rigid = 1/track->GetParamLast()->GetQp();
            hPIDgem->Fill(fZout, rigid);
        }

        // draw Dch-based PID
        for(int m=0; m<fBmnDchTrack->GetEntries(); m++){
            BmnDchTrack *dchTrack = (BmnDchTrack *)fBmnDchTrack->At(m);
            if(dchTrack->GetParamFirst()->GetZ()<550 || dchTrack->GetParamFirst()->GetZ()>650) continue; // global dch tracks
            FairTrackParam parPrev(*(dchTrack->GetParamFirst()));
            Double_t xdch=parPrev.GetX();
            Double_t Txdch=parPrev.GetTx();
            Double_t Az=(xdch - Txdch * 1470.88) * 0.026106 + 3.508280;
            hPIDdch->Fill(fZout, Az);
        }
    }
}

void BmnPidSRC::Finish() {
    delete fKalman;
    cout << "Work time of PID: " << workTime << endl;

    TFile pidFile("pid.root", "RECREATE");

    pidFile.Add(hPIDdch);
    pidFile.Add(hPIDgem);

    pidFile.Write();
    pidFile.Close();

    cout << "nFoundAZ = " << nFoundAZ << "     nFoundPZ = " << nFoundPZ << endl;
}

