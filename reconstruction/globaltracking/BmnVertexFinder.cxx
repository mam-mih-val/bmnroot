#include "BmnVertexFinder.h"

#include "BmnMath.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "vector"

using namespace std;
using namespace TMath;

BmnVertexFinder::BmnVertexFinder(Int_t period) {
    fPeriodId = period;
    fEventNo = 0;
    fGlobalTracksArray = NULL;
    fRobustRefit = kTRUE;
    fGlobalTracksBranchName = "BmnGlobalTrack";
    fVertexBranchName = "BmnVertex";
}

BmnVertexFinder::~BmnVertexFinder() {
    delete fKalman;
}

InitStatus BmnVertexFinder::Init() {
    if (fVerbose > 1) cout << "=========================== Vertex finder init started ====================" << endl;

    fKalman = new BmnKalmanFilter();

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    fGlobalTracksArray = (TClonesArray*)ioman->GetObject(fGlobalTracksBranchName);  //in
    if (!fGlobalTracksArray) {
        cout << "BmnVertexFinder::Init(): branch " << fGlobalTracksBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fVertexArray = new TClonesArray("BmnVertex", 1);  //out
    ioman->Register(fVertexBranchName, "GEM", fVertexArray, kTRUE);

    if (fVerbose > 1) cout << "=========================== Vertex finder init finished ===================" << endl;

    return kSUCCESS;
}

void BmnVertexFinder::Exec(Option_t* opt) {
    TStopwatch sw;
    sw.Start();

    if (!IsActive())
        return;

    if (fVerbose > 1) cout << "======================== Vertex finder exec started  ======================" << endl;
    if (fVerbose > 1) cout << "Event number: " << fEventNo++ << endl;

    fVertexArray->Delete();

    new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) BmnVertex(FindPrimaryVertex(fGlobalTracksArray));
    new ((*fVertexArray)[fVertexArray->GetEntriesFast()]) BmnVertex(FindSecondaryVertex(fGlobalTracksArray));

    if (fVerbose > 1) cout << "\n======================== Vertex finder exec finished ======================" << endl;

    sw.Stop();
    fTime += sw.RealTime();
}

BmnVertex BmnVertexFinder::FindPrimaryVertex(TClonesArray* tracks) {
    //cout << "Primary" << endl;
    const Double_t kRange = 50.0;
    const Double_t xVertMin = -0.5;
    const Double_t xVertMax = +2.0;
    const Double_t yVertMin = -5.0;
    const Double_t yVertMax = -2.5;
    Double_t roughVertexZ = -1.0;
    Int_t nPrim = 0;

    for (Int_t iTr = 0; iTr < tracks->GetEntriesFast(); ++iTr) {
        BmnGlobalTrack* track = (BmnGlobalTrack*)tracks->At(iTr);
        FairTrackParam par0 = *(track->GetParamFirst());
        if (fKalman->TGeoTrackPropagate(&par0, roughVertexZ, (par0.GetQp() > 0.) ? 2212 : -211, NULL, NULL) == kBMNERROR) {
            continue;
        }

        if (par0.GetX() > xVertMin && par0.GetX() < xVertMax && par0.GetY() > yVertMin && par0.GetY() < yVertMax) {
            track->SetFlag(0);  //primary track
            nPrim++;
        } else {
            track->SetFlag(1);  //secondary track
        }
    }

    if (nPrim < 2) {
        return BmnVertex();
    }

    Double_t vz = FindVZByVirtualPlanes(roughVertexZ, kRange, tracks, 0);
    if (Abs(vz - roughVertexZ) > kRange) {
        for (Int_t iTr = 0; iTr < tracks->GetEntriesFast(); ++iTr) {
            BmnGlobalTrack* track = (BmnGlobalTrack*)tracks->At(iTr);
            track->SetFlag(1);
        }
        return BmnVertex();
    }

    vector<Double_t> xHits;
    vector<Double_t> yHits;
    vector<Int_t> indexes;
    for (Int_t iTr = 0; iTr < tracks->GetEntriesFast(); ++iTr) {
        BmnGlobalTrack* track = (BmnGlobalTrack*)tracks->At(iTr);
        if (track->GetFlag() != 0) continue;
        FairTrackParam par0 = *(track->GetParamFirst());
        if (fKalman->TGeoTrackPropagate(&par0, vz, (par0.GetQp() > 0.) ? 2212 : -211, NULL, NULL) == kBMNERROR) {
            continue;
        }
        xHits.push_back(par0.GetX());
        yHits.push_back(par0.GetY());
        indexes.push_back(iTr);
    }

    Double_t vx = Mean(xHits.begin(), xHits.end());
    Double_t vy = Mean(yHits.begin(), yHits.end());

    Double_t rRMS = CalcRms2D(xHits, yHits);

    return BmnVertex(vx, vy, vz, rRMS, 0, xHits.size(), TMatrixFSym(3), -1, vector<Int_t>());
}

BmnVertex BmnVertexFinder::FindSecondaryVertex(TClonesArray* tracks) {
    //cout << "Sacondary" << endl;
    Double_t roughVertexZ = (fPeriodId == 7) ? -1.0 : (fPeriodId == 6) ? -21.9 : 0.0;
    const Double_t kRange = 300.0;

    Int_t nSec = 0;

    for (Int_t iTr = 0; iTr < tracks->GetEntriesFast(); ++iTr) {
        BmnGlobalTrack* track = (BmnGlobalTrack*)tracks->At(iTr);
        if (track->GetFlag() == 1) nSec++;  //secondary track
    }

    if (nSec < 2) {
        return BmnVertex();
    }

    Double_t vz = FindVZByVirtualPlanes(roughVertexZ, kRange, tracks, 1);

    vector<Double_t> xHits;
    vector<Double_t> yHits;
    vector<Int_t> indexes;
    for (Int_t iTr = 0; iTr < tracks->GetEntriesFast(); ++iTr) {
        BmnGlobalTrack* track = (BmnGlobalTrack*)tracks->At(iTr);
        if (track->GetFlag() != 1) continue;
        FairTrackParam par0 = *(track->GetParamFirst());
        if (fKalman->TGeoTrackPropagate(&par0, vz, (par0.GetQp() > 0.) ? 2212 : -211, NULL, NULL) == kBMNERROR) {
            continue;
        }
        xHits.push_back(par0.GetX());
        yHits.push_back(par0.GetY());
        indexes.push_back(iTr);
    }

    Double_t vx = Mean(xHits.begin(), xHits.end());
    Double_t vy = Mean(yHits.begin(), yHits.end());

    Double_t rRMS = CalcRms2D(xHits, yHits);

    return BmnVertex(vx, vy, vz, rRMS, 0, xHits.size(), TMatrixFSym(3), -1, vector<Int_t>());
}

Float_t BmnVertexFinder::FindVZByVirtualPlanes(Float_t z_0, Float_t range, TClonesArray* tracks, Int_t flag) {
    const Int_t nPlanes = 3;
    Float_t minZ = z_0;

    while (range >= 0.01) {
        Float_t zMax = minZ + range;
        Float_t zMin = minZ - range;
        Float_t zStep = (zMax - zMin) / (nPlanes - 1);

        vector<Double_t> xHits[nPlanes];
        vector<Double_t> yHits[nPlanes];
        Float_t zPlane[nPlanes];
        Float_t rRMS[nPlanes] = {0};

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane)
            zPlane[iPlane] = zMax - iPlane * zStep;

        Int_t nOkTr = 0;

        for (Int_t iTr = 0; iTr < tracks->GetEntriesFast(); ++iTr) {
            BmnGlobalTrack* track = (BmnGlobalTrack*)tracks->At(iTr);
            if (track->GetFlag() != flag) continue;
            FairTrackParam par0 = *(track->GetParamFirst());
            //cout << "tr" << iTr << endl;
            Double_t xTr[nPlanes];
            Double_t yTr[nPlanes];
            Bool_t trOk = kTRUE;
            for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
                if (fKalman->TGeoTrackPropagate(&par0, zPlane[iPlane], (par0.GetQp() > 0.) ? 2212 : -211, NULL, NULL) == kBMNERROR) {
                    track->SetFlag(-1);  //maybe 666???
                    trOk = kFALSE;
                    break;
                }

                //cout << " " << par0.GetX() << " " << par0.GetY() << " " << par0.GetZ() << endl;
                xTr[iPlane] = par0.GetX();
                yTr[iPlane] = par0.GetY();
                //xHits[iPlane].push_back(par0.GetX());
                //yHits[iPlane].push_back(par0.GetY());
            }

            if (trOk) {
                nOkTr++;
                for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
                    xHits[iPlane].push_back(xTr[iPlane]);
                    yHits[iPlane].push_back(yTr[iPlane]);
                }
            }
        }

        if (nOkTr < 2) return -1000.0;

        Double_t minRMS = DBL_MAX;

        //Calculation minZ as minimal value
        // for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
        //     rRMS[iPlane] = CalcMeanDist(xHits[iPlane], yHits[iPlane]);
        //     if (rRMS[iPlane] < minRMS) {
        //         minRMS = rRMS[iPlane];
        //         minZ = zPlane[iPlane];
        //     }
        // }

        //Calculation minZ as minimum of parabola
        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            rRMS[iPlane] = CalcMeanDist(xHits[iPlane], yHits[iPlane]);
        }
        TGraph* vertex = new TGraph(nPlanes, zPlane, rRMS);
        TFitResultPtr ptr = vertex->Fit("pol2", "QFS");
        Float_t b = ptr->Parameter(1);
        Float_t a = ptr->Parameter(2);
        minZ = -b / (2 * a);
        delete vertex;

        range /= 2;

        // for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane)
        //     //rRMS[iPlane] = CalcRms2D(xHits[iPlane], yHits[iPlane]);
        // rRMS[iPlane] = CalcMeanDist(xHits[iPlane], yHits[iPlane]);

        //TCanvas* c = new TCanvas("c1", "c1", 1200, 800);
        // //c->Divide(1, 1);
        // //c->cd(1);
        //TGraph* vertex = new TGraph(nPlanes, zPlane, rRMS);
        // TFitResultPtr ptr = vertex->Fit("pol2", "QFS");
        // //        TF1 *fit_func = vertex->GetFunction("pol2");
        // Float_t b = ptr->Parameter(1);
        // Float_t a = ptr->Parameter(2);
        // minZ = -b / (2 * a);
        // range /= 2;
        //vertex->Draw("AP*");
        //c->SaveAs("tmp.pdf");
        //getchar();
        //delete vertex;
    }
    return minZ;
}

void BmnVertexFinder::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt", ofstream::app);
    outFile << "Vertex Finder Time: " << fTime;
    if (fVerbose == 0) cout << "Work time of the GEM vertex finder: " << fTime << endl;
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
    return sumDist / nPairs;  // calc. ave. dist value
}

ClassImp(BmnVertexFinder);
