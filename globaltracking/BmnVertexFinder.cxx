#include "BmnVertexFinder.h"
#include "BmnMath.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "vector"

static Float_t workTime = 0.0;

using namespace std;
using namespace TMath;

BmnVertexFinder::BmnVertexFinder(Int_t period, Bool_t isField) {
    fPeriodId = period;
    fEventNo = 0;
    fGlobalTracksArray = NULL;
    fNTracks = 0;
    fRoughVertex3D = (fPeriodId == 7) ? TVector3(0.5, -4.6, -2.3) : (fPeriodId == 6) ? TVector3(0.0, -3.5, -21.9) : TVector3(0.0, 0.0, 0.0);
    fIsField = isField;
    fRobustRefit = kTRUE;
    fGlobalTracksBranchName = "BmnGlobalTrack";
    fVertexBranchName = "BmnVertex";
    fKalman = new BmnKalmanFilter();
}

BmnVertexFinder::~BmnVertexFinder() {
    delete fKalman;
}

InitStatus BmnVertexFinder::Init() {
    if (fVerbose > 1) cout << "=========================== Vertex finder init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    fGlobalTracksArray = (TClonesArray*)ioman->GetObject(fGlobalTracksBranchName);  //in
    if (!fGlobalTracksArray) {
        cout << "BmnVertexFinder::Init(): branch " << fGlobalTracksBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fVertexArray = new TClonesArray("CbmVertex", 1);  //out
    ioman->Register(fVertexBranchName, "GEM", fVertexArray, kTRUE);

    if (fVerbose > 1) cout << "=========================== Vertex finder init finished ===================" << endl;

    return kSUCCESS;
}

void BmnVertexFinder::Exec(Option_t* opt) {
    if (!IsActive())
        return;
    clock_t tStart = clock();

    if (fVerbose > 1) cout << "======================== Vertex finder exec started  ======================" << endl;
    if (fVerbose > 1) cout << "Event number: " << fEventNo++ << endl;

    fVertexArray->Delete();

    fNTracks = fGlobalTracksArray->GetEntriesFast();

    if (fNTracks > 1) {
        FindVertexByVirtualPlanes();
        CbmVertex* vert = (CbmVertex*)fVertexArray->At(0);
        if (fVerbose > 0) cout << "BmnVertexFinder: (" << vert->GetX() << ", " << vert->GetY() << ", " << vert->GetZ() << ")" << endl;
    } else {
        new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) CbmVertex("vertex", "vertex", -1000., -1000., -1000., 0.0, 0, -1, TMatrixFSym(3), fRoughVertex3D);
        if (fVerbose > 0) cout << "BmnVertexFinder: Vertex NOT found" << endl;
    }

    if (fVerbose > 1) cout << "\n======================== Vertex finder exec finished ======================" << endl;

    clock_t tFinish = clock();
    workTime += ((Float_t)(tFinish - tStart)) / CLOCKS_PER_SEC;
}

Float_t BmnVertexFinder::FindVZByVirtualPlanes(Float_t z_0, Float_t range) {
    const Int_t nPlanes = 5;
    Float_t minZ = z_0;

    while (range >= 0.01) {
        Float_t zMax = minZ + range;
        Float_t zMin = minZ - range;
        Float_t zStep = (zMax - zMin) / nPlanes;

        vector<Double_t> xHits[nPlanes];
        vector<Double_t> yHits[nPlanes];
        Float_t zPlane[nPlanes];
        Float_t rRMS[nPlanes] = {0};

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane)
            zPlane[iPlane] = zMax - iPlane * zStep;

        for (Int_t iTr = 0; iTr < fNTracks; ++iTr) {
            BmnGlobalTrack* track = (BmnGlobalTrack*)fGlobalTracksArray->At(iTr);
            if (track->GetFlag() == 13) continue;
            FairTrackParam par0 = *(track->GetParamFirst());
            for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
                fKalman->TGeoTrackPropagate(&par0, zPlane[iPlane], (par0.GetQp() > 0.) ? 2212 : -211, NULL, NULL, fIsField);
                xHits[iPlane].push_back(par0.GetX());
                yHits[iPlane].push_back(par0.GetY());
            }
        }

        Double_t minRMS = DBL_MAX;
        //Int_t minZ = -1000;

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            rRMS[iPlane] = CalcMeanDist(xHits[iPlane], yHits[iPlane]);
            //rRMS[iPlane] = CalcRms2D(xHits[iPlane], yHits[iPlane]);
            if (rRMS[iPlane] < minRMS) {
                minRMS = rRMS[iPlane];
                minZ = zPlane[iPlane];
            }
        }
        range /= 2;

        // for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane)
        //     //rRMS[iPlane] = CalcRms2D(xHits[iPlane], yHits[iPlane]);
        // rRMS[iPlane] = CalcMeanDist(xHits[iPlane], yHits[iPlane]);

        // //TCanvas* c = new TCanvas("c1", "c1", 1200, 800);
        // //c->Divide(1, 1);
        // //c->cd(1);
        // TGraph* vertex = new TGraph(nPlanes, zPlane, rRMS);
        // TFitResultPtr ptr = vertex->Fit("pol2", "QFS");
        // //        TF1 *fit_func = vertex->GetFunction("pol2");
        // Float_t b = ptr->Parameter(1);
        // Float_t a = ptr->Parameter(2);
        // minZ = -b / (2 * a);
        // range /= 2;
        // //         vertex->Draw("AP*");
        // // c->SaveAs("tmp.pdf");
        // // getchar();
        // delete vertex;
    }
    return minZ;
}

void BmnVertexFinder::FindVertexByVirtualPlanes() {
    Bool_t doAgain = kFALSE;
    const Double_t kRmsCut = 1.0;  //cm
    Double_t rRMS = 0.0;
    Double_t vx = 0.0;
    Double_t vy = 0.0;
    Double_t vz = 0.0;
    Int_t nOk = 0;
    Int_t iter = 0;
    
    Double_t range = 50.0;

    do {
       // cout << "\n ITER: " << iter << endl;
        doAgain = kFALSE;
        vz = FindVZByVirtualPlanes(fRoughVertex3D.Z(), range);
        vector<Double_t> xHits;
        vector<Double_t> yHits;
        vector<Int_t> indexes;
        rRMS = 0.0;

        for (Int_t iTr = 0; iTr < fNTracks; ++iTr) {
            BmnGlobalTrack* track = (BmnGlobalTrack*)fGlobalTracksArray->At(iTr);
            if (track->GetFlag() == 13) continue;
            FairTrackParam par0 = *(track->GetParamFirst());
            fKalman->TGeoTrackPropagate(&par0, vz, (par0.GetQp() > 0.) ? 2212 : -211, NULL, NULL, fIsField);
            xHits.push_back(par0.GetX());
            yHits.push_back(par0.GetY());
            indexes.push_back(iTr);
        }

        vx = Mean(xHits.begin(), xHits.end());
        vy = Mean(yHits.begin(), yHits.end());
        rRMS = CalcRms2D(xHits, yHits);
        nOk = indexes.size();
        //cout << "V(X, Y, Z) = (" << vx << ", " << vy << ", " << vz << ")" << endl;
        //cout << "nTracks = " << nOk << endl;
        //cout << "rRMS = " << rRMS << endl;

        if (nOk > 2)        
        if (rRMS > kRmsCut) {
            Double_t rMax = 0;
            Int_t idxMax = -1;
            for (Int_t iHit = 0; iHit < xHits.size(); ++iHit) {
                Double_t rHit = Sqrt(Sq(xHits[iHit]) + Sq(yHits[iHit]));
                if (rHit > rMax) {
                    rMax = rHit;
                    idxMax = indexes[iHit];
                }
            }

            if (idxMax != -1) {
                BmnGlobalTrack* track = (BmnGlobalTrack*)fGlobalTracksArray->At(idxMax);
                track->SetFlag(13);
                doAgain = kTRUE;
            }
        }
        iter++;
    } while (doAgain);

    Double_t VX = (Abs(vz - fRoughVertex3D.Z()) < range) ? vx : -1000.;
    Double_t VY = (Abs(vz - fRoughVertex3D.Z()) < range) ? vy : -1000.;
    Double_t VZ = (Abs(vz - fRoughVertex3D.Z()) < range) ? vz : -1000.;

    new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) CbmVertex("vertex", "vertex", VX, VY, VZ, rRMS, 0, nOk, TMatrixFSym(3), fRoughVertex3D);
}

void BmnVertexFinder::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt", ofstream::app);
    outFile << "Vertex Finder Time: " << workTime;
    if (fVerbose == 0) cout << "Work time of the GEM vertex finder: " << workTime << endl;
}

Double_t BmnVertexFinder::CalcRms2D(vector<Double_t> x, vector<Double_t> y) {
    Double_t xMean = 0.0;
    Double_t yMean = 0.0;
    Double_t rms = 0.0;
    for (Int_t iHit = 0; iHit < x.size(); ++iHit) {
        xMean += x.at(iHit);
        yMean += y.at(iHit);
    }
    xMean /= x.size();
    yMean /= y.size();

    for (Int_t iHit = 0; iHit < x.size(); ++iHit) {
        rms += (Sq(x.at(iHit) - xMean) + Sq(y.at(iHit) - yMean));
    }

    rms /= x.size();
    return Sqrt(rms);
}

Double_t BmnVertexFinder::CalcMeanDist(vector<Double_t> x, vector<Double_t> y) {
    
    Double_t sumDist = 0.0;
    Int_t nPairs = 0;
    for (Int_t i = 0; i < x.size(); ++i) {
        for (Int_t j = i + 1; j < x.size(); ++j) {
            sumDist += Sqrt(Sq(x[i] - x[j]) + Sq(y[i] - y[j]));
            nPairs++;
        }
    }
    return sumDist / nPairs; // calc. ave. dist value

}
