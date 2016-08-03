#include <TSystem.h>
#include <TFile.h>

#include "BmnGemAlignment.h"

BmnGemAlignment::BmnGemAlignment(Char_t* filename, Char_t* outname) :
fGemDigits(NULL),        
fRecoFileName(NULL),
fRecoTree(NULL),       
fGemHits(NULL),
fDebugInfo(kFALSE),
fContainer(NULL),
fNstat(7),
fMaxNofHits(fNstat),
fGemTracks(NULL),
fSignalToNoise(2.),
fChi2Max(LDBL_MAX),
fThreshold(0.0),
fMinHitsAccepted(1),
fXhitMin(-LDBL_MAX),
fXhitMax(LDBL_MAX),
fYhitMin(-LDBL_MAX),
fYhitMax(LDBL_MAX),
fChainIn(NULL),
fChainOut(NULL) {
    fDigiFilename = filename;

    fChainIn = new TChain("cbmsim");
    fChainIn->Add(fDigiFilename);
    
   // fChainOut = new TChain("cbmsim");
  
    cout << "#events: " << fChainIn->GetEntries() << endl;
    fNumEvents = fChainIn->GetEntries();
    fChainIn->SetBranchAddress("BmnGemStripDigit", &fGemDigits);

    fRecoFileName = outname;
    
    fRecoFile = new TFile(fRecoFileName, "recreate");
    fRecoTree = new TTree("cbmsim", "cbmsim");
    
    fGemHits =   new TClonesArray("BmnGemStripHit");  
    fGemTracks = new TClonesArray("BmnGemTrack");
    fContainer = new TClonesArray("BmnAlignmentContainer");
    
    fRecoTree->Branch("BmnGemStripHit", &fGemHits);
    fRecoTree->Branch("BmnGemTrack", &fGemTracks);
    fRecoTree->Branch("BmnAlignmentContainer", &fContainer);
}

void BmnGemAlignment::PrepareData() {
    for (Int_t iEv = 0; iEv < fNumEvents; iEv++) {
        fChainIn->GetEntry(iEv);
      //  if (iEv % 1000 == 0)
      //   cout << "Event# = " << iEv << endl;

        fGemHits->Delete();
        fGemTracks->Delete();
        fContainer->Delete();

        BmnGemStripStationSet* fDetector = new BmnGemStripStationSet_RunSummer2016();

        for (Int_t iStation = 0; iStation < fDetector->GetNStations(); ++iStation) {
            BmnGemStripStation* station = fDetector->GetGemStation(iStation);
            for (Int_t iMod = 0; iMod < station->GetNModules(); iMod++) {
                BmnGemStripReadoutModule* module = station->GetReadoutModule(iMod);
                module->SetStripSignalThreshold(fThreshold);
            }
        }

        // Loop over digits and put a signal to strips
        for (Int_t iDigit = 0; iDigit < fGemDigits->GetEntriesFast(); iDigit++) {
            BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigit);
            if (dig->GetStripSignal() / dig->GetStripSignalNoise() < fSignalToNoise)
                continue;

            BmnGemStripStation* station = fDetector->GetGemStation(dig->GetStation());
            BmnGemStripReadoutModule* module = station->GetReadoutModule(dig->GetModule());

            Int_t layer = dig->GetStripLayer();

            if (layer == 0)
                module->SetValueOfLowerStrip(dig->GetStripNumber(), dig->GetStripSignal());

            else
                module->SetValueOfUpperStrip(dig->GetStripNumber(), dig->GetStripSignal());
        }

        for (Int_t iStation = 0; iStation < fDetector->GetNStations(); ++iStation) {
            BmnGemStripStation* station = fDetector->GetGemStation(iStation);
            station->ProcessPointsInStation();

            for (Int_t iMod = 0; iMod < station->GetNModules(); iMod++) {
                BmnGemStripReadoutModule* module = station->GetReadoutModule(iMod);

                Double_t z = module->GetZPositionRegistered();

                Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

                for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                    Double_t x = module->GetIntersectionPointX(iPoint);
                    Double_t y = module->GetIntersectionPointY(iPoint);

                    if (x < fXhitMin || x > fXhitMax || y < fYhitMin || y > fYhitMax)
                        continue;

                    Double_t x_err = module->GetIntersectionPointXError(iPoint);
                    Double_t y_err = module->GetIntersectionPointYError(iPoint);
                    Double_t z_err = 0.0;

                    BmnGemStripHit* hit = new((*fGemHits)[fGemHits->GetEntriesFast()]) BmnGemStripHit(iStation, TVector3(x, y, z), TVector3(x_err, y_err, 0.), iPoint);
                    hit->SetDx(x_err);
                    hit->SetDy(y_err);
                    hit->SetDz(z_err);
                    hit->SetStation(iStation);
                    hit->SetModule(iMod);
                }
            }
        }

        // Checking for maximal number of hits
        if (fGemHits->GetEntriesFast() == 0 || fGemHits->GetEntriesFast() > fMaxNofHits) {
            // cout << iEv << endl;
//            new ((*fGemTracks)[fGemTracks->GetEntriesFast()]) BmnGemTrack();
//            new ((*fContainer)[fContainer->GetEntriesFast()]) BmnAlignmentContainer();
            delete fDetector;
            continue;
        }

        // Fill hits over stations
        vector <BmnGemStripHit*> stat[fNstat];
        for (Int_t iHit = 0; iHit < fGemHits->GetEntriesFast(); iHit++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(iHit);
            stat[hit->GetStation()].push_back(hit);
        }

        // Checking for empty stations
        Int_t emptyStat = 0;
        vector <Int_t> nonEmptyStatNumber;
        for (Int_t iStat = 0; iStat < fNstat; iStat++) {
            if (stat[iStat].size() < 1)
                emptyStat++;
            else
                nonEmptyStatNumber.push_back(iStat);
        }

        // Checking for (nStat - 1) empty stations 
        if (emptyStat == fNstat - 1) {
            // cout << iEv << endl;
//            new ((*fGemTracks)[fGemTracks->GetEntriesFast()]) BmnGemTrack();
//            new ((*fContainer)[fContainer->GetEntriesFast()]) BmnAlignmentContainer();
            delete fDetector;
            continue;
        }

        // Checking for minimal number of hits per track
        if (nonEmptyStatNumber.size() < fMinHitsAccepted) {
            // cout << iEv << endl;
//            new ((*fGemTracks)[fGemTracks->GetEntriesFast()]) BmnGemTrack();
//            new ((*fContainer)[fContainer->GetEntriesFast()]) BmnAlignmentContainer();
            delete fDetector;
            continue;
        }

        //                cout << "Event # =  " << iEv << endl;
        //                for (Int_t iStat = 0; iStat < fNstat; iStat++)
        //                    cout << stat[iStat].size() << " ";
        //                cout << endl;


        vector <BmnGemStripHit*> hits;
        goToStations(fGemTracks, hits, stat, 0);
     
        // Searching for one track with min. value of chi2 and putting its params. to align. container
        vector <Float_t> chi2;
        for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
            chi2.push_back(track->GetChi2());
        }
        vector <Float_t>::const_iterator it_min = min_element(chi2.begin(), chi2.end());
        for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
            if (Abs(track->GetChi2() - *it_min) < 0.1) {
                BmnAlignmentContainer* cont = new ((*fContainer)[fContainer->GetEntriesFast()]) BmnAlignmentContainer();
                cont->SetTx(track->GetParamFirst()->GetTx());
                cont->SetTy(track->GetParamFirst()->GetTy());
                cont->SetX0(track->GetParamFirst()->GetX());
                cont->SetY0(track->GetParamFirst()->GetY());
                cont->SetZ0(track->GetParamFirst()->GetZ());
                cont->SetTrackHits(track->GetHits());
                break;
            }
        }
       
        fRecoTree->Fill();
        delete fDetector;
        cout << iEv << endl;
    } 
    fRecoTree->Write();
    fRecoFile->Close(); 
    // delete fRecoFile;
}

void BmnGemAlignment::StartMille() {
//    fRecoFile = new TFile(fRecoFileName, "read");
    fChainOut = new TChain("cbmsim");
    fChainOut->Add(fRecoFileName);
//    cout << fRecoFile->GetName() << endl; 
    
//   fContainer = NULL;
//    TFile file(fRecoFileName);
//    TTree* tree = (TTree*)file.Get("BmnAlignmentContainer");
//    tree->SetBranchAddress("BmnAlignmentContainer", &fContainer);
//    
 //   cout << fContainer->GetEntriesFast() << endl;

    cout << "#recorded tracks = " << fChainOut->GetEntries() << endl;
    TClonesArray* hits = NULL;
    TClonesArray* tracks = NULL;
    TClonesArray* align = NULL;
    
    fChainOut->SetBranchAddress("BmnGemStripHit", &hits);
    fChainOut->SetBranchAddress("BmnGemTrack", &tracks);
    fChainOut->SetBranchAddress("BmnAlignmentContainer", &align);
//    
    for (Int_t iEv = 0; iEv < fChainOut->GetEntries(); iEv++) {
        cout << iEv << endl;
        fChainOut->GetEntry(iEv);
//      
//       for (Int_t iHit = 0; iHit = hits->GetEntriesFast(); iHit++) {
//           BmnGemStripHit* hit = (BmnGemStripHit*)hits->UncheckedAt(iHit);
//           cout << hit->GetX() << " " << hit->GetY() << endl;
//       }
//       
//        for (Int_t iTrack = 0; iTrack = tracks->GetEntriesFast(); iTrack++) {
//           BmnGemTrack* track = (BmnGemTrack*)tracks->UncheckedAt(iTrack);
//           cout << track->GetChi2() << endl;
//        }
//        
//        for (Int_t iAlign = 0; iAlign = align->GetEntriesFast(); iAlign++) {
//           BmnAlignmentContainer* cont = (BmnAlignmentContainer*)align->UncheckedAt(iAlign);
//           cout << cont->GetTx() << endl;
//       }
    }
}

void BmnGemAlignment::goToStations(TClonesArray* tracks, vector<BmnGemStripHit*>& hits, vector<BmnGemStripHit*> *hitsOnStation, Int_t stat) {
    Int_t nextStat = stat + 1;

    if (hitsOnStation[stat].size() < 1) {
        if (stat == fNstat - 1) {
            DeriveFoundTrackParams(hits);
            return;
        }
        goToStations(tracks, hits, hitsOnStation, nextStat);
    } else {
        for (Int_t iSize = 0; iSize < hitsOnStation[stat].size(); iSize++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) hitsOnStation[stat].at(iSize);
            hits.push_back(hit);

            if (nextStat == fNstat) {
                DeriveFoundTrackParams(hits);
                hits.pop_back();
                continue;
            }
            goToStations(tracks, hits, hitsOnStation, nextStat);
            hits.pop_back();
        }
    }
    nextStat--;
}

void BmnGemAlignment::DeriveFoundTrackParams(vector<BmnGemStripHit*> hits) {
    TVector3 vertex, direction;
    Float_t chi2 = LineFit3D(hits, vertex, direction);
    if (chi2 > 0 && chi2 < fChi2Max) {
        BmnGemTrack track;
        FairTrackParam par;
        CreateTrack(direction, vertex, track, par, chi2, hits.size());
        track.SetHits(hits);
        new ((*fGemTracks)[fGemTracks->GetEntriesFast()]) BmnGemTrack(track);
    }
}

BmnGemAlignment::~BmnGemAlignment() {
//    fRecoTree->Write();
//    fRecoFile->Close();
//    delete fContainer;
    delete fChainIn;
    delete fChainOut;
}

Float_t BmnGemAlignment::LineFit3D(vector<BmnGemStripHit*> hits, TVector3& vertex, TVector3& direction) {
    //    cout << "LineFit3D started!" << endl;
    const Int_t nHits = hits.size();

    Float_t Xi = 0.0, Yi = 0.0, Zi = 0.0; // coordinates of current track point
    Float_t Ci;
    Float_t SumXW = 0.0, SumYW = 0.0;
    Float_t SumXWC = 0.0, SumYWC = 0.0;
    Float_t SumW = 0.0;
    Float_t SumC = 0.0, SumC2 = 0.0;
    Float_t Wi = 1.0 / nHits; // weight    
    Float_t ZV = ((BmnGemStripHit*) hits.at(0))->GetZ(); //Z-coordinate of vertex
    Float_t ZN = ((BmnGemStripHit*) hits.at(nHits - 1))->GetZ();
    Float_t Az = (ZN - ZV);
    if (Az == 0.0) {
        cout << "Az = 0.0" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return 1000.0;
    }
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) hits.at(i);
        if (hit == NULL) continue;
        Xi = hit->GetX();
        Yi = hit->GetY();
        Zi = hit->GetZ();
        Ci = (Zi - ZV) / Az;
        SumXW += (Xi * Wi);
        SumYW += (Yi * Wi);
        SumXWC += (Xi * Wi * Ci);
        SumYWC += (Yi * Wi * Ci);
        SumC += (Ci * Wi);
        SumC2 += (Ci * Ci * Wi);
        SumW += Wi;
    }
    Float_t koef = (SumC2 * SumW - SumC * SumC);
    if (koef == 0.0) {
        cout << "coef = 0.0" << endl;
        vertex = TVector3(0.0, 0.0, 0.0);
        direction = TVector3(0.0, 0.0, 0.0);
        return 1000.0;
    }
    Float_t XV = (SumXW * SumC2 - SumC * SumXWC) / koef;
    Float_t YV = (SumYW * SumC2 - SumC * SumYWC) / koef;
    // cout << "Xv = " << XV << " Yv = " << YV << endl;
    Float_t Ax = (SumXWC * SumW - SumC * SumXW) / koef;
    Float_t Ay = (SumYWC * SumW - SumC * SumYW) / koef;

    vertex = TVector3(XV, YV, ZV);
    direction = TVector3(Ax, Ay, Az);
    //    vertex.Print();
    //    direction.Print();

    //chi2 calculation
    Float_t sumChi2xy = 0.0;
    Float_t sumChi2xz = 0.0;
    Float_t sumChi2yz = 0.0;
    for (Int_t i = 0; i < nHits; ++i) {
        BmnGemStripHit* hit = (BmnGemStripHit*) hits.at(i);
        if (hit == NULL) continue;
        Xi = hit->GetX();
        Yi = hit->GetY();
        Zi = hit->GetZ();
        // cout << "Xi =  " << Xi << " Yi = " << Yi << " Zi = " << Zi << endl;
        //XY
        Float_t Y_Xi = Ay / Ax * (Xi - XV) + YV;
        Float_t X_Zi = Ax / Az * (Zi - ZV) + XV;
        Float_t Y_Zi = Ay / Az * (Zi - ZV) + YV;
        sumChi2xy += ((Y_Xi - Yi) * (Y_Xi - Yi) / Yi / Yi);
        // cout << "sumChi2xy in summing = " << sumChi2xy << endl;
        sumChi2xz += ((X_Zi - Xi) * (X_Zi - Xi) / Xi / Xi);
        sumChi2yz += ((Y_Zi - Yi) * (Y_Zi - Yi) / Yi / Yi);
    }
    sumChi2xy /= (nHits - 1);
    sumChi2xz /= (nHits - 1);
    sumChi2yz /= (nHits - 1);
    // cout << "sumChi2xy =  " << sumChi2xy << " sumChi2xz = " << sumChi2xz << " sumChi2yz = " << sumChi2yz << " nHits = " << nHits << endl;

    Float_t chi2 = Max(sumChi2xy, Max(sumChi2xz, sumChi2yz));
    // cout << " Chi2_______ = " << chi2 << endl;
    // if (chi2 < 1000) return chi2;
    // else return -1000.0;
    return chi2;
}

void BmnGemAlignment::CreateTrack(TVector3 dir, TVector3 vert, BmnGemTrack& track, FairTrackParam& par, Float_t chi2, Int_t nHits) {
    par.SetPosition(vert);
    par.SetTx(dir.X() / dir.Z());
    par.SetTy(dir.Y() / dir.Z());
    par.SetQp(dir.Mag());
    track.SetParamFirst(par);
    track.SetChi2(chi2);
    track.SetNDF(nHits);
}

Bool_t BmnGemAlignment::isOneTrack(TClonesArray* hits) {
    Bool_t oneTrack = kTRUE;

    cout << "nHits = " << hits->GetEntriesFast() << endl;
    for (Int_t iHit = 1; iHit < hits->GetEntriesFast(); iHit++) {
        BmnGemStripHit* hit = (BmnGemStripHit*) hits->UncheckedAt(iHit);
        Short_t stat = hit->GetStation();
        cout << " iHit = " << iHit << " stat = " << stat << endl;
        if (iHit != stat) {
            oneTrack = kFALSE;
            break;
        }
    }
    return oneTrack;
}

ClassImp(BmnGemAlignment)