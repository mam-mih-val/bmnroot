#include "BmnVertexFinder.h"
#include "BmnMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "vector"

static Float_t workTime = 0.0;

using namespace std;
using namespace TMath;

BmnVertexFinder::BmnVertexFinder(Int_t period, Bool_t isField) {
    fPeriodId = period;
    fEventNo = 0;
    fGlobalTracksArray = NULL;
    fKalman = NULL;
    fNTracks = 0;
    fRoughVertex3D = (fPeriodId == 7) ? TVector3(0.5, -4.6, -2.3) : (fPeriodId == 6) ? TVector3(0.0, -3.5, -21.9) : TVector3(0.0, 0.0, 0.0);
    fIsField = isField;
    fRobustRefit = kTRUE;
    //fField = NULL;
    fGlobalTracksBranchName = "BmnGlobalTrack";
    fVertexBranchName = "BmnVertex";
}

BmnVertexFinder::~BmnVertexFinder() {
}

InitStatus BmnVertexFinder::Init() {

    if (fVerbose) cout << "=========================== Vertex finder init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    fGlobalTracksArray = (TClonesArray*) ioman->GetObject(fGlobalTracksBranchName); //in
    if (!fGlobalTracksArray) {
        cout << "BmnVertexFinder::Init(): branch " << fGlobalTracksBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fVertexArray = new TClonesArray("CbmVertex", 1); //out
    ioman->Register(fVertexBranchName, "GEM", fVertexArray, kTRUE);

    //fField = FairRunAna::Instance()->GetField();
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    TString confGem = (fPeriodId == 7) ? "GemRunSpring2018.xml" : (fPeriodId == 6) ? "GemRunSpring2017.xml" : "GemRunSpring2017.xml";
    if (confGem == "") {
        printf(ANSI_COLOR_RED "No GEM geometry defined!\n" ANSI_COLOR_RESET);
        throw;
    }
    fDetector = new BmnGemStripStationSet(gPathGemConfig + confGem);

    if (fVerbose) cout << "=========================== Vertex finder init finished ===================" << endl;

    return kSUCCESS;
}

void BmnVertexFinder::Exec(Option_t* opt) {
    if (!IsActive())
        return;
    clock_t tStart = clock();

    if (fVerbose) cout << "======================== Vertex finder exec started  ======================" << endl;
    if (fVerbose) cout << "Event number: " << fEventNo++ << endl;

    fVertexArray->Delete();

    fNTracks = fGlobalTracksArray->GetEntriesFast();

    if (fNTracks > 1)
        FindVertexByVirtualPlanes();
        // To prevent crash caused by uninitialized TClonesArray due to unsufficient number of tracks when reading event by event in user's code
    else
        new((*fVertexArray)[fVertexArray->GetEntriesFast()]) CbmVertex("vertex", "vertex", -1000., -1000., -1000., 0.0, 0, fNTracks, TMatrixFSym(3), fRoughVertex3D);

    if (fVerbose) cout << "\n======================== Vertex finder exec finished ======================" << endl;

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

Float_t BmnVertexFinder::FindVZByVirtualPlanes(Float_t z_0, Float_t range) {

    fKalman = new BmnKalmanFilter();

    const Int_t nPlanes = 5;
    Float_t minZ = fRoughVertex3D.Z();

    while (range >= 0.1) {
        Float_t zMax = z_0 + range;
        Float_t zMin = z_0 - range;
        Float_t zStep = (zMax - zMin) / nPlanes;

        vector <TVector3> PlaneHits[nPlanes];
        Float_t zPlane[nPlanes];
        Float_t Dist[nPlanes];

        vector <Double_t> distancesEachPlane[nPlanes];
        vector <Double_t> aveDist[nPlanes];
        vector <Double_t> w[nPlanes];
        Double_t mu[nPlanes];
        Double_t sigma[nPlanes];

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            zPlane[iPlane] = zMax - iPlane * zStep;
            Dist[iPlane] = 0.0;
            sigma[nPlanes] = 0.;
            mu[nPlanes] = 0.;
        }

        for (Int_t iTr = 0; iTr < fNTracks; ++iTr) {
            BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracksArray->At(iTr);
            //            if (track->GetFlag() == 666)
            //                continue;
            FairTrackParam par0 = *(track->GetParamFirst());

            for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
                fKalman->TGeoTrackPropagate(&par0, zPlane[iPlane], (par0.GetQp() > 0.) ? 2212 : 211, NULL, NULL, fIsField);
                PlaneHits[iPlane].push_back(TVector3(par0.GetX(), par0.GetY(), par0.GetZ()));
            }
        }

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            Int_t nPairs = 0;
            for (Int_t i = 0; i < fNTracks; ++i) {
                TVector3 vI = PlaneHits[iPlane].at(i);
                for (Int_t j = i + 1; j < fNTracks; ++j) {
                    TVector3 vJ = PlaneHits[iPlane].at(j);
                    Double_t d = Sqrt(Sqr(vI.X() - vJ.X()) + Sqr(vI.Y() - vJ.Y()));
                    distancesEachPlane[iPlane].push_back(d);
                    Dist[iPlane] += d;
                    nPairs++;
                }
            }
            mu[iPlane] = Dist[iPlane] / nPairs; // calc. ave. dist value
            // cout << iPlane << " " << mu[iPlane] << endl;
            /*
                        cout << "Mu_before = " << endl;
                        cout << iPlane << " " << mu[iPlane] << endl;


                        //for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
                        aveDist[iPlane] = dist(distancesEachPlane[iPlane], mu[iPlane]); // calc. distance from averaged value
                        //            cout << iPlane << " " << distancesEachPlane[iPlane].size() << endl;
                        sigma[iPlane] = 0.;
                        //            cout << "iPlane = " << iPlane << " distancesEachPlane[iPlane].size() = " << distancesEachPlane[iPlane].size() << endl;
                        //if (distancesEachPlane[iPlane].size() > 1)
                        //            for (Int_t iSize = 0; iSize < distancesEachPlane[iPlane].size(); iPlane++) {
                        if (distancesEachPlane[iPlane].size() > 1)
                            for (auto it : distancesEachPlane[iPlane]) {
                                //       cout << "iPlane = " << iPlane << " mu[iPlane] = " << mu[iPlane] << " iSize = " << iSize << " distancesEachPlane[iPlane].at(iSize) = "
                                //               << distancesEachPlane[iPlane].at(iSize) << " " << distancesEachPlane[iPlane].size() << endl;


                                sigma[iPlane] += Sqr(it - mu[iPlane]);
                                // if (distancesEachPlane[iPlane].size() != 1) cout << sigma[iPlane] << endl;
                            }
                        sigma[iPlane] = Sqrt(sigma[iPlane] / distancesEachPlane[iPlane].size()); // calc. initial approx. for sigma

                        w[iPlane] = W(aveDist[iPlane], sigma[iPlane]); // calc. init. weights
                        sigma[iPlane] = Sigma(aveDist[iPlane], w[iPlane]); // calc reweighted sigma

                        // Robust refit with use of Tukey weights calculation algorithm
                        if (fRobustRefit) {
                            const Int_t kNIter = 10; // FIXME
                            for (Int_t iIter = 1; iIter < kNIter; iIter++) {
                                // cout << iIter << endl;
                                mu[iPlane] = Mu(distancesEachPlane[iPlane], w[iPlane]);
                                aveDist[iPlane] = dist(distancesEachPlane[iPlane], mu[iPlane]);
                                w[iPlane] = W(aveDist[iPlane], sigma[iPlane]);
                                sigma[iPlane] = Sigma(aveDist[iPlane], w[iPlane]);
                            }
                        }
                        cout << "Mu_after = " << endl;
                        cout << iPlane << " " << mu[iPlane] << endl;
                        getchar();
             */
        }

        Float_t minDist = FLT_MAX;
        minZ = FLT_MAX;
        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            if (mu[iPlane] < minDist) {
                minZ = zPlane[iPlane];
                minDist = mu[iPlane];
            }
        }


        //TCanvas* c = new TCanvas("c1", "c1", 1200, 800);
        //c->Divide(1, 1);
        //c->cd(1);
        TGraph* vertex = new TGraph(nPlanes, zPlane, Dist);
        vertex->Fit("pol2", "QF");
        TF1 *fit_func = vertex->GetFunction("pol2");
        Float_t b = fit_func->GetParameter(1);
        Float_t a = fit_func->GetParameter(2);
        z_0 = -b / (2 * a);
        range /= 2;
//        vertex->Draw("AP*");
        //c->SaveAs("tmp.pdf");
        // getchar();
        delete vertex;
    }
    return minZ;
}

//void BmnVertexFinder::FindVertexByVirtualPlanes() {
//    Bool_t doIterations = kTRUE;
//
//    Double_t VX = -1000.;
//    Double_t VY = -1000.;
//    Double_t VZ = -1000.;
//
//    Int_t nBad = 0;
//    Int_t itCounter = 0;
//
//    while (doIterations) {
//        itCounter++;
//        Float_t vz = FindVZByVirtualPlanes(fRoughVertex3D.Z(), 50.0);
//        Double_t range = 50.0;
//        if (Abs(vz - fRoughVertex3D.Z()) > range)
//            break;
//        Float_t vx = 0.0;
//        Float_t vy = 0.0;
//        UInt_t nOk = 0;
//
//        for (Int_t iTr = 0; iTr < fNTracks; ++iTr) {
//            BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracksArray->UncheckedAt(iTr);
//            if (track->GetFlag() == 666)
//                continue;
//            FairTrackParam par0 = *(track->GetParamFirst());
//
//            fKalman->TGeoTrackPropagate(&par0, vz, (par0.GetQp() > 0.) ? 2212 : 211, NULL, NULL, fIsField);
//            vx += par0.GetX();
//            vy += par0.GetY();
//            track->SetB(Sqrt(par0.GetX() * par0.GetX() + par0.GetY() * par0.GetY())); //impact parameter
//
//            nOk++;
//        }
//
//        vx /= nOk;
//        vy /= nOk;
//
//        for (Int_t iTr = 0; iTr < fNTracks; ++iTr) {
//            BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracksArray->At(iTr);
//            if (track->GetFlag() == 666)
//                continue;
//            FairTrackParam par0 = *(track->GetParamFirst());
//
//            fKalman->TGeoTrackPropagate(&par0, vz, (par0.GetQp() > 0.) ? 2212 : 211, NULL, NULL, fIsField);
//            cout << Abs(vx - par0.GetX()) << " " << Abs(vy - par0.GetY()) << endl;
//            cout << vz << endl;
//            if (Abs(vx - par0.GetX()) > 5. || Abs(vy - par0.GetY()) > 5.) {
//                track->SetFlag(666);
//                nBad++;
//            } else
//                doIterations = kFALSE;
//        }
//        //
//
//        // 2.5 - the best range from MC simulations and reconstruction (by A.Zelenoff)
//        VX = (Abs(vz - fRoughVertex3D.Z()) < range) ? vx : -1000.;
//        VY = (Abs(vz - fRoughVertex3D.Z()) < range) ? vy : -1000.;
//        VZ = (Abs(vz - fRoughVertex3D.Z()) < range) ? vz : -1000.;
//
//        if (doIterations) {
//            cout << itCounter << endl;
//            cout << doIterations << " " << nBad << " " << fNTracks << endl;
//        }
//        delete fKalman;
//    }
//    new((*fVertexArray)[fVertexArray->GetEntriesFast()]) CbmVertex("vertex", "vertex", VX, VY, VZ, 0.0, 0, fNTracks, TMatrixFSym(3), fRoughVertex3D);
//}

void BmnVertexFinder::FindVertexByVirtualPlanes() {

    Float_t vz = FindVZByVirtualPlanes(fRoughVertex3D.Z(), 50.0);
    Float_t vx = 0.0;
    Float_t vy = 0.0;
    UInt_t nOk = 0;

    for (Int_t iTr = 0; iTr < fNTracks; ++iTr) {
        BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracksArray->At(iTr);
        //        if (track->GetFlag() == kBMNBAD) continue;
        FairTrackParam par0 = *(track->GetParamFirst());

        fKalman->TGeoTrackPropagate(&par0, vz, 211, NULL, NULL, fIsField);
        vx += par0.GetX();
        vy += par0.GetY();
        track->SetB(Sqrt(par0.GetX() * par0.GetX() + par0.GetY() * par0.GetY())); //impact parameter
        nOk++;
    }

    vx /= nOk;
    vy /= nOk;

    Double_t range = 50.0;
    // 2.5 - the best range from MC simulations and reconstruction (by A.Zelenoff)
    Double_t VX = (Abs(vz - fRoughVertex3D.Z()) < range) ? vx : -1000.;
    Double_t VY = (Abs(vz - fRoughVertex3D.Z()) < range) ? vy : -1000.;
    Double_t VZ = (Abs(vz - fRoughVertex3D.Z()) < range) ? vz : -1000.;

    new((*fVertexArray)[fVertexArray->GetEntriesFast()]) CbmVertex("vertex", "vertex", VX, VY, VZ, 0.0, 0, fNTracks, TMatrixFSym(3), fRoughVertex3D);

    delete fKalman;
}

void BmnVertexFinder::Finish() {
    ofstream outFile;
    outFile.open("QA/timing.txt", ofstream::app);
    outFile << "Vertex Finder Time: " << workTime;
    cout << "Work time of the GEM vertex finder: " << workTime << endl;
}

