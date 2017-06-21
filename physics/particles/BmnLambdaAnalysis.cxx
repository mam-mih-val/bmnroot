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
fGeometry(config),
fDebugCalculations(kFALSE),
fKalman(NULL),
fField(NULL),
fParticlePair(NULL),
fParticlePairCuts(NULL) {
    for (Int_t i = 0; i < 2; i++)
        for (Int_t j = 0; j < 2; j++) {
            Double_t val = (j == 0) ? -DBL_MAX : DBL_MAX; 
            fMom[i][j] = val;
            fTx[i][j] = val;
            fTy[i][j] = val;
            fY[i][j] = val;
        }
    
    for (Int_t i = 0; i < 2; i++) {
        Double_t val = (i == 0) ? 0. : DBL_MAX;
        fVpVpProton[i] = val;
        fVpVpPion[i] = val;
        fV0ProtonPion[i] = val;
        fV0VpDiff[i] = val;
    }

    for (Int_t i = 0; i < 6; i++)
        fInputUncertainties[i] = 0.;

    //    fBranchGemHits = "BmnGemStripHit";
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

vector <Double_t> BmnLambdaAnalysis::GeometryCuts(FairTrackParam proton_V0, FairTrackParam pion_V0, FairTrackParam proton_Vp, FairTrackParam pion_Vp) {
    Double_t X = (fUseMc) ? 0. : fEventVertex->GetX();
    Double_t Y = (fUseMc) ? 0. : fEventVertex->GetY();
    Double_t Z = (fUseMc) ? 0. : fEventVertex->GetZ();

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
    // 2)
    // Distance beetween Vp and Vp_prot_extrap
    Double_t pionVpVp = TVector3(pionVp - Vp).Mag();
    // 3)
    // Distance between proton and pion at V0
    Double_t protonV0PionV0 = TVector3(protonV0 - pionV0).Mag();
    // 4)
    // Distance between V0 and Vp along beamline
    Double_t vertexDiff = proton_V0.GetZ() - Z;

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

void BmnLambdaAnalysis::FindFirstPointOnMCTrack(Int_t iTrack, BmnGemTrack* track, Int_t sign) {
    FairTrackParam param;
    for (Int_t iPoint = 0; iPoint < fGemHits->GetEntriesFast(); iPoint++) {
        CbmStsPoint* gemPoint = (CbmStsPoint*) fGemHits->UncheckedAt(iPoint);
        Int_t TrackID = gemPoint->GetTrackID();

        if (TrackID != iTrack || gemPoint->GetZIn() > 35)
            continue;
        Double_t Px = gemPoint->GetPx();
        Double_t Py = gemPoint->GetPy();
        Double_t Pz = gemPoint->GetPz();

        param.SetTx(Px / Pz);
        param.SetTy(Py / Pz);
        param.SetQp(sign / Sqrt(Px * Px + Py * Py + Pz * Pz));
        param.SetX(gemPoint->GetXIn());
        param.SetY(gemPoint->GetYIn());
        param.SetZ(gemPoint->GetZIn());
        track->SetParamFirst(param);
        // param.Print();
        break;
    }
}

Bool_t BmnLambdaAnalysis::CheckTrack(BmnGemTrack* track, Int_t pdgCode) {
    Double_t Tx = track->GetParamFirst()->GetTx();
    Double_t Ty = track->GetParamFirst()->GetTy();
    Double_t p = 1. / track->GetParamFirst()->GetQp();

    Double_t m2 = fPDG->GetParticle(pdgCode)->Mass() * fPDG->GetParticle(pdgCode)->Mass();
    Double_t E = Sqrt(p * p + m2);
    Double_t Pz = Abs(p) * Sqrt(1 - Tx * Tx - Ty * Ty);
    Double_t Y = 0.5 * Log((E + Pz) / (E - Pz));
    
    Int_t sign = CheckSign(fPDG->GetParticle(pdgCode)->Charge());
    Int_t nPart = (sign > 0) ? 0 : 1;

    if (sign * p < 0 || Abs(p) < fMom[nPart][0] || Abs(p) > fMom[nPart][1] ||
            Tx < fTx[nPart][0] || Tx > fTx[nPart][1] ||
            Ty < fTy[nPart][0] || Ty > fTy[nPart][1] ||
            Y < fY[nPart][0] || Y > fY[nPart][1])
        return kFALSE;
    
    else
        return kTRUE;
}

void BmnLambdaAnalysis::Analysis() {
    TLorentzVector lPos, lNeg;
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack Track1;
        BmnGemTrack* track1 = &Track1;
        if (fUseMc) {
            TParticlePDG* particle1 = fPDG->GetParticle(((CbmMCTrack*) fGemTracks->UncheckedAt(iTrack))->GetPdgCode());
            if (!particle1)
                continue;
            Double_t Q1 = particle1->Charge();
            if (!(Q1 > 0))
                continue;

            FindFirstPointOnMCTrack(iTrack, track1, CheckSign(Q1));
        } else
            track1 = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);

        if (!CheckTrack(track1, fPdgProton))
            continue;

        for (Int_t jTrack = 0; jTrack < fGemTracks->GetEntriesFast(); jTrack++) {
            if (iTrack == jTrack)
                continue;

            BmnGemTrack Track2;
            BmnGemTrack* track2 = &Track2;

            if (fUseMc) {
                TParticlePDG* particle2 = fPDG->GetParticle(((CbmMCTrack*) fGemTracks->UncheckedAt(jTrack))->GetPdgCode());
                if (!particle2)
                    continue;
                Double_t Q2 = particle2->Charge();
                if (!(Q2 < 0))
                    continue;

                FindFirstPointOnMCTrack(jTrack, track2, CheckSign(Q2));
            } else
                track2 = (BmnGemTrack*) fGemTracks->UncheckedAt(jTrack);
            
            if (!CheckTrack(track2, fPdgPionMinus))
                continue;
           
            // Calculate zV and yV using the YZ-trajectory ...
            // Probably, it has to be parametrized by a straight line
            TVector2 yzVertex = SecondaryVertexY(track1->GetParamFirst(), track2->GetParamFirst());
            Double_t zVY = yzVertex.Y();
            // Double_t yV = yzVertex.X();
            if (zVY < 1.0 || zVY > 30)
                continue;

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

            // Array is used either to store point coord. and dist. between (z1[0], x1[1], z2[2], x2[3], min_dist[4]) them or 
            // points of overlapping (z1, x1, z2, x2, 0.) when the curves are overlapped.
            Double_t* pointsAndMinDist = new Double_t[5];
            CalculateMinDistance(protParametrizedTraject, pionParametrizedTraject, pointsAndMinDist);

            // New track params. at secondary vertex (V0) and Vp to be stored here
            FairTrackParam proton_V0, pion_V0, proton_Vp, pion_Vp;

            Double_t zVX = Min(pointsAndMinDist[0], pointsAndMinDist[2]);
            delete [] pointsAndMinDist;
            if (zVX < 1. || zVX > 30.)
                continue;

            Double_t minZ = 0.0; // minZ is a possible candidate to be the second. vertex (V0)
            if (Abs(zVX - zVY) < 5.0)
                minZ = (zVY + zVX) * 0.5;
            else
                continue;

            // Go to V0 ...
            proton_V0 = KalmanTrackPropagation(track1, fPdgProton, minZ);
            pion_V0 = KalmanTrackPropagation(track2, fPdgPionMinus, minZ);

            // Go to Vp
            Double_t Vpz = 0.; // FIXME (0. --> fEventVertex->GetZ())
            proton_Vp = KalmanTrackPropagation(track1, fPdgProton, Vpz);
            pion_Vp = KalmanTrackPropagation(track2, fPdgPionMinus, Vpz);

            vector <Double_t> geomCuts = GeometryCuts(proton_V0, pion_V0, proton_Vp, pion_Vp);

            // Apply geometry cuts from vector <<geomCuts>>
            // Distance beetween Vp and Vp_prot_extrap   [0]
            // Distance beetween Vp and Vp_prot_extrap   [1]
            // Distance between proton and pion at V0    [2]
            // Distance between V0 and Vp along beamline [3]
            // Write cut values to output tree ...
            BmnParticlePair* partPairCuts = new((*fParticlePairCuts)[fParticlePairCuts->GetEntriesFast()]) BmnParticlePair();
            partPairCuts->SetV0XZ(zVX);
            partPairCuts->SetV0YZ(zVY);
            partPairCuts->SetVpPart1(geomCuts[0]);
            partPairCuts->SetVpPart2(geomCuts[1]);
            partPairCuts->SetV0Part1Part2(geomCuts[2]);
            partPairCuts->SetV0VpDist(geomCuts[3]);

            if (geomCuts[0] < fVpVpProton[0] || geomCuts[0] > fVpVpProton[1] ||
                    geomCuts[1] < fVpVpPion[0] || geomCuts[1] > fVpVpPion[1] ||
                    geomCuts[2] < fV0ProtonPion[0] || geomCuts[2] > fV0ProtonPion[1] ||
                    geomCuts[3] < fV0VpDiff[0] || geomCuts[3] > fV0VpDiff[1])
                continue;

            // Track params. are redefined (correspond to zVX) ...
            Double_t Tx1 = proton_V0.GetTx();
            Double_t Ty1 = proton_V0.GetTy();
            Double_t Tx2 = pion_V0.GetTx();
            Double_t Ty2 = pion_V0.GetTy();
            Double_t p1 = 1. / proton_V0.GetQp();
            Double_t p2 = 1. / pion_V0.GetQp();

            Double_t A1 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
            lPos.SetXYZM(Tx1 * A1 * p1, Ty1 * A1 * p1, p1 * A1, fPDG->GetParticle(fPdgProton)->Mass());

            p2 *= -1.; // Since in the calculations pos. mom. values should be used

            Double_t A2 = 1. / Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
            lNeg.SetXYZM(Tx2 * A2 * p2, Ty2 * A2 * p2, p2 * A2, fPDG->GetParticle(fPdgPionMinus)->Mass());

            // Ready to write pair info supposing it to be from a decayed Lambda0 ...
            BmnParticlePair* partPair = new((*fParticlePair)[fParticlePair->GetEntriesFast()]) BmnParticlePair();
            partPair->SetInvMass(TLorentzVector((lPos + lNeg)).Mag());
            partPair->SetV0XZ(zVX);
            partPair->SetV0YZ(zVY);
            partPair->SetVpPart1(geomCuts[0]);
            partPair->SetVpPart2(geomCuts[1]);
            partPair->SetV0Part1Part2(geomCuts[2]);
            partPair->SetV0VpDist(geomCuts[3]);
        }
    }
}

// -------------------------------------------------------------------

InitStatus BmnLambdaAnalysis::Init() {
    cout << "\nBmnLambdaAnalysis::Init()" << endl;
    // Read current geometry (RunSpring2017) from database 
    Char_t* geoFileName = (Char_t*) "current_geo_file.root";
    // 1242 is an arbitrary file from period 6
    Int_t res_code = UniDbRun::ReadGeometryFile(6, 1242, geoFileName);
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-1);
    }
    TGeoManager::Import(geoFileName);

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
    fBranchGemHits = (!fUseMc) ? "BmnGemStripHit" : "StsPoint";

    fGemHits = (TClonesArray*) ioman->GetObject(fBranchGemHits.Data());
    fGemTracks = (TClonesArray*) ioman->GetObject(fBranchGemTracks.Data());

    if (!fUseMc)
        fVertex = (TClonesArray*) ioman->GetObject(fBranchVertex.Data());

    fParticlePair = new TClonesArray("BmnParticlePair");
    ioman->Register("ParticlePair", "Lambda", fParticlePair, kTRUE);

    fParticlePairCuts = new TClonesArray("BmnParticlePairCuts");
    ioman->Register("ParticlePairCuts", "_Lambda", fParticlePairCuts, kTRUE);

    fPDG = TDatabasePDG::Instance();

    fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.dat");
    fMagField->SetScale(1.3);
    fMagField->Init();

    FairRunAna::Instance()->SetField(fMagField);
    fField = FairRunAna::Instance()->GetField();
    fKalman = new BmnKalmanFilter_tmp();

    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnLambdaAnalysis::Exec(Option_t* option) {
    fParticlePairCuts->Delete();
    fParticlePair->Delete();

    fEventCounter++;
    if (fEventCounter % 1000 == 0)
        cout << fEventCounter << endl;

    if (!fUseMc)
        fEventVertex = (CbmVertex*) fVertex->UncheckedAt(0);

    Analysis();
}
// -------------------------------------------------------------------

void BmnLambdaAnalysis::Finish() {
    delete fKalman;
    delete fMagField;
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
    Double_t b1 = param1->GetY() - Ty1 * param1->GetZ();

    Double_t Ty2 = param2->GetTy();
    Double_t b2 = param2->GetY() - Ty2 * param2->GetZ();

    Double_t zV = (b2 - b1) / (Ty1 - Ty2);
    Double_t yV = Ty1 * zV + b1;

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
    } else {
        if (Abs(D) < FLT_EPSILON)
            cout << "Something strange happens!!! D is very close to zero! " << fEventCounter << endl;

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
