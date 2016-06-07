
#include "BmnGemVertexFinder.h"
#include "BmnMath.h"

static Float_t workTime = 0.0;

using namespace std;
using namespace TMath;

BmnGemVertexFinder::BmnGemVertexFinder() : fEventNo(0) {

    fGemHitsArray = NULL;
    fGemTracksArray = NULL;
    fKalman = NULL;
    fHitsBranchName = "BmnGemStripHit";
    fTracksBranchName = "BmnGemTracks";
    fVertexBranchName = "BmnVertex";
}

BmnGemVertexFinder::~BmnGemVertexFinder() {
}

InitStatus BmnGemVertexFinder::Init() {

    cout << "======================== Vertex finder init started =========================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitsArray = (TClonesArray*) ioman->GetObject(fHitsBranchName); //in
    fGemTracksArray = (TClonesArray*) ioman->GetObject(fTracksBranchName); //in
    fVertexArray = new TClonesArray("CbmVertex", 100); //out
    ioman->Register(fVertexBranchName, "GEM", fVertexArray, kTRUE);

    fField = FairRunAna::Instance()->GetField();

    cout << "======================== Vertex finder init finished ========================" << endl;
}

void BmnGemVertexFinder::Exec(Option_t* opt) {
    clock_t tStart = clock();

    cout << "\n======================== Vertex finder exec started =====================\n" << endl;
    cout << "Event number: " << fEventNo++ << endl;

    fVertexArray->Clear();
    vector<TVector3> vertexes;
    Float_t vx = 0.0;
    Float_t vy = 0.0;
    Float_t vz = 0.0;
    const Int_t nTracks = fGemTracksArray->GetEntriesFast();
    if (nTracks == 0) return;
    //    const Float_t z0 = 0.0;
    const Int_t pdg = 211;

    //    for (Int_t iTr = 0; iTr < nTracks; ++iTr) {
    //        BmnGemTrack* track = (BmnGemTrack*) fGemTracksArray->At(iTr);
    //
    //        if (Abs(track->GetChi2()) > 50) continue;
    //        FairTrackParam par = *(track->GetParamFirst());
    //        fKalman = new BmnKalmanFilter_tmp();
    //
    //        Float_t length = track->GetLength();
    //        vector<Double_t>* F = new vector<Double_t> (25, 0.);
    //        if (F != NULL) {
    //            F->assign(25, 0.);
    //            (*F)[0] = 1.;
    //            (*F)[6] = 1.;
    //            (*F)[12] = 1.;
    //            (*F)[18] = 1.;
    //            (*F)[24] = 1.;
    //        }
    //
    //        fKalman->TGeoTrackPropagate(&par, z0, pdg, F, &length, "field");
    //        vertexes.push_back(TVector3(par.GetX(), par.GetX(), par.GetX()));
    //        vx += par.GetX();
    //        vy += par.GetY();
    //        vz += par.GetZ();
    //
    //        delete F;
    //        delete fKalman;
    //    }
    //
    //    vx /= nTracks;
    //    vy /= nTracks;
    //    vz /= nTracks;

    const Float_t z0 = -1.0;
    const Float_t z1 = 1.0;

    Float_t l1(0.0), l2(0.0), l3(0.0), l4(0.0);
    Float_t t1(0.0), t2(0.0), t3(0.0), t4(0.0);
    Float_t k1(0.0), k2(0.0), k3(0.0), k4(0.0);

    for (Int_t iTr = 0; iTr < nTracks; ++iTr) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracksArray->At(iTr);
        if (Abs(track->GetChi2()) > 50) continue;
//        if (Abs(track->GetLength()) < 100) continue;
        FairTrackParam par0 = *(track->GetParamFirst());
        FairTrackParam par1 = *(track->GetParamFirst());
        fKalman = new BmnKalmanFilter_tmp();
        vector<Double_t>* F = new vector<Double_t> (25, 0.);
        if (F != NULL) {
            F->assign(25, 0.);
            (*F)[0] = 1.;
            (*F)[6] = 1.;
            (*F)[12] = 1.;
            (*F)[18] = 1.;
            (*F)[24] = 1.;
        }

        fKalman->TGeoTrackPropagate(&par0, z0, pdg, F, 0, "field");
        fKalman->TGeoTrackPropagate(&par1, z1, pdg, F, 0, "field");
        Float_t x0 = par0.GetX();
        Float_t y0 = par0.GetY();
        Float_t x1 = par1.GetX();
        Float_t y1 = par1.GetY();
        
        Float_t a1 = (y1 - y0) / (x1 - x0);
        Float_t a2 = (x1 - x0) / (z1 - z0);
        Float_t a3 = (y1 - y0) / (z1 - z0);
        
        Float_t b1 = y0 - a1 * x0;
        Float_t b2 = x0 - a2 * z0;
        Float_t b3 = y0 - a3 * z0;
        
        Float_t A1 = 1 / (1 + a1 * a1);
        Float_t A2 = 1 / (1 + a2 * a2);
        Float_t A3 = 1 / (1 + a3 * a3);
        
        l1 += (a1 * a1 * A1 + A2);
        l2 += (-a1 * A1);
        l3 += (-a2 * A2);
        l4 += (a1 * b1 * A1 - b2 * A2);
        
        t1 += (a1 * A1);
        t2 += (A1 + A3);
        t3 += (-a3 * A3);
        t4 += (-b1 * A1 - b3 * A3);
        
        k1 += (-a2 * A2);
        k2 += (-a3 * A3);
        k3 += (a2 * a2 * A2 + a3 * a3 * A3);
        k4 += (a2 * b2 * A2 + a3 * b3 * A3);
                
        delete F;
        delete fKalman;
    }
    
    vz = (l2 * (t4 * k1 - k4 * t1) + l1 * (t2 * k4 - k2 * t4) + l4 * (t1 * k2 - k1 * t2)) / (l2 * (t3 * k1 - k3 * t1) + l1 * (t2 * k3 - k2 * t3) + l3 * (t1 * k2 - k1 * t2));
    vy = (t4 * l1 - t1 * l4 - vz * (l1 * t3 - t1 * l3)) / (l1 * t2 - t1 * l2);
    vx = (l4 - vz * l3 - vy * l2) / l1;


    for (Int_t iTr = 0; iTr < nTracks; ++iTr) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracksArray->At(iTr);
        if (Abs(track->GetChi2()) > 50) continue;
        FairTrackParam par0 = *(track->GetParamFirst());
        fKalman = new BmnKalmanFilter_tmp();
        vector<Double_t>* F = new vector<Double_t> (25, 0.);
        if (F != NULL) {
            F->assign(25, 0.);
            (*F)[0] = 1.;
            (*F)[6] = 1.;
            (*F)[12] = 1.;
            (*F)[18] = 1.; 
            (*F)[24] = 1.;
        }

        fKalman->TGeoTrackPropagate(&par0, vz, pdg, F, 0, "field");
        Float_t xi = par0.GetX();
        Float_t yi = par0.GetY();
        track->SetB(Sqrt((yi - vy) * (yi - vy) + (xi - vx) * (xi - vx)));
        delete F;
        delete fKalman;

    }

    //    cout << "vx = " << vx << "   vy = " << vy << endl;

    new((*fVertexArray)[fVertexArray->GetEntriesFast()]) CbmVertex("vertex", "vertex", vx, vy, vz, 0.0, 0, nTracks, TMatrixFSym(3));

    cout << "\n======================== Vertex finder exec finished ========================" << endl;

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnGemVertexFinder::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt", ofstream::app);
    outFile << "Vertex Finder Time: " << workTime;
    cout << "Work time of the GEM vertex finder: " << workTime << endl;
}