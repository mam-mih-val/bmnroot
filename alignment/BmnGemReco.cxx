#include <stdlib.h>

#include "BmnGemReco.h"
#include "../gem/BmnGemStripConfiguration.h"

Int_t BmnGemReco::fCurrentEvent = 0;

BmnGemReco::BmnGemReco() :
fGemDigits(NULL),
fGemHits(NULL), fDebugInfo(kFALSE), fContainer(NULL),
fMaxNofHits(30),
fGemTracks(NULL), fChi2MaxPerNDF(LDBL_MAX),
fMinHitsAccepted(3), fXMin(-LDBL_MAX),
fXMax(LDBL_MAX), fYMin(-LDBL_MAX), fYMax(LDBL_MAX),
fTxMin(-LDBL_MAX), fTxMax(LDBL_MAX), fTyMin(-LDBL_MAX),
fTyMax(LDBL_MAX) {

    // Declare branch names here 
    hitsBranch = "BmnGemStripHit";
    tracksBranch = "BmnGemTrack";
    tracksSelectedBranch = "BmnAlignmentContainer";

}

InitStatus BmnGemReco::Init() {
    cout << "\nBmnGemReco::Init()\n ";

    FairRootManager* ioman = FairRootManager::Instance();
    fGemDigits = (TClonesArray*) ioman->GetObject("GEM"); // Get input branch 

    fGemHits = new TClonesArray(hitsBranch);
    fGemTracks = new TClonesArray(tracksBranch);
    fContainer = new TClonesArray(tracksSelectedBranch);

    // Register output branches
    ioman->Register(hitsBranch, "GEM", fGemHits, kTRUE);
    ioman->Register(tracksBranch, "GEM", fGemTracks, kTRUE);
    ioman->Register(tracksSelectedBranch, "GEM", fContainer, kTRUE);

    // Create current geometry
    fDetector = new BmnGemStripStationSet_RunSummer2016(BmnGemStripConfiguration::RunSummer2016);
    const Int_t nStat = fDetector->GetNStations();
    const Int_t nParams = 3; // x, y, z
    
    // Read rough corrections from the file
    corr = new Double_t**[nStat];
    for (Int_t iStat = 0; iStat < nStat; iStat++) {
        Int_t nModul = fDetector->GetGemStation(iStat)->GetNModules();
        corr[iStat] = new Double_t*[nModul];
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            corr[iStat][iMod] = new Double_t[nParams];
            for (Int_t iPar = 0; iPar < nParams; iPar++)
                corr[iStat][iMod][iPar] = 0.;
        }
    }

    ReadFileCorrections(fDetector);

    if (fDebugInfo) {
        cout << "Rough corrections are: " << endl;
        for (Int_t iStat = 0; iStat < nStat; iStat++) {
            Int_t nModul = fDetector->GetGemStation(iStat)->GetNModules();
            for (Int_t iMod = 0; iMod < nModul; iMod++) 
                for (Int_t iPar = 0; iPar < nParams; iPar++)
                    cout << " Stat " << iStat << " Mod " << iMod << " Par " << corr[iStat][iMod][iPar] << endl;
        }
    }

    return kSUCCESS;
}

void BmnGemReco::Exec(Option_t* opt) {
    PrepareData();
}

void BmnGemReco::Finish() {
    cout << "\nBmnGemReco::Finish()\n";

    // Delete dynamic array
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
        Int_t nModul = fDetector->GetGemStation(iStat)->GetNModules();
        for (Int_t iMod = 0; iMod < nModul; iMod++) {
            delete corr[iStat][iMod];
        }
        delete[] corr[iStat];
    }
    delete[] corr;

    delete fDetector;
}

void BmnGemReco::PrepareData() {
    fCurrentEvent++;
    if (fCurrentEvent % 1000 == 0)
        cout << "Event# = " << fCurrentEvent << endl;

    fDetector->Reset();

    fGemHits->Delete();
    fGemTracks->Delete();
    fContainer->Delete();

    //       Loop over digits and put a signal to strips
    for (Int_t iDigit = 0; iDigit < fGemDigits->GetEntriesFast(); iDigit++) {
        BmnGemStripDigit* dig = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDigit);

        BmnGemStripStation* station = fDetector->GetGemStation(dig->GetStation());
        BmnGemStripModule* module = station->GetModule(dig->GetModule());

        Int_t layer = dig->GetStripLayer();

        module->SetStripSignalInLayer(layer, dig->GetStripNumber(), dig->GetStripSignal());
    }

    // Create hits in modules
    for (Int_t iStation = 0; iStation < fDetector->GetNStations(); ++iStation) {
        BmnGemStripStation* station = fDetector->GetGemStation(iStation);
        station->ProcessPointsInStation();

        for (Int_t iMod = 0; iMod < station->GetNModules(); iMod++) {
            BmnGemStripModule* module = station->GetModule(iMod);

            Double_t z = module->GetZPositionRegistered();

            Int_t NIntersectionPointsInModule = module->GetNIntersectionPoints();

            for (Int_t iPoint = 0; iPoint < NIntersectionPointsInModule; ++iPoint) {
                Double_t x = module->GetIntersectionPointX(iPoint);
                Double_t y = module->GetIntersectionPointY(iPoint);

                Double_t x_err = module->GetIntersectionPointXError(iPoint);
                Double_t y_err = module->GetIntersectionPointYError(iPoint);
                Double_t z_err = 0.0;

                x *= -1.;
                x += corr[iStation][iMod][0];
                y += corr[iStation][iMod][1];
                z += corr[iStation][iMod][2];

                if (x < fXMin || x > fXMax || y < fYMin || y > fYMax)
                    continue;

                BmnGemStripHit* hit = new((*fGemHits)[fGemHits->GetEntriesFast()]) BmnGemStripHit(iStation, TVector3(x, y, z), TVector3(x_err, y_err, 0.), iPoint);
                hit->SetDx(x_err);
                hit->SetDy(y_err);
                hit->SetDz(z_err);
                hit->SetStation(iStation);
                hit->SetModule(iMod);
                hit->SetIndex(fGemHits->GetEntriesFast() - 1);
            }
        }
    }

    // Checking for maximal number of hits
    if (fGemHits->GetEntriesFast() == 0 || fGemHits->GetEntriesFast() > fMaxNofHits)
        return;

    // Fill hits over stations
    vector <BmnGemStripHit*> stat[fDetector->GetNStations()];
    for (Int_t iHit = 0; iHit < fGemHits->GetEntriesFast(); iHit++) {
        BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(iHit);
        stat[hit->GetStation()].push_back(hit);
    }

    // Checking for empty stations
    Int_t emptyStat = 0;
    vector <Int_t> nonEmptyStatNumber;
    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
        if (stat[iStat].size() < 1)
            emptyStat++;
        else
            nonEmptyStatNumber.push_back(iStat);
    }

    // Checking for (nStat - 1) empty stations
    if (emptyStat == fDetector->GetNStations() - 1)
        return;

    // Checking for minimal number of hits per track
    if (nonEmptyStatNumber.size() == 2)
        return;

    vector <BmnGemStripHit*> hits;
    goToStations(hits, stat, 0);

    // Searching for one track with min. value of chi2 and putting its params. to align. container
    vector <Double_t> chi2;
    if (fBeamRun)
        for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
            chi2.push_back(track->GetChi2());
        }
    vector <Double_t>::const_iterator it_min = min_element(chi2.begin(), chi2.end());
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
        Double_t tx = track->GetParamFirst()->GetTx();
        Double_t ty = track->GetParamFirst()->GetTy();
        Double_t x0 = track->GetParamFirst()->GetX();
        Double_t y0 = track->GetParamFirst()->GetY();
        Double_t z0 = track->GetParamFirst()->GetZ();

        if (fBeamRun) {
            if (tx < fTxMin || tx > fTxMax || ty < fTyMin || ty > fTyMax || Abs(track->GetChi2() - Float_t(*it_min)) > FLT_EPSILON)
                continue;
        } else {
            if (tx < fTxMin || tx > fTxMax || ty < fTyMin || ty > fTyMax)
                continue;
        }

        // Use tracks without common hits
        Bool_t isUsed = kFALSE;
        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(track->GetHitIndex(iHit));
            if (hit->IsUsed()) {
                isUsed = kTRUE;
                break;
            }
            hit->SetUsing(kTRUE);
        }

        if (isUsed)
            continue;

        BmnAlignmentContainer* cont = new ((*fContainer)[fContainer->GetEntriesFast()]) BmnAlignmentContainer();
        cont->SetEventNum(fCurrentEvent);
        cont->GetParamFirst()->SetTx(tx);
        cont->GetParamFirst()->SetTy(ty);
        cont->GetParamFirst()->SetX(x0);
        cont->GetParamFirst()->SetY(y0);
        cont->GetParamFirst()->SetZ(z0);
        cont->SetTrackIndex(iTrack);
        cont->SetNHits(track->GetNHits());
        cont->SetChi2(track->GetChi2());
        cont->SetNDF(track->GetNDF());

        if (fDebugInfo) {
            cout << "Track Info: " << endl;
            cout << "Event# " << fCurrentEvent << endl;
            cout << "Nhits = " << track->GetNHits() << endl;
            cout << "Chi2 = " << track->GetChi2() << endl;
            cout << "NDF = " << track->GetNDF() << endl;
            cout << "Chi2 / NDF = " << track->GetChi2() / track->GetNDF() << endl;
            cout << "Tx = " << tx << " Ty = " << ty << endl;
            cout << "X0 = " << x0 << " Y0 = " << y0 << " Z0 = " << z0 << endl;
            cout << endl;
        }
    }
}

void BmnGemReco::goToStations(vector<BmnGemStripHit*>& hits, vector<BmnGemStripHit*> *hitsOnStation, Int_t stat) {
    Int_t nextStat = stat + 1;

    if (hitsOnStation[stat].size() < 1) {
        if (stat == fDetector->GetNStations() - 1) {
            DeriveFoundTrackParams(hits);
            return;
        }
        goToStations(hits, hitsOnStation, nextStat);
    } else {
        for (Int_t iSize = 0; iSize < hitsOnStation[stat].size(); iSize++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) hitsOnStation[stat].at(iSize);
            hits.push_back(hit);

            if (nextStat == fDetector->GetNStations()) {
                DeriveFoundTrackParams(hits);
                hits.pop_back();
                continue;
            }
            goToStations(hits, hitsOnStation, nextStat);
            hits.pop_back();
        }
    }
    nextStat--;
}

void BmnGemReco::DeriveFoundTrackParams(vector<BmnGemStripHit*> hits) {
    BmnGemTrack* track = new BmnGemTrack();
    FairTrackParam* par = new FairTrackParam();

    for (Int_t iHit = 0; iHit < hits.size(); iHit++) {
        BmnGemStripHit* trHit = ((BmnGemStripHit*) hits.at(iHit));
        track->AddHit(trHit->GetIndex(), trHit);
    }
    track->SortHits();
    TVector3 zxParams = LineFit(track, fGemHits, "ZX"); // Tx, X0
    TVector3 zyParams = LineFit(track, fGemHits, "ZY"); // Ty, Y0

    if (track->GetNHits() >= fMinHitsAccepted && zxParams.Z() / (track->GetNHits() - 2) < fChi2MaxPerNDF && zyParams.Z() / (track->GetNHits() - 2) < fChi2MaxPerNDF) {
        BmnGemTrack* newTrack = new ((*fGemTracks)[fGemTracks->GetEntriesFast()]) BmnGemTrack(*track);
        par->SetX(zxParams.Y());
        par->SetY(zyParams.Y());
        par->SetZ(0.0);
        par->SetTx(zxParams.X());
        par->SetTy(zyParams.X());
        newTrack->SetNDF(newTrack->GetNHits());
        newTrack->SetChi2(Max(zyParams.Z(), zxParams.Z()));
        newTrack->SetNDF(newTrack->GetNHits() - 2); // Since we estimate two params. from linear fit
        newTrack->SetParamFirst(*par);
    }
    delete par;
    delete track;
}

void BmnGemReco::ReadFileCorrections(BmnGemStripStationSet* StationSet) {
    TString dir = getenv("VMCWORKDIR");
    TString pathToFile = dir + "/input/" + "alignCorr_65v2Base.txt";
    ifstream file(pathToFile.Data(), ios::in);

    string line;
    TString stat = "", xCorr = "", yCorr = "", zCorr = "";

    while (getline(file, line)) {
        stringstream ss(line);

        ss >> stat >> xCorr >> yCorr >> zCorr;
        Int_t nMod = StationSet->GetGemStation(stat.Atoi())->GetNModules();

        if (nMod == 1) {
            corr[stat.Atoi()][0][0] = xCorr.Atof();
            corr[stat.Atoi()][0][1] = yCorr.Atof();
            corr[stat.Atoi()][0][2] = zCorr.Atof();

        } else {
            for (Int_t iMod = 0; iMod < nMod; iMod++) {
                corr[stat.Atoi()][iMod][0] = xCorr.Atof();
                corr[stat.Atoi()][iMod][1] = yCorr.Atof();
                corr[stat.Atoi()][iMod][2] = zCorr.Atof();
                ss >> xCorr >> yCorr >> zCorr;
            }
        }

    }
    file.close();
}

ClassImp(BmnGemReco)