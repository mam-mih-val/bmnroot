// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-12-27

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnTwoParticleDecay                                                        //
//                                                                            //
//  A supplementary class for two-body decay reconstruction                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <TCanvas.h>
#include <TGeoManager.h>
#include <Fit/FitResult.h>
#include "BmnTwoParticleDecay.h"

BmnTwoParticleDecay::BmnTwoParticleDecay(BmnGemStripConfiguration::GEM_CONFIG config, Int_t runNumb) :
fRunPeriod(6),
fRunId(runNumb),
// Particles set by default:
fPDG1(2212), // proton
fPDG2(-211), // pion
fPdgParticle1(fPDG1),
fPdgParticle2(fPDG2),
fEventCounter(0),
fGeometry(config),
fKalman(NULL),
fField(NULL),
fParticlePair(NULL),
fVertex(NULL),
fIsUseRealVertex(kFALSE),
fSiRequired(kFALSE),
fGlobalMatches(NULL) {
    fMcVertex.SetXYZ(0., 0., 0.);
    Double_t val = 0.;

    for (Int_t i = 0; i < 2; i++) {
        val = (i == 0) ? -DBL_MAX : DBL_MAX;
        for (Int_t j = 0; j < 2; j++) {
            val = (j == 0) ? -DBL_MAX : DBL_MAX;
            fMom[i][j] = val;
            fEta[i][j] = val;
            fTx[i][j] = val;
            fTy[i][j] = val;
            fDCA[i][j] = val;
            fDCA12[j] = val;
            fPath[j] = val;
        }
    }

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

BmnTwoParticleDecay::~BmnTwoParticleDecay() {
    delete fDetector;
}

// Ideal calculations what we would have ...

void BmnTwoParticleDecay::CalculateReconstuctableParticles() {
    const Int_t kHitsPerTrackMin = 4; // Min. number of hits to be per a reco-track

    if (fMCTracks->GetEntriesFast() < 2)
        return;

    for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* mcTrack_pos = (CbmMCTrack*) fMCTracks->UncheckedAt(iTrack);
        if (mcTrack_pos->GetPdgCode() < 0)
            continue;
        Int_t motherId_pos = mcTrack_pos->GetMotherId();
        if (motherId_pos == -1)
            continue;
        Int_t pdgMother = ((CbmMCTrack*) fMCTracks->UncheckedAt(motherId_pos))->GetPdgCode();
        if (pdgMother != fPDGDecay)
            continue;

        for (Int_t jTrack = 0; jTrack < fMCTracks->GetEntriesFast(); jTrack++) {
            if (iTrack == jTrack)
                continue;
            CbmMCTrack* mcTrack_neg = (CbmMCTrack*) fMCTracks->UncheckedAt(jTrack);
            if (mcTrack_neg->GetPdgCode() > 0)
                continue;

            Int_t motherId_neg = mcTrack_neg->GetMotherId();
            if (motherId_neg != motherId_pos)
                continue;

            if (mcTrack_pos->GetNPoints(kGEM) >= kHitsPerTrackMin && mcTrack_neg->GetNPoints(kGEM) >= kHitsPerTrackMin) {
                //cout << fEventCounter << endl;
                //cout << "P_{lambda} = " << ((CbmMCTrack*) fMCTracks->UncheckedAt(motherId_pos))->GetP() <<
                //         " P_{p} = " << mcTrack_pos->GetP() << " P_{pi-} = " << mcTrack_neg->GetP() << 
                //         " Pz_{p} = " << mcTrack_pos->GetPz() << " Pz_{pi} = " << mcTrack_neg->GetPz() << endl;

                Double_t PLambda = ((CbmMCTrack*) fMCTracks->UncheckedAt(motherId_pos))->GetP();
                Double_t PzLambda = ((CbmMCTrack*) fMCTracks->UncheckedAt(motherId_pos))->GetPz();
                Double_t etaLambda = 0.5 * TMath::Log((PLambda + PzLambda) / (PLambda - PzLambda));

                // 1d-histos ...
                hSim[0]->Fill(PLambda);
                hSim[1]->Fill(etaLambda);
                Double_t Pprot = mcTrack_pos->GetP();
                hSim[2]->Fill(Pprot);
                Double_t Ppi = mcTrack_neg->GetP();
                hSim[3]->Fill(Ppi);
                Double_t etaP = 0.5 * TMath::Log((mcTrack_pos->GetP() + mcTrack_pos->GetPz()) / (mcTrack_pos->GetP() - mcTrack_pos->GetPz()));
                hSim[4]->Fill(etaP);
                Double_t etaPi = 0.5 * TMath::Log((mcTrack_neg->GetP() + mcTrack_neg->GetPz()) / (mcTrack_neg->GetP() - mcTrack_neg->GetPz()));
                hSim[5]->Fill(etaPi);
                hSim[6]->Fill(mcTrack_pos->GetNPoints(kGEM));
                hSim[7]->Fill(mcTrack_neg->GetNPoints(kGEM));

                // 2d-histos ...
                h2Sim[0]->Fill(etaLambda, PLambda);
                h2Sim[1]->Fill(etaP, Pprot);
                h2Sim[2]->Fill(etaPi, Ppi);
                h2Sim[3]->Fill(Pprot, Ppi);
                h2Sim[4]->Fill(etaP, etaPi);
                h2Sim[5]->Fill(mcTrack_pos->GetPx() / mcTrack_pos->GetPz(), mcTrack_neg->GetPx() / mcTrack_neg->GetPz());
                h2Sim[6]->Fill(mcTrack_pos->GetPy() / mcTrack_pos->GetPz(), mcTrack_neg->GetPy() / mcTrack_neg->GetPz());
                // cout << fEventCounter << " " << mcTrack_pos->GetNPoints(kGEM) << " " << mcTrack_neg->GetNPoints(kGEM) << " " << etaP << " " << etaPi << endl;

                break;
            }
        }
    }
}

vector <Double_t> BmnTwoParticleDecay::GeomTopology(FairTrackParam proton_V0, FairTrackParam pion_V0, FairTrackParam proton_Vp, FairTrackParam pion_Vp) {
    Double_t X = 0., Y = 0., Z = 0.;

    // evetest.root -->
    if (fUseMc) {
        X = fMcVertex.X();
        Y = fMcVertex.Y();
        Z = fMcVertex.Z();
    }// bmndst.root -->
    else {
        X = (!fIsUseRealVertex) ? fEventVertex->GetRoughX() : fEventVertex->GetX();
        Y = (!fIsUseRealVertex) ? fEventVertex->GetRoughY() : fEventVertex->GetY();
        Z = (!fIsUseRealVertex) ? fEventVertex->GetRoughZ() : fEventVertex->GetZ();
    }
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
    cuts.push_back(Abs(vertexDiff));

    return cuts;
}

vector <TVector3> BmnTwoParticleDecay::KalmanTrackPropagation(BmnGlobalTrack* track, Int_t pdg) {
    Double_t zStart = fDetector->GetGemStation(0)->GetZPosition();

    FairTrackParam parPredict = *(track->GetParamFirst());

    vector <TVector3> pointsPerTrack; // Container to store existing points and ones to be obtained when the Kalman propagation doing 
    pointsPerTrack.push_back(TVector3(parPredict.GetX(), parPredict.GetY(), parPredict.GetZ())); // Put already existing point

    vector <Double_t>* F = new vector <Double_t> (25, 0.);

    for (Int_t iStep = 1; iStep <= Int_t(zStart); iStep++) { //FIXME
        fKalman->TGeoTrackPropagate(&parPredict, zStart - Double_t(iStep), pdg, F, NULL, kTRUE);
        pointsPerTrack.push_back(TVector3(parPredict.GetX(), parPredict.GetY(), parPredict.GetZ()));
    }

    delete F;
    // track->SetParamFirst(parPredict);
    return pointsPerTrack;
}

FairTrackParam BmnTwoParticleDecay::KalmanTrackPropagation(BmnGlobalTrack* track, Int_t pdg, Double_t Z) {
    FairTrackParam parPredict = *(track->GetParamFirst());
    vector <Double_t>* F = new vector <Double_t> (25, 0.);
    fKalman->TGeoTrackPropagate(&parPredict, Z, pdg, F, NULL, kTRUE);
    delete F;
    // track->SetParamFirst(parPredict);
    return parPredict;
}

void BmnTwoParticleDecay::FindFirstPointOnMCTrack(Int_t iTrack, BmnGlobalTrack* track, Int_t sign) {
    FairTrackParam param;
    for (Int_t iPoint = 0; iPoint < fGemPoints->GetEntriesFast(); iPoint++) {
        CbmStsPoint* gemPoint = (CbmStsPoint*) fGemPoints->UncheckedAt(iPoint);
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

Bool_t BmnTwoParticleDecay::CheckTrack(BmnGlobalTrack* track, Int_t pdgCode, Double_t& mom, Double_t& eta) {
    Double_t Tx = track->GetParamFirst()->GetTx();
    Double_t Ty = track->GetParamFirst()->GetTy();
    Double_t p = 1. / track->GetParamFirst()->GetQp();

    //    Double_t Pz = Abs(p) * Sqrt(1 - Tx * Tx - Ty * Ty);
    Double_t Pz = Abs(p) / Sqrt(1 + Tx * Tx + Ty * Ty);

    Int_t sign = CheckSign(fPDG->GetParticle(pdgCode)->Charge());
    Int_t nPart = (sign > 0) ? 0 : 1;

    mom = Abs(p);
    eta = 0.5 * Log((Abs(p) + Pz) / (Abs(p) - Pz));

    if (sign * p < 0 || Abs(p) < fMom[nPart][0] || Abs(p) > fMom[nPart][1] ||
            Tx < fTx[nPart][0] || Tx > fTx[nPart][1] ||
            Ty < fTy[nPart][0] || Ty > fTy[nPart][1] ||
            eta < fEta[nPart][0] || eta > fEta[nPart][1])
        return kFALSE;

    else
        return kTRUE;
}

void BmnTwoParticleDecay::Analysis() {
    const Int_t nV0 = 2;
    TLorentzVector lPos[nV0], lNeg[nV0];
    TClonesArray* arr = ((fUseMc && fGlobalMatches) || !fUseMc) ? fGlobalTracks : fMCTracks;
    
    for (Int_t iTrack = 0; iTrack < arr->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack Track1;
        BmnGlobalTrack* track1 = &Track1;

        if (fUseMc && !fGlobalMatches) {
            TParticlePDG* particle1 = fPDG->GetParticle(((CbmMCTrack*) arr->UncheckedAt(iTrack))->GetPdgCode());
            if (!particle1)
                continue;
            Double_t Q1 = particle1->Charge();
            if (!(Q1 > 0))
                continue;

            FindFirstPointOnMCTrack(iTrack, track1, CheckSign(Q1));
        } else {
            track1 = (BmnGlobalTrack*) arr->UncheckedAt(iTrack);
            if (fSiRequired && track1->GetSilHitIndex() == -1)
                continue;
        }

        Double_t _p1, _eta1;
        if (!CheckTrack(track1, fPdgParticle1, _p1, _eta1))
            continue;

        for (Int_t jTrack = 0; jTrack < arr->GetEntriesFast(); jTrack++) {
            if (iTrack == jTrack)
                continue;

            BmnGlobalTrack Track2;
            BmnGlobalTrack* track2 = &Track2;

            if (fUseMc && !fGlobalMatches) {
                TParticlePDG* particle2 = fPDG->GetParticle(((CbmMCTrack*) arr->UncheckedAt(jTrack))->GetPdgCode());
                if (!particle2)
                    continue;
                Double_t Q2 = particle2->Charge();
                if (!(Q2 < 0))
                    continue;

                FindFirstPointOnMCTrack(jTrack, track2, CheckSign(Q2));
            } else {
                track2 = (BmnGlobalTrack*) arr->UncheckedAt(jTrack);
                if (fSiRequired && track2->GetSilHitIndex() == -1)
                    continue;
            }

            Double_t _p2, _eta2;
            if (!CheckTrack(track2, fPdgParticle2, _p2, _eta2))
                continue;

            // Array to store V0XZ and V0YZ ...           
            Double_t V0Z[nV0] = {0., 0.}; // V0XZ and V0YZ

            // Go to primary vertex Vp
            Double_t Vpz = (fUseMc) ? fMcVertex.Z() : ((fIsUseRealVertex) ? fEventVertex->GetZ() : fEventVertex->GetRoughZ());

            FairTrackParam proton_Vp = KalmanTrackPropagation(track1, fPdgParticle1, Vpz);
            FairTrackParam pion_Vp = KalmanTrackPropagation(track2, fPdgParticle2, Vpz);

            // Calculate V0YZ the YZ-projection ...
            V0Z[1] = SecondaryVertexY(track1->GetParamFirst(), track2->GetParamFirst()).Y(); // V0ZY

            // Use Kalman to estimate a possible secondary vertex in xz-plane
            // Propagation to Z = 0; 
            // After the procedure <<track(1,2)->GetParamFirst()>> will be updated by shifted values corresponding to Z = 0 
            vector <TVector3> protonTrackPoints = KalmanTrackPropagation(track1, fPdgParticle1);
            vector <TVector3> pionTrackPoints = KalmanTrackPropagation(track2, fPdgParticle2);

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

            Double_t zPartOrigDeath = (pointsAndMinDist[0] + pointsAndMinDist[2]) / 2.0;
            Double_t xPartOrigDeath = (pointsAndMinDist[1] + pointsAndMinDist[3]) / 2.0;
            
            V0Z[0] = Min(pointsAndMinDist[0], pointsAndMinDist[2]); // V0ZX
            delete [] pointsAndMinDist;

            // Go to secondary vertex V0
            FairTrackParam proton_V0[nV0], pion_V0[nV0];
            vector <Double_t> geomTopology[nV0];
            // Description of vector:
            // Distance beetween Vp and Vp_prot_extrap   [0]
            // Distance beetween Vp and Vp_prot_extrap   [1]
            // Distance between proton and pion at V0    [2]
            // Distance between V0 and Vp along beamline [3]

            for (Int_t iProj = 0; iProj < nV0; iProj++) {
                proton_V0[iProj] = KalmanTrackPropagation(track1, fPdgParticle1, V0Z[iProj]);
                pion_V0[iProj] = KalmanTrackPropagation(track2, fPdgParticle2, V0Z[iProj]);
                geomTopology[iProj] = GeomTopology(proton_V0[iProj], pion_V0[iProj], proton_Vp, pion_Vp);
            }
            
            Double_t protonPz = 1.0 / proton_V0[0].GetQp() / Sqrt(Sqr(proton_V0[0].GetTx()) + Sqr(proton_V0[0].GetTy()) + 1.0);
            Double_t pionPz = 1.0 / pion_V0[0].GetQp() / Sqrt(Sqr(pion_V0[0].GetTx()) + Sqr(pion_V0[0].GetTy()) + 1.0);
            Double_t protonPx = proton_V0[0].GetTx() * protonPz;
            Double_t pionPx = pion_V0[0].GetTx() * pionPz;
            
            Double_t txPartOrig = (protonPx + pionPx) / (protonPz + pionPz);
            
            Double_t PartOrigBX = txPartOrig * (Vpz - zPartOrigDeath) + xPartOrigDeath;

            BmnParticlePair* partPair = new((*fParticlePair)[fParticlePair->GetEntriesFast()]) BmnParticlePair();
            partPair->SetPartOrigB(PartOrigBX, 0.0); //FIXME
            partPair->SetV0XZ(V0Z[0]);
            partPair->SetV0YZ(V0Z[1]);

            partPair->SetDCA1(geomTopology[0].at(0));
            partPair->SetDCA2(geomTopology[0].at(1));
            partPair->SetDCA12(geomTopology[0].at(2), geomTopology[1].at(2));
            partPair->SetPath(geomTopology[0].at(3), geomTopology[1].at(3));

            partPair->SetMomPair(_p1, _p2);
            partPair->SetEtaPair(_eta1, _eta2);

            // Track params. are redefined
            Double_t Tx1[nV0], Ty1[nV0], Tx2[nV0], Ty2[nV0], p1[nV0], p2[nV0];
            Double_t A1[nV0], A2[nV0];

            TVector2 armenPodol[nV0];

            for (Int_t iProj = 0; iProj < nV0; iProj++) {
                Tx1[iProj] = proton_V0[iProj].GetTx();
                Ty1[iProj] = proton_V0[iProj].GetTy();
                Tx2[iProj] = pion_V0[iProj].GetTx();
                Ty2[iProj] = pion_V0[iProj].GetTy();
                p1[iProj] = 1. / proton_V0[iProj].GetQp();
                p2[iProj] = 1. / pion_V0[iProj].GetQp();

                armenPodol[iProj] = ArmenterosPodol(proton_V0[iProj], pion_V0[iProj]);

                A1[iProj] = 1. / Sqrt(Tx1[iProj] * Tx1[iProj] + Ty1[iProj] * Ty1[iProj] + 1);
                lPos[iProj].SetXYZM(Tx1[iProj] * A1[iProj] * p1[iProj], Ty1[iProj] * A1[iProj] * p1[iProj], p1[iProj] * A1[iProj],
                        fPDG->GetParticle(fPdgParticle1)->Mass());

                p2[iProj] *= -1.; // Since in the calculations pos. mom. values should be used

                A2[iProj] = 1. / Sqrt(Tx2[iProj] * Tx2[iProj] + Ty2[iProj] * Ty2[iProj] + 1);
                lNeg[iProj].SetXYZM(Tx2[iProj] * A2[iProj] * p2[iProj], Ty2[iProj] * A2[iProj] * p2[iProj], p2[iProj] * A2[iProj],
                        fPDG->GetParticle(fPdgParticle2)->Mass());
            }
            partPair->SetAlpha(armenPodol[0].X(), armenPodol[1].X());
            partPair->SetPtPodol(armenPodol[0].Y(), armenPodol[1].Y());
            partPair->SetInvMass(TLorentzVector((lPos[0] + lNeg[0])).Mag(), TLorentzVector((lPos[1] + lNeg[1])).Mag());

            if (fGlobalMatches) {
                BmnGlobalTrack* glTr1 = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(iTrack);
                BmnGlobalTrack* glTr2 = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(jTrack);

                CbmMCTrack* mcTr1 = (CbmMCTrack*) fMCTracks->UncheckedAt(recoToMcIdx(iTrack));
                CbmMCTrack* mcTr2 = (CbmMCTrack*) fMCTracks->UncheckedAt(recoToMcIdx(jTrack));

                Double_t pGlTr1 = 1. / glTr1->GetParamFirst()->GetQp();
                Double_t pGlTr2 = 1. / glTr2->GetParamFirst()->GetQp();

                Int_t nHits1 = glTr1->GetNHits();
                Int_t nHits2 = glTr2->GetNHits();

                Double_t pMcTr1 = mcTr1->GetP();
                Double_t pMcTr2 = mcTr2->GetP();

                Int_t idx = (nHits1 == 4 && nHits2 == 4) ? 0 : (nHits1 == 5 && nHits2 == 5) ? 1 : 2;
                h3Sim[0][idx]->Fill(pGlTr1 - pMcTr1, Abs(pGlTr2) - pMcTr2);

                if (partPair->GetInvMass("X") < fLeftInvMass || partPair->GetInvMass("X") > fRightInvMass)
                    //h3[1][idx]->Fill(pGlTr1 - pMcTr1, Abs(pGlTr2) - pMcTr2);
                    continue;

                // Getting info from a pair we are considering ...
                Double_t Pp = partPair->GetMomPart1();
                Double_t Ppi = partPair->GetMomPart2();
                Double_t Etap = partPair->GetEtaPart1();
                Double_t Etapi = partPair->GetEtaPart2();

                Double_t dca1 = partPair->GetDCA1();
                Double_t dca2 = partPair->GetDCA2();
                Double_t dca12 = partPair->GetDCA12("X");
                Double_t path = partPair->GetPath("X");

                // Kin. cuts applied ...
                if (Pp < fMom[0][0] || Pp > fMom[0][1] || Ppi < fMom[1][0] || Ppi > fMom[1][1])
                    continue;

                if (Etap < fEta[0][0] || Etap > fEta[0][1] || Etapi < fEta[1][0] || Etapi > fEta[1][1])
                    continue;

                // Geom. cuts applied ...
                if (dca1 < fDCA[0][0] || dca1 > fDCA[0][1] || dca2 < fDCA[1][0] || dca2 > fDCA[1][1])
                    continue;

                if (dca12 < fDCA12[0] || dca12 > fDCA12[1])
                    continue;

                if (path < fPath[0] || path > fPath[1])
                    continue;

                // 1d-histos ...
                // hReco[0]->Fill(PLambda);
                // hReco[1]->Fill(etaLambda);
                hReco[2]->Fill(Pp);
                hReco[3]->Fill(Ppi);
                hReco[4]->Fill(Etap);
                hReco[5]->Fill(Etapi);
                hReco[6]->Fill(nHits1);
                hReco[7]->Fill(nHits2);

                // 2d-histos ...
                // h2Reco[0]->Fill(etaLambda, PLambda);
                h2Reco[1]->Fill(Etap, Pp);
                h2Reco[2]->Fill(Etapi, Ppi);
                h2Reco[3]->Fill(Pp, Ppi);
                h2Reco[4]->Fill(Etap, Etapi);
                h2Reco[5]->Fill(glTr1->GetParamFirst()->GetTx(), glTr2->GetParamFirst()->GetTx());
                h2Reco[6]->Fill(glTr1->GetParamFirst()->GetTy(), glTr2->GetParamFirst()->GetTy());
                //cout << Pp << " " <<  Ppi << " " << Etap << " " << Etapi << " " << dca1 << " " << dca2 << " " << dca12 << " " << path << " " << nHits1 << " " << nHits2 << 
                //        " Pz_{p} = " << pGlTr1 / Sqrt(1 + (glTr1->GetParamFirst()->GetTx()) * (glTr1->GetParamFirst()->GetTx()) + (glTr1->GetParamFirst()->GetTy()) * (glTr1->GetParamFirst()->GetTy())) << 
                //        " Pz_{pi} = " << pGlTr2 / Sqrt(1 + (glTr2->GetParamFirst()->GetTx()) * (glTr2->GetParamFirst()->GetTx()) + (glTr2->GetParamFirst()->GetTy()) * (glTr2->GetParamFirst()->GetTy())) << endl; //getchar();
            }
        }
    }
}
// -------------------------------------------------------------------

InitStatus BmnTwoParticleDecay::Init() {
    cout << "\nBmnTwoParticleDecay::Init()" << endl;
    // Read current geometry (RunSpring2017) from database 
    Char_t* geoFileName = (Char_t*) "current_geo_file.root";
    Int_t res_code = UniDbRun::ReadGeometryFile(fRunPeriod, fRunId, geoFileName);
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-1);
    }
    TGeoManager::Import(geoFileName);

    // Get run info..
    UniDbRun* runInfo = UniDbRun::GetRun(fRunPeriod, fRunId);
    if (!runInfo) {
        cout << "Something is wrong when getting run info from DB..." << endl;
        throw;
    }

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

    fBranchGemPoints = "StsPoint";
    fBranchGlobalTracks = "BmnGlobalTrack";
    fBranchMCTracks = "MCTrack";
    fBranchGlobalMatch = "BmnGlobalTrackMatch";
    fBranchVertex = "BmnVertex";

    fGemPoints = (TClonesArray*) ioman->GetObject(fBranchGemPoints.Data());
    fGlobalTracks = (TClonesArray*) ioman->GetObject(fBranchGlobalTracks.Data());
    fMCTracks = (TClonesArray*) ioman->GetObject(fBranchMCTracks.Data());
    fGlobalMatches = (TClonesArray*) ioman->GetObject(fBranchGlobalMatch.Data());
    fVertex = (TClonesArray*) ioman->GetObject(fBranchVertex.Data());

    fParticlePair = new TClonesArray("BmnParticlePair");
    ioman->Register("ParticlePair", "Lambda", fParticlePair, kTRUE);

    fPDG = TDatabasePDG::Instance();

    fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    fMagField->SetScale((!fUseMc) ? *runInfo->GetFieldVoltage() / 55.87 : 1.3);
    fMagField->Init();

    FairRunAna::Instance()->SetField(fMagField);
    fField = FairRunAna::Instance()->GetField();
    fKalman = new BmnKalmanFilter();

    fPdgParticle1 = fPDG1;
    fPdgParticle2 = fPDG2;
    cout << "PDG, particle1 = " << fPdgParticle1 << endl;
    cout << "PDG, particle2 = " << fPdgParticle2 << endl;

    // Possible two-particle decays are listed here (lambda0, K0-short):
    fPDGDecay = (fPDG1 == 2212 && fPDG2 == -211) ? 3122 :
            (fPDG1 == 211 && fPDG2 == -211) ? 310 : -1;

    fLeftInvMass = (fPDG1 == 2212 && fPDG2 == -211) ? 1.11 : -1; // FIXME (for K0-short)
    fRightInvMass = (fPDG1 == 2212 && fPDG2 == -211) ? 1.12 : -1;

    if (fUseMc && fGlobalMatches) {
        fN = 8;
        hSim = new TH1F*[fN]();
        hReco = new TH1F*[fN]();

        fN2 = 7;
        h2Sim = new TH2F*[fN2]();
        h2Reco = new TH2F*[fN2]();

        fN3 = 2; // [0] --> all pairs, [1] --> pairs from lambda 
        fN4 = 3; // [0] --> 4 hits, [1] --> 5 hits, [2] --> 6 hits per track

        h3Sim = new TH2F**[fN3]();

        const Int_t _fN = 8;
        const Int_t _fN2 = 7;
        const Int_t _fN3 = 2;
        const Int_t _fN4 = 3;

        TString hTitles[_fN];
        TString hTitles2[_fN2];
        TString hTitles3[_fN3][_fN4];

        TString hNames[_fN] = {
            "Distr. over P_{#Lambda^{0}}", "Distr. over #eta_{#Lambda^{0}}",
            "Distr. over P_{p}", "Distr. over P_{#pi^{-}}",
            "Distr. over #eta_{p}", "Distr. over #eta_{#pi^{-}}",
            "Distr. over NHits_{p}", "Distr. over NHits_{#pi^{-}}"
        };

        TString hNames2[_fN2] = {
            "Distr. over P_{#Lambda^{0}} vs. #eta_{#Lambda^{0}}",
            "Distr. over P_{p} vs. #eta_{p}",
            "Distr. over P_{#pi^{-}} vs. #eta_{#pi^{-}}",
            "Distr. over P_{p} vs. P_{#pi^{-}}",
            "Distr. over #eta_{p} vs. #eta_{#pi^{-}}",
            "Distr. over T_{x}^{p} vs. T_{x}^{#pi^{-}}",
            "Distr. over T_{y}^{p} vs. T_{y}^{#pi^{-}}"
        };

        TString hNames3[_fN3][_fN4];
        for (Int_t i = 0; i < _fN3; i++) {
            TString addI = (i == 0) ? "all pairs " : "pairs from #Lambda^{0} ";
            for (Int_t j = 0; j < _fN4; j++) {
                TString addJ = (j == 0) ? "4 hits" : (j == 1) ? "5 hits" : "6 hits";
                hNames3[i][j] = "Diff. between reco and sim. momenta, " + addI + addJ + " per both tracks";
            }
        }

        TString xTitles[_fN] = {
            "P_{#Lambda^{0}} [GeV/c]", "#eta_{#Lambda^{0}}",
            "P_{p} [GeV/c]", "P_{#pi^{-}} [GeV/c]",
            "#eta_{p}", "#eta_{#pi^{-}}", "NHits_{p}", "NHits_{#pi^{-}}"
        };

        TString xTitles2[_fN2] = {
            "#eta_{#Lambda^{0}}", "#eta_{p}", "#eta_{#pi^{-}}", "P_{p} [GeV/c]", "#eta_{p}", "T_{x}^{p}", "T_{y}^{p}"
        };

        TString yTitles2[_fN2] = {
            "P_{#Lambda^{0}} [GeV/c]", "P_{p} [GeV/c]", "P_{#pi^{-}} [GeV/c]", "P_{#pi^{-}} [GeV/c]", "#eta_{#pi^{-}}", "T_{x}^{#pi^{-}}", "T_{y}^{#pi^{-}}"
        };

        for (Int_t iHist = 0; iHist < fN; iHist++) {
            hTitles[iHist] = hNames[iHist];
            hSim[iHist] = new TH1F(TString("SIM " + hNames[iHist]).Data(), TString("SIM " + hTitles[iHist]).Data(), 100, 0., 0.);
            hReco[iHist] = new TH1F(TString("RECO " + hNames[iHist]).Data(), TString("RECO " + hTitles[iHist]).Data(), 100, 0., 0.);
            hSim[iHist]->GetXaxis()->SetTitle(xTitles[iHist].Data());
            hReco[iHist]->GetXaxis()->SetTitle(xTitles[iHist].Data());
        }

        for (Int_t iHist = 0; iHist < fN2; iHist++) {
            hTitles2[iHist] = hNames2[iHist];
            Double_t xLow = (iHist == 5 || iHist == 6) ? -1. : 0.;
            Double_t xUp = (iHist == 5 || iHist == 6) ? +1. : 6.;
            h2Sim[iHist] = new TH2F(TString("SIM " + hNames2[iHist]).Data(), TString("SIM " + hTitles2[iHist]).Data(), 100, xLow, xUp, 100, xLow, xUp);
            h2Reco[iHist] = new TH2F(TString("RECO " + hNames2[iHist]).Data(), TString("RECO " + hTitles2[iHist]).Data(), 100, xLow, xUp, 100, xLow, xUp);
            h2Sim[iHist]->GetXaxis()->SetTitle(xTitles2[iHist].Data());
            h2Reco[iHist]->GetXaxis()->SetTitle(xTitles2[iHist].Data());
            h2Sim[iHist]->GetYaxis()->SetTitle(yTitles2[iHist].Data());
            h2Reco[iHist]->GetYaxis()->SetTitle(yTitles2[iHist].Data());
        }

        for (Int_t iHist = 0; iHist < fN3; iHist++) {
            h3Sim[iHist] = new TH2F*[fN4]();
            for (Int_t jHist = 0; jHist < fN4; jHist++) {
                hTitles3[iHist][jHist] = hNames3[iHist][jHist];
                h3Sim[iHist][jHist] = new TH2F(TString("SIM/RECO " + hNames3[iHist][jHist]).Data(), TString("SIM/RECO " + hTitles3[iHist][jHist]).Data(), 100, -1., 1., 100, -1., 1.);
                h3Sim[iHist][jHist]->GetXaxis()->SetTitle("Diff. P_{p} [GeV/c]");
                h3Sim[iHist][jHist]->GetYaxis()->SetTitle("Diff. P_{#pi^{-}} [GeV/c]");
            }
        }
    }
    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnTwoParticleDecay::Exec(Option_t* option) {
    fParticlePair->Delete();

    fEventCounter++;
    if (fEventCounter % 1000 == 0)
        cout << fEventCounter << endl;

    // In case of MC-data one has to extract coordinates of Vp known exactly ...
    if (fUseMc) {
        for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
            CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->UncheckedAt(iTrack);
            if (mcTrack->GetMotherId() != -1)
                continue;
            fMcVertex.SetXYZ(mcTrack->GetStartX(), mcTrack->GetStartY(), mcTrack->GetStartZ());
            break;
        }
        if (fGlobalMatches)
            CalculateReconstuctableParticles();
    }// Real data .. 
    else {
        fEventVertex = (CbmVertex*) fVertex->UncheckedAt(0);

        if (fEventVertex->GetNTracks() < 2 || fEventVertex->GetNTracks() > 20) //FIXME!
            return;

        TVector3 roughVert(fEventVertex->GetRoughX(), fEventVertex->GetRoughY(), fEventVertex->GetRoughZ());
        TVector3 realVert(fEventVertex->GetX(), fEventVertex->GetY(), fEventVertex->GetZ());

        const Double_t vertexCut = 100.;

        for (Int_t iProj = 0; iProj < 3; iProj++)
            if (Abs(TVector3(roughVert - realVert)[iProj]) > vertexCut)
                return;
    }
    Analysis();
}
// -------------------------------------------------------------------

void BmnTwoParticleDecay::Finish() {
    if (fUseMc) {
        //        for (Int_t iHist = 0; iHist < fN; iHist++) {
        //            if (hSim[iHist]->GetEntries() > 0)
        //                hSim[iHist]->Write();
        //            if (hReco[iHist]->GetEntries() > 0)
        //                hReco[iHist]->Write();
        //        }
        for (Int_t iHist = 0; iHist < fN2; iHist++) {
            //if (h2Sim[iHist]->GetEntries() > 0)
            h2Sim[iHist]->Write();
            //  if (h2Reco[iHist]->GetEntries() > 0)
            h2Reco[iHist]->Write();
        }
        //        for (Int_t iHist = 0; iHist < fN3; iHist++)
        //            for (Int_t jHist = 0; jHist < fN4; jHist++)
        //                if (h3Sim[iHist][jHist]->GetEntries() > 0)
        //                    h3Sim[iHist][jHist]->Write();
    }
    delete fKalman;
    delete fMagField;
    cout << "\n-I- [BmnTwoParticleDecay::Finish] " << endl;
}

TVector3 BmnTwoParticleDecay::FitParabola(vector <TVector3> points) {
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

    delete tmp;

    if (!std::isnan(a * b * c) && !std::isinf(a * b * c))
        return TVector3(a, b, c);
    else
        return TVector3(0., 0., 0.);
}

TVector2 BmnTwoParticleDecay::SecondaryVertexY(FairTrackParam* param1, FairTrackParam* param2) {
    Double_t Ty1 = param1->GetTy();
    Double_t b1 = param1->GetY() - Ty1 * param1->GetZ();

    Double_t Ty2 = param2->GetTy();
    Double_t b2 = param2->GetY() - Ty2 * param2->GetZ();

    Double_t zV = (b2 - b1) / (Ty1 - Ty2);
    Double_t yV = Ty1 * zV + b1;

    return TVector2(yV, zV);
}

void BmnTwoParticleDecay::CalculateMinDistance(TVector3 paramsCurv1, TVector3 paramsCurv2, Double_t* arr) {
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
        Double_t zStart = 50.; // fDetector->GetGemStation(0)->GetZPosition();
        Double_t zFinish = -50.; // fEventVertex->GetRoughZ();
        Double_t Zi = zStart;
        Double_t Zj = zStart;

        while (Zi > zFinish) {
            Double_t Xi = a1 * Zi * Zi + b1 * Zi + c1;
            while (Zj > zFinish) {
                Double_t Xj = a2 * Zj * Zj + b2 * Zj + c2;
                Double_t dist = Dist(Zi, Xi, Zj, Xj); // taken from BmnMath.h
                if (dist < arr[4]) {
                    arr[0] = Zi;
                    arr[1] = Xi;
                    arr[2] = Zj;
                    arr[3] = Xj;
                    arr[4] = dist;
                }
                Zj -= step;
            }
            Zi -= step;
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

TVector2 BmnTwoParticleDecay::ArmenterosPodol(FairTrackParam prot, FairTrackParam pion) {
    Float_t mom1 = 1. / prot.GetQp();
    Float_t Tx1 = prot.GetTx();
    Float_t Ty1 = prot.GetTy();

    Float_t mom1sq = mom1 * mom1;
    Float_t Pz1 = Abs(mom1) / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
    Float_t Px1 = Pz1 * Tx1;
    Float_t Py1 = Pz1 * Ty1;

    Float_t mom2 = 1. / pion.GetQp();
    Float_t Tx2 = pion.GetTx();
    Float_t Ty2 = pion.GetTy();

    Float_t mom2sq = mom2 * mom2;
    Float_t Pz2 = Abs(mom2) / Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
    Float_t Px2 = Pz2 * Tx2;
    Float_t Py2 = Pz2 * Ty2;

    Float_t momHyp2 = (Px1 + Px2) * (Px1 + Px2) + (Py1 + Py2) * (Py1 + Py2) + (Pz1 + Pz2) * (Pz1 + Pz2);
    Float_t momHyp = Sqrt(momHyp2);
    Float_t oneOver2MomHyp = 1 / (2 * momHyp);
    Float_t L1 = (momHyp2 + mom1sq - mom2sq) * oneOver2MomHyp;
    Float_t L2 = (momHyp2 + mom2sq - mom1sq) * oneOver2MomHyp;
    Float_t alpha = (L1 - L2) / (L1 + L2);
    Float_t Pt = Sqrt((mom1sq + mom2sq + momHyp2) * (mom1sq + mom2sq + momHyp2) - 2 * (mom1sq * mom1sq + mom2sq * mom2sq + momHyp2 * momHyp2)) * oneOver2MomHyp;

    return TVector2(alpha, Pt);
}


ClassImp(BmnTwoParticleDecay);
