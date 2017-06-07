// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-04-14

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnLambdaAnalysis                                                           //
//                                                                            //
//  A supplementary class for Lambda-reconstruction                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <TCanvas.h>
#include <TGeoManager.h>
#include <Fit/FitResult.h>
#include "BmnLambdaAnalysis.h"

BmnLambdaAnalysis::BmnLambdaAnalysis(BmnGemStripConfiguration::GEM_CONFIG config) :
fPdgLambda(3122),
fPdgProton(2212),
fPdgPionMinus(-211),
fEventCounter(0),
fMomProtMin(-DBL_MAX),
fMomProtMax(DBL_MAX),
fMomPionMin(-DBL_MAX),
fMomPionMax(DBL_MAX),
fTxProtMax(DBL_MAX),
fTyProtMax(DBL_MAX),
fTxProtMin(-DBL_MAX),
fTyProtMin(-DBL_MAX),
fTxPionMax(DBL_MAX),
fTyPionMax(DBL_MAX),
fTxPionMin(-DBL_MAX),
fTyPionMin(-DBL_MAX),
fYProtMax(DBL_MAX),
fYProtMin(-DBL_MAX),
fYPionMax(DBL_MAX),
fYPionMin(-DBL_MAX),
fGeometry(config),
fDebugCalculations(kFALSE),
fCutsDistrOnly(kFALSE),
fKalman(NULL),
fField(NULL) {
    for (Int_t i = 0; i < 2; i++) {
        Double_t val = (i == 0) ? 0. : DBL_MAX;
        fVpVpProton[i] = val;
        fVpVpPion[i] = val;
        fV0ProtonPion[i] = val;
        fV0VpDiff[i] = val;
    }
    
    for (Int_t i = 0; i < 6; i++)
        fInputUncertainties[i] = 0.;

    fBranchGemHits = "BmnGemStripHit";
    fBranchVertex = "BmnVertex";

    // Create GEM detector ------------------------------------------------------
    switch (fGeometry) {
        case BmnGemStripConfiguration::RunWinter2016:
            fDetector = new BmnGemStripStationSet_RunWinter2016(fGeometry);
            cout << "   Current Configuration : RunWinter2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2017:
            fDetector = new BmnGemStripStationSet_RunSpring2017(fGeometry);
            cout << "   Current Configuration : RunSpring2017" << "\n";
            break;

        default:
            fDetector = NULL;
    }
}

BmnLambdaAnalysis::~BmnLambdaAnalysis() {
    delete fDetector;
}

void BmnLambdaAnalysis::RecoAnalysis() {
    TLorentzVector lPos, lNeg;
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack* track1 = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
        Double_t Tx1 = track1->GetParamFirst()->GetTx();
        Double_t Ty1 = track1->GetParamFirst()->GetTy();
        Double_t p1 = 1. / track1->GetParamFirst()->GetQp();
        Double_t mProt2 = fPDG->GetParticle(fPdgProton)->Mass() * fPDG->GetParticle(fPdgProton)->Mass();
        Double_t E1 = Sqrt(p1 * p1 + mProt2);
        Double_t Pz1 = Abs(p1) * Sqrt(1 - Tx1 * Tx1 - Ty1 * Ty1);
        Double_t Y1 = 0.5 * Log((E1 + Pz1) / (E1 - Pz1));

        // Select protons here. p1 < 0 means a particle not to be a proton
        if (p1 < 0. || Abs(p1) < fMomProtMin || Abs(p1) > fMomProtMax ||
                Tx1 < fTxProtMin || Tx1 > fTxProtMax ||
                Ty1 < fTyProtMin || Ty1 > fTyProtMax ||
                Y1 < fYProtMin || Y1 > fYProtMax)
            continue;

        for (Int_t jTrack = 0; jTrack < fGemTracks->GetEntriesFast(); jTrack++) {
            if (iTrack == jTrack)
                continue;

            BmnGemTrack* track2 = (BmnGemTrack*) fGemTracks->UncheckedAt(jTrack);
            Double_t Tx2 = track2->GetParamFirst()->GetTx();
            Double_t Ty2 = track2->GetParamFirst()->GetTy();
            Double_t p2 = 1. / track2->GetParamFirst()->GetQp();
            Double_t mPion2 = fPDG->GetParticle(fPdgPionMinus)->Mass() * fPDG->GetParticle(fPdgPionMinus)->Mass();
            Double_t E2 = Sqrt(p2 * p2 + mPion2);
            Double_t Pz2 = Abs(p2) * Sqrt(1 - Tx2 * Tx2 - Ty2 * Ty2);
            Double_t Y2 = 0.5 * Log((E2 + Pz2) / (E2 - Pz2));

            // Select protons here. p2 < 0 means a particle not to be a pion
            if (p2 > 0. || Abs(p2) < fMomPionMin || Abs(p2) > fMomPionMax ||
                    Tx2 < fTxPionMin || Tx2 > fTxPionMax ||
                    Ty2 < fTyPionMin || Ty2 > fTyPionMax ||
                    Y2 < fYPionMin || Y2 > fYPionMax)
                continue;

            // Calculate zV and yV using the YZ-trajectory ...
            // Probably, it has to be parametrized by a straight line
            TVector2 yzVertex = SecondaryVertexY(track1->GetParamFirst(), track2->GetParamFirst());
            Double_t zVY = yzVertex.Y();
            // Double_t yV = yzVertex.X();
            fzVY->Fill(zVY);

            // Use Kalman to estimate a possible secondary vertex in xz-plane
            // Propagation to Z = 0; 
            // After the procedure <<track(1,2)->GetParamFirst()>> will be updated by shifted values corresponding to Z = 0 
            vector <TVector3> protonTrackPoints = KalmanTrackPropagation(track1, fPdgProton);
            vector <TVector3> pionTrackPoints = KalmanTrackPropagation(track2, fPdgPionMinus);

            // XZ-trajectory parametrized by pol2 (x(z) = az^2 + bz + c)
            // FitParabola(...) returns coeff. of the paramtr. used
            TVector3 protParametrizedTraject = FitParabola(protonTrackPoints);
            TVector3 pionParametrizedTraject = FitParabola(pionTrackPoints);

            // Omitting bad fits ...
            if ((protParametrizedTraject.Mag()) < FLT_EPSILON || pionParametrizedTraject.Mag() < FLT_EPSILON ||
                    protParametrizedTraject.X() * pionParametrizedTraject.X() > 0.)
                continue;

            if (fDebugCalculations) {
                cout << "a = " << protParametrizedTraject.X() << " b = " << protParametrizedTraject.Y() << " c = " << protParametrizedTraject.Z() << endl;
                cout << "a = " << pionParametrizedTraject.X() << " b = " << pionParametrizedTraject.Y() << " c = " << pionParametrizedTraject.Z() << endl;
            }

            // Array is used either to store point coord. and dist. between (z1[0], x1[1], z2[2], x2[3], min_dist[4]) them or 
            // points of overlapping (z1, x1, z2, x2, 0.) when the curves are overlapped.
            Double_t* pointsAndMinDist = new Double_t[5];
            CalculateMinDistance(protParametrizedTraject, pionParametrizedTraject, pointsAndMinDist);

            // New track params. at secondary vertex (V0) and Vp to be stored here
            FairTrackParam proton_V0, pion_V0, proton_Vp, pion_Vp;
            vector <Double_t> geomCuts; // 

            // Case of overlapping extrap.curves
            if (Abs(pointsAndMinDist[4]) < FLT_EPSILON) {
                Double_t minZ = Min(pointsAndMinDist[0], pointsAndMinDist[2]);
                if (minZ < 0. || minZ > 30.) {
                    delete [] pointsAndMinDist;
                    continue;
                }

                fzVX->Fill(minZ); // minZ is a possible candidate to be the second. vertex (V0)

                // Go to V0 ...
                proton_V0 = KalmanTrackPropagation(track1, fPdgProton, minZ);
                pion_V0 = KalmanTrackPropagation(track2, fPdgPionMinus, minZ);

                // Go to Vp
                Double_t Vpz = 0.; // FIXME (0. --> fEventVertex->GetZ())
                proton_Vp = KalmanTrackPropagation(track1, fPdgProton, Vpz);
                pion_Vp = KalmanTrackPropagation(track2, fPdgPionMinus, Vpz);

                geomCuts = GeometryCuts(proton_V0, pion_V0, proton_Vp, pion_Vp);

                delete [] pointsAndMinDist;
            }// Non overlapping curves ...
            else {
                delete [] pointsAndMinDist;
                continue;
            }

            // Apply geometry cuts from vector <<geomCuts>>
            // Distance beetween Vp and Vp_prot_extrap   [0]
            // Distance beetween Vp and Vp_prot_extrap   [1]
            // Distance between proton and pion at V0    [2]
            // Distance between V0 and Vp along beamline [3]
            if (geomCuts[0] < fVpVpProton[0] || geomCuts[0] > fVpVpProton[1] ||
                    geomCuts[1] < fVpVpPion[0] || geomCuts[1] > fVpVpPion[1] ||
                    geomCuts[2] < fV0ProtonPion[0] || geomCuts[2] > fV0ProtonPion[1] ||
                    geomCuts[3] < fV0VpDiff[0] || geomCuts[3] > fV0VpDiff[1])
                continue;


            // Track params. are redefined (correspond to zVX) ...
            Tx1 = proton_V0.GetTx();
            Ty1 = proton_V0.GetTy();
            Tx2 = pion_V0.GetTx();
            Ty2 = pion_V0.GetTy();

            Double_t A1 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
            lPos.SetXYZM(Tx1 * A1 * p1, Ty1 * A1 * p1, p1 * A1, fPDG->GetParticle(fPdgProton)->Mass());

            p2 *= -1.; // Since in the calculations pos. mom. values should be used

            Double_t A2 = 1. / Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
            lNeg.SetXYZM(Tx2 * A2 * p2, Ty2 * A2 * p2, p2 * A2, fPDG->GetParticle(fPdgPionMinus)->Mass());

            fLambdaInvMass->Fill(TLorentzVector((lPos + lNeg)).Mag());

            if (fDebugCalculations) {
                //dP and dT (dTx1, dTx2, dTy1, dTy2) (%)
                vector <Double_t> dP;
                vector <Double_t> dT;
                for (Int_t i = 0; i < 6; i++) {
                    if (i < 2)
                        dP.push_back(fInputUncertainties[i]);

                    else
                        dT.push_back(fInputUncertainties[i]);
                }

                vector <Double_t> res = DebugCalculations(track1, track2, dP, dT);
                cout << "Minv =  " << res[0] << " GeV; Err =  " << res[1] << " GeV" << endl;
            }
        }
    }
}

vector <Double_t> BmnLambdaAnalysis::GeometryCuts(FairTrackParam proton_V0, FairTrackParam pion_V0, FairTrackParam proton_Vp, FairTrackParam pion_Vp) {
    Double_t X = fEventVertex->GetX();
    Double_t Y = fEventVertex->GetY();
    Double_t Z = fEventVertex->GetZ();

    X = 0.;
    Y = 0.;
    Z = 0.; // FIXME !!!
    // Vp
    TVector3 Vp(X, Y, Z);

    // Secondary proton at V0
    TVector3 protonV0(proton_V0.GetX(), proton_V0.GetY(), proton_V0.GetZ());
    // Secondary pion at V0
    TVector3 pionV0(pion_V0.GetX(), pion_V0.GetY(), pion_V0.GetZ());

    // Secondary proton extrapolated to Vp (Vp_prot_extrap)
    TVector3 protonVp(proton_Vp.GetX(), proton_Vp.GetY(), proton_Vp.GetZ());
    // Secondary pion extrapolated to Vp (Vp_pion_extrap)
    TVector3 pionVp(pion_Vp.GetX(), pion_Vp.GetY(), pion_Vp.GetZ());

    // 1) 
    // Distance beetween Vp and Vp_prot_extrap
    Double_t protonVpVp = TVector3(protonVp - Vp).Mag();
    fProtonVpVp->Fill(protonVpVp);
    // 2)
    // Distance beetween Vp and Vp_prot_extrap
    Double_t pionVpVp = TVector3(pionVp - Vp).Mag();
    fPionVpVp->Fill(pionVpVp);

    // 3)
    // Distance between proton and pion at V0
    Double_t protonV0PionV0 = TVector3(protonV0 - pionV0).Mag();
    fProtonV0PionV0->Fill(protonV0PionV0);

    // 4)
    // Distance between V0 and Vp along beamline
    Double_t vertexDiff = proton_V0.GetZ() - Z;
    fVertexDiff->Fill(vertexDiff);

    vector <Double_t> cuts;
    cuts.push_back(protonVpVp);
    cuts.push_back(pionVpVp);
    cuts.push_back(protonV0PionV0);
    cuts.push_back(vertexDiff);

    return cuts;
}

vector <TVector3> BmnLambdaAnalysis::KalmanTrackPropagation(BmnGemTrack* track, Int_t pdg) {
    Double_t zStart = fDetector->GetGemStation(0)->GetZPosition();

    FairTrackParam parPredict = *(track->GetParamFirst());

    vector <TVector3> pointsPerTrack; // Container to store existing points and ones to be obtained when the Kalman propagation doing 
    pointsPerTrack.push_back(TVector3(parPredict.GetX(), parPredict.GetY(), parPredict.GetZ())); // Put already existing point

    vector <Double_t>* F = new vector <Double_t> (25, 0.);

    for (Int_t iStep = 1; iStep <= Int_t(zStart); iStep++) {
        fKalman->TGeoTrackPropagate(&parPredict, zStart - Double_t(iStep), pdg, F, NULL, "field");
        pointsPerTrack.push_back(TVector3(parPredict.GetX(), parPredict.GetY(), parPredict.GetZ()));
    }

    delete F;
    // track->SetParamFirst(parPredict);
    return pointsPerTrack;
}

FairTrackParam BmnLambdaAnalysis::KalmanTrackPropagation(BmnGemTrack* track, Int_t pdg, Double_t Z) {
    FairTrackParam parPredict = *(track->GetParamFirst());
    vector <Double_t>* F = new vector <Double_t> (25, 0.);
    fKalman->TGeoTrackPropagate(&parPredict, Z, pdg, F, NULL, "field");
    delete F;
    // track->SetParamFirst(parPredict);
    return parPredict;
}

vector <Double_t> BmnLambdaAnalysis::DebugCalculations(BmnGemTrack* track1, BmnGemTrack* track2, vector <Double_t> dP, vector <Double_t> dT) {
    FairTrackParam* first1 = track1->GetParamFirst();
    FairTrackParam* first2 = track2->GetParamFirst();

    Double_t Tx1 = first1->GetTx();
    Double_t Ty1 = first1->GetTy();
    Double_t Tx2 = first2->GetTx();
    Double_t Ty2 = first2->GetTy();

    Double_t p1 = Abs(1. / first1->GetQp());
    Double_t p2 = Abs(1. / first2->GetQp());

    Double_t dp1 = dP[0] * p1 / 100.;
    Double_t dp2 = dP[1] * p2 / 100.;

    Double_t dTx1 = dT[0] * Abs(Tx1) / 100.;
    Double_t dTx2 = dT[1] * Abs(Tx2) / 100.;
    Double_t dTy1 = dT[2] * Abs(Ty1) / 100.;
    Double_t dTy2 = dT[3] * Abs(Ty2) / 100.;

    Double_t m1 = fPDG->GetParticle(fPdgProton)->Mass(); // proton
    Double_t m2 = fPDG->GetParticle(fPdgPionMinus)->Mass(); // pion

    Double_t A1 = Tx1 * Tx1 + Ty1 * Ty1 + 1;
    Double_t A2 = Tx2 * Tx2 + Ty2 * Ty2 + 1;
    Double_t A1A2 = Tx1 * Tx2 + Ty1 * Ty2 + 1;
    Double_t SA12 = Sqrt(A1 * A2);

    Double_t cosTheta = A1A2 / SA12;
    Double_t SE1 = Sqrt(m1 * m1 + p1 * p1);
    Double_t SE2 = Sqrt(m2 * m2 + p2 * p2);

    // Alternative way to calculate inv. mass   
    Double_t mInv = Sqrt(m1 * m1 + m2 * m2 + 2 * SE1 * SE2 - 2 * p1 * p2 * cosTheta);

    // Calculate inv. mass error depending on accuracy of input params.    
    Double_t dMinv_dp1 = Abs(2 * p1 * SE2 / SE1 - 2 * p2 * cosTheta);
    Double_t dMinv_dp2 = Abs(2 * p2 * SE1 / SE2 - 2 * p1 * cosTheta);
    Double_t dMinv_dTx1 = Abs(2 * p1 * p2 * (Tx2 * SA12 - A1A2 * Tx1 * A2 / SA12) / (A1 * A2));
    Double_t dMinv_dTx2 = Abs(2 * p1 * p2 * (Tx1 * SA12 - A1A2 * Tx2 * A2 / SA12) / (A1 * A2));
    Double_t dMinv_dTy1 = Abs(2 * p1 * p2 * (Ty2 * SA12 - A1A2 * Ty1 * A2 / SA12) / (A1 * A2));
    Double_t dMinv_dTy2 = Abs(2 * p1 * p2 * (Ty1 * SA12 - A1A2 * Ty2 * A2 / SA12) / (A1 * A2));
    Double_t errSq = Sqrt(dMinv_dp1 * dMinv_dp1 * dp1 * dp1 + dMinv_dp2 * dMinv_dp2 * dp2 * dp2 +
            dMinv_dTx1 * dTx1 * dMinv_dTx1 * dTx1 + dMinv_dTx2 * dTx2 * dMinv_dTx2 * dTx2 + dMinv_dTy1 * dTy1 * dMinv_dTy1 * dTy1 +
            dMinv_dTy2 * dTy2 * dMinv_dTy2 * dTy2);

    vector <Double_t> res;
    res.push_back(mInv);
    res.push_back(errSq);

    return res;
}

void BmnLambdaAnalysis::McAnalysis() {
    TLorentzVector lPos, lNeg;
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* track1 = (CbmMCTrack*) fGemTracks->UncheckedAt(iTrack);
        if (track1->GetPdgCode() != fPdgProton)
            continue;

        Double_t Px1 = track1->GetPx();
        Double_t Py1 = track1->GetPy();
        Double_t Pz1 = track1->GetPz();
        Double_t P1 = track1->GetP();
        Double_t Tx1 = Px1 / Pz1;
        Double_t Ty1 = Py1 / Pz1;
        Double_t Y1 = track1->GetRapidity();
        Double_t z1 = track1->GetStartZ();

        if (P1 < fMomProtMin || P1 > fMomProtMax ||
                Tx1 < fTxProtMin || Tx1 > fTxProtMax ||
                Ty1 < fTyProtMin || Ty1 > fTyProtMax ||
                Y1 < fYProtMin || Y1 > fYProtMax || z1 > fDetector->GetGemStation(0)->GetZPosition() || z1 < 1.)
            continue;
        lPos.SetXYZM(Px1, Py1, Pz1, fPDG->GetParticle(fPdgProton)->Mass());

        for (Int_t jTrack = 0; jTrack < fGemTracks->GetEntriesFast(); jTrack++) {
            if (iTrack == jTrack)
                continue;

            CbmMCTrack * track2 = (CbmMCTrack*) fGemTracks->UncheckedAt(jTrack);
            if (track2->GetPdgCode() != fPdgPionMinus)
                continue;

            Double_t Px2 = track2->GetPx();
            Double_t Py2 = track2->GetPy();
            Double_t Pz2 = track2->GetPz();
            Double_t P2 = track2->GetP();
            Double_t Tx2 = Px2 / Pz2;
            Double_t Ty2 = Py2 / Pz2;
            Double_t Y2 = track2->GetRapidity();
            Double_t z2 = track2->GetStartZ();
            if (z1 != z2)
                continue;

            if (P2 < fMomPionMin || P2 > fMomPionMax ||
                    Tx2 < fTxPionMin || Tx2 > fTxPionMax ||
                    Ty2 < fTyPionMin || Ty2 > fTyPionMax ||
                    Y2 < fYPionMin || Y2 > fYPionMax || z2 > fDetector->GetGemStation(0)->GetZPosition() || z1 < 1.)

                continue;
            lNeg.SetXYZM(Px2, Py2, Pz2, fPDG->GetParticle(fPdgPionMinus)->Mass());
            fLambdaInvMass->Fill(TLorentzVector((lPos + lNeg)).Mag());
        }
    }
}

// -------------------------------------------------------------------

InitStatus BmnLambdaAnalysis::Init() {
    cout << "\nBmnLambdaAnalysis::Init()" << endl;
    TGeoManager::Import("geofile_full.root"); /// FIXME

    FairRootManager* ioman = FairRootManager::Instance();
    TString inFileName = TString(ioman->GetInFile()->GetName());
    fOutFileName = ioman->GetOutFile()->GetName();
    if (inFileName.Contains("evetest"))
        fUseMc = kTRUE;
    else if (inFileName.Contains("bmndst"))
        fUseMc = kFALSE;
    else {
        cout << "Specify a correct file name!" << endl;
        throw;
    }

    fBranchGemTracks = (!fUseMc) ? "BmnGemTrack" : "MCTrack";

    if (!fUseMc) {
        fGemHits = (TClonesArray*) ioman->GetObject(fBranchGemHits.Data());
        fVertex = (TClonesArray*) ioman->GetObject(fBranchVertex.Data());
    }
    fGemTracks = (TClonesArray*) ioman->GetObject(fBranchGemTracks.Data());

    fLambdaInvMass = new TH1F("LambdaInvMass", "#Lambda -> #pi^{-}p, Inv. mass", 100, 1.05, 1.3);
    fPDG = TDatabasePDG::Instance();

    if (fCutsDistrOnly) {
        const Int_t nBins = 100;
        fLambdaMomenta = new TH1F("Lambda mom. distrib.", "Lambda mom. distrib.", nBins, 0.0, 0.0);
        fProtonMomenta = new TH1F("Prot. mom. distrib.", "Prot. mom. distrib.", nBins, 0.0, 0.0);
        fPionMomenta = new TH1F("Pion. mom. distrib.", "Pion. mom. distrib.", nBins, 0.0, 0.0);
        fProtonTx = new TH1F("Prot. Tx-distrib.", "Prot. Tx-distrib.", nBins, -1.0, +1.0);
        fProtonTy = new TH1F("Prot. Ty-distrib.", "Prot. Tx-distrib.", nBins, -1.0, +1.0);
        fPionTx = new TH1F("Pion. Tx-distrib.", "Pion. Tx-distrib.", nBins, -1.0, +1.0);
        fPionTy = new TH1F("Pion. Ty-distrib.", "Pion. Ty-distrib.", nBins, -1.0, +1.0);
        fStartZ = new TH1F("Distrib. of start Z-position", "Distrib. of start Z-position", nBins, 0.0, 0.0);
        fEtaProton = new TH1F("Proton #eta distrib.", "Proton #eta distrib.", nBins, 0.0, 0.0);
        fEtaPion = new TH1F("Pion #eta distrib.", "Pion #eta distrib.", nBins, 0.0, 0.0);

        fNhitsPerProton = new TH1I("NhitsPerProton", "NhitsPerProton", 7, 0, 7);
        fNhitsPerPion = new TH1I("NhitsPerPion", "NhitsPerPion", 7, 0, 7);
        fNhitsPerProtonVsP = new TH2F("NhitsPerProtonVsP", "NhitsPerProtonVsP", 7, 0, 7, nBins, 0., 5.);
        fNhitsPerPionVsP = new TH2F("NhitsPerPionVsP", "NhitsPerPionVsP", 7, 0, 7, nBins, 0., 2.);
    }

    if (!fUseMc) {
        fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.dat");
        fMagField->SetScale(1.3);
        fMagField->Init();

        FairRunAna::Instance()->SetField(fMagField);
        fField = FairRunAna::Instance()->GetField();
        fKalman = new BmnKalmanFilter_tmp();

        //        fzVX = new TH1F("zVX", "zVX", 200, 0., fDetector->GetGemStation(0)->GetZPosition());
        fzVX = new TH1F("zVX", "zVX", 500, -50., 50.);
        fzVY = new TH1F("zVY", "zVY", 500, -50., 50.);

        fMinDistNonOverlappedCurves = new TH1F("MinDistNonOverlappedCurves, XZ", "MinDistNonOverlappedCurves, XZ", 100, 0., 10.);

        fProtonVpVp = new TH1F("fProtonVpVp", "fProtonVpVp", 100, 0., 0.);
        fPionVpVp = new TH1F("fPionVpVp", "fPionVpVp", 100, 0., 0.);
        fProtonV0PionV0 = new TH1F("fProtonV0PionV0", "fProtonV0PionV0", 100, 0., 0.);
        fVertexDiff = new TH1F("fVertexDiff", "fVertexDiff", 100, 0., 0.);
    }

    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnLambdaAnalysis::Exec(Option_t* option) {
    fEventCounter++;
    if (fEventCounter % 1000 == 0)
        cout << fEventCounter << endl;

    fEventVertex = (CbmVertex*) fVertex->UncheckedAt(0);

    if (!fUseMc && !fCutsDistrOnly)
        RecoAnalysis();
    else if (fUseMc && !fCutsDistrOnly)
        McAnalysis();
    else if (fUseMc && fCutsDistrOnly)
        ObtainCuts();
    else {

        cout << "Specify analysis conditions precisely!" << endl;
        throw;
    }
}

void BmnLambdaAnalysis::ObtainCuts() {
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* track = (CbmMCTrack*) fGemTracks->UncheckedAt(iTrack);

        if (track->GetPdgCode() != fPdgLambda)
            continue;

        if (track->GetStartZ() > fDetector->GetGemStation(0)->GetZPosition())
            continue;

        CbmMCTrack * proton = NULL;
        CbmMCTrack * pion = NULL;
        // Int_t nProtons, nPions = 0;
        for (Int_t jTrack = 0; jTrack < fGemTracks->GetEntriesFast(); jTrack++) {
            CbmMCTrack* trackFromDecay = (CbmMCTrack*) fGemTracks->UncheckedAt(jTrack);
            Int_t mId = trackFromDecay->GetMotherId();

            if (mId != iTrack)
                continue;

            Int_t pdg = trackFromDecay->GetPdgCode();

            if (pdg == fPdgProton)
                proton = trackFromDecay;

            else if (pdg == fPdgPionMinus)
                pion = trackFromDecay;

            else
                continue;
        }

        if (pion && proton) {

            fProtonMomenta->Fill(proton->GetP());
            fProtonTx->Fill(proton->GetPx() / proton->GetPz());
            fProtonTy->Fill(proton->GetPy() / proton->GetPz());
            fEtaProton->Fill(proton->GetRapidity());

            fPionMomenta->Fill(pion->GetP());
            fPionTx->Fill(pion->GetPx() / pion->GetPz());
            fPionTy->Fill(pion->GetPy() / pion->GetPz());
            fEtaPion->Fill(pion->GetRapidity());

            fStartZ->Fill(pion->GetStartZ());
            fLambdaMomenta->Fill(track->GetP());

            fNhitsPerProton->Fill(proton->GetNPoints(kGEM));
            fNhitsPerPion->Fill(pion->GetNPoints(kGEM));

            fNhitsPerProtonVsP->Fill(proton->GetNPoints(kGEM), proton->GetP());
            fNhitsPerPionVsP->Fill(pion->GetNPoints(kGEM), pion->GetP());
        }
    }
}


// -------------------------------------------------------------------

void BmnLambdaAnalysis::Finish() {
    if (!fCutsDistrOnly) {
        TF1* fI = new TF1("dblgaus", "[0] * TMath::Exp(-(x - [1]) * (x - [1]) / [2] / [2]) + [3] * TMath::Exp(-(x - [4]) * (x - [4]) / [5] / [5])", 1, 1.2);
        fI->SetParameter(1, 1.115);
        fI->SetParameter(4, 1.115);
        // fLambdaInvMass->Fit("dblgaus", "WW", "", 1.05, 1.15);

        fLambdaInvMass->Write();
        fzVX->Write();
        fzVY->Write();

        fMinDistNonOverlappedCurves->Write();

        fProtonVpVp->Write();
        fPionVpVp->Write();
        fProtonV0PionV0->Write();
        fVertexDiff->Write();
    } else {
        fStartZ->Write();

        fProtonMomenta->Write();
        fPionMomenta->Write();

        fProtonTx->Write();
        fProtonTy->Write();
        fPionTx->Write();
        fPionTy->Write();

        fEtaProton->Write();
        fEtaPion->Write();

        fLambdaMomenta->Write();

        fNhitsPerProton->Write();
        fNhitsPerPion->Write();
        fNhitsPerProtonVsP->Write();
        fNhitsPerPionVsP->Write();
    }

    if (!fUseMc) {
        delete fKalman;
        delete fMagField;
    }

    cout << "\n-I- [BmnLambdaAnalysis::Finish] " << endl;
}

TVector3 BmnLambdaAnalysis::FitParabola(vector <TVector3> points) {
    // Prevent bad fits
    if (Abs(points[points.size() - 1].X() - points[1].X()) < FLT_EPSILON) 
      return TVector3(0., 0., 0.);  
    
    TGraph* tmp = new TGraph();
    for (Int_t iSize = 0; iSize < points.size(); iSize++) 
        tmp->SetPoint(iSize, points[iSize].Z(), points[iSize].X());
    
    TFitResultPtr fitPtr = tmp->Fit("pol2", "SQww");
    Double_t a = fitPtr->Parameter(2);
    Double_t b = fitPtr->Parameter(1);
    Double_t c = fitPtr->Parameter(0);

    // Draw prolongated trajectory ...
    if (fDebugCalculations) {
        TCanvas* ca = new TCanvas("c1", "c1", 1200, 800);
        tmp->Draw("AP*");
        ca->SaveAs("c1.png");
        delete ca;
    }
    delete tmp;

    if (!isnan(a * b * c) && !isinf(a * b * c))
        return TVector3(a, b, c);
    else
        return TVector3(0., 0., 0.);
}

TVector2 BmnLambdaAnalysis::SecondaryVertexY(FairTrackParam* param1, FairTrackParam* param2) {
    Double_t Ty1 = param1->GetTy();
    Double_t Y1 = param1->GetY();

    Double_t Ty2 = param2->GetTy();
    Double_t Y2 = param2->GetY();

    Double_t zV = (Y2 - Y1) / (Ty1 - Ty2);
    Double_t yV = Ty1 * zV + Y1;

    return TVector2(yV, zV);
}

void BmnLambdaAnalysis::CalculateMinDistance(TVector3 paramsCurv1, TVector3 paramsCurv2, Double_t* arr) {
    Double_t a1 = paramsCurv1[0];
    Double_t b1 = paramsCurv1[1];
    Double_t c1 = paramsCurv1[2];

    Double_t a2 = paramsCurv2[0];
    Double_t b2 = paramsCurv2[1];
    Double_t c2 = paramsCurv2[2];

    // Do they have a cross-point? (D >= 0) 
    // If not than D < 0

    Double_t D = (b1 - b2) * (b1 - b2) - 4 * (a1 - a2) * (c1 - c2);

    for (Int_t iSize = 0; iSize < 5; iSize++)
        arr[iSize] = 0.;

    if (D < 0) {
        arr[4] = DBL_MAX;
        const Double_t step = 0.1; // step of scanning along curve, in cm
        Double_t zStart = fDetector->GetGemStation(0)->GetZPosition();
        for (Int_t iStep = 0; iStep < Int_t(zStart / step); iStep++) {
            Double_t Zi = zStart - iStep * step;
            Double_t Xi = a1 * Zi * Zi + b1 * Zi + c1;
            for (Int_t jStep = 0; jStep < Int_t(zStart / step); jStep++) {
                Double_t Zj = zStart - jStep * step;
                Double_t Xj = a2 * Zj * Zj + b2 * Zj + c2;
                Double_t dist = Dist(Zi, Xi, Zj, Xj); // taken from BmnMath.h
                if (dist < arr[4]) {
                    arr[0] = Zi;
                    arr[1] = Xi;
                    arr[2] = Zj;
                    arr[3] = Xj;
                    arr[4] = dist;
                }
            }
        }
        fMinDistNonOverlappedCurves->Fill(arr[4]);
    } else {
        if (Abs(D) < FLT_EPSILON) {
            cout << "Something strange happens!!! D is very close to zero! " << fEventCounter << endl;
            // throw;
        }

        Double_t z1 = (-(b1 - b2) + Sqrt(D)) / (2 * (a1 - a2));
        Double_t x1 = a1 * z1 * z1 + b1 * z1 + c1;
        Double_t z2 = (-(b1 - b2) - Sqrt(D)) / (2 * (a1 - a2));
        Double_t x2 = a2 * z2 * z2 + b2 * z2 + c2;

        arr[0] = z1;
        arr[1] = x1;
        arr[2] = z2;
        arr[3] = x2;
    }
}

ClassImp(BmnLambdaAnalysis);
