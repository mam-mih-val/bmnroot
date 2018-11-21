// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-12-27

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnTwoParticleDecay                                                        //
//                                                                            //
//  A supplementary class for two-body decay reconstruction                   //
//                                                                            //
//////////  //////////////////////////////////////////////////////////////////////
#include <TCanvas.h>
#include <TGeoManager.h>
#include <Fit/FitResult.h>
#include "BmnTwoParticleDecay.h"
#include "BmnParticlePairsInfo.h"

BmnTwoParticleDecay::BmnTwoParticleDecay(BmnGemStripConfiguration::GEM_CONFIG config) :
// Particles set by default:
fPDG1(2212), // proton
fPDG2(-211), // pion
fPdgParticle1(fPDG1),
fPdgParticle2(fPDG2),
fEventCounter(0),
fGeometry(config),
fKalman(nullptr),
fField(nullptr),
fParticlePair_MC_noCuts(nullptr),
fParticlePair_MC_withCuts(nullptr),
fParticlePair_RECO_noCuts(nullptr),
fParticlePair_RECO_withCuts(nullptr),
fParticlePair(nullptr),
fVertex(nullptr),
fIsUseRealVertex(kFALSE),
fGlobalMatches(nullptr),
fParticlePairsInfo(nullptr) {
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

    if (config == BmnGemStripConfiguration::GEM_CONFIG::RunSpring2018) {
        fRunPeriod = 7;
        fRunId = 4629;
    } else if (config == BmnGemStripConfiguration::GEM_CONFIG::RunSpring2017) {
        fRunPeriod = 6;
        fRunId = 1209;
    } else {
        cout << "BmnGemStripConfiguration not defined !!!" << endl;
        throw;
    }

    // Create GEM detector ------------------------------------------------------

    TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");
    gPathGemConfig += "/gem/XMLConfigs/";
    // Create GEM detector ------------------------------------------------------
    switch (fGeometry) {
        case BmnGemStripConfiguration::RunSpring2017:
            fDetector = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2017.xml");
            cout << "   Current Configuration : RunSpring2017" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2018:
            fDetector = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2018.xml");
            cout << "   Current Configuration : RunSpring2018" << "\n";
            break;

        default:
            fDetector = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2018.xml");
            cout << "   Current Configuration : RunSpring2018" << "\n";
            break;
    }
}

BmnTwoParticleDecay::~BmnTwoParticleDecay() {
    delete fDetector;
}

vector <Double_t> BmnTwoParticleDecay::GeomTopology(FairTrackParam proton_V0, FairTrackParam pion_V0, FairTrackParam proton_Vp, FairTrackParam pion_Vp) {
    Double_t X = 0., Y = 0., Z = 0.;

    // evetest.root -->
    Bool_t isMC = fAnalType[0].Contains("eve") && !fAnalType[0].Contains("dst"); // only MC
    if (isMC) {
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

        if (TrackID != iTrack || gemPoint->GetZIn() > fDetector->GetGemStation(0)->GetZPosition() + 5.) //FIXME
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

    TClonesArray* arr = (fAnalType[1].Contains("ON") || fAnalType[0].Contains("dst")) ? fGlobalTracks : fMCTracks;
    Bool_t isMC = fAnalType[0].Contains("eve") && !fAnalType[0].Contains("dst"); // only MC

    for (Int_t iTrack = 0; iTrack < arr->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack Track1;
        BmnGlobalTrack* track1 = &Track1;

        if (isMC) {
            TParticlePDG* particle1 = fPDG->GetParticle(((CbmMCTrack*) arr->UncheckedAt(iTrack))->GetPdgCode());
            if (!particle1) {
                continue;
            }
            Double_t Q1 = particle1->Charge();
            if (!(Q1 > 0)) {
                continue;

            }
            FindFirstPointOnMCTrack(iTrack, track1, CheckSign(Q1));
        } else
            track1 = (BmnGlobalTrack*) arr->UncheckedAt(iTrack);


        Double_t _p1, _eta1;
        if (!CheckTrack(track1, fPdgParticle1, _p1, _eta1))
            continue;

        for (Int_t jTrack = 0; jTrack < arr->GetEntriesFast(); jTrack++) {
            if (iTrack == jTrack)
                continue;

            BmnGlobalTrack Track2;
            BmnGlobalTrack* track2 = &Track2;

            if (isMC) {
                TParticlePDG* particle2 = fPDG->GetParticle(((CbmMCTrack*) arr->UncheckedAt(jTrack))->GetPdgCode());
                if (!particle2)
                    continue;
                Double_t Q2 = particle2->Charge();
                if (!(Q2 < 0))
                    continue;

                FindFirstPointOnMCTrack(jTrack, track2, CheckSign(Q2));
            } else
                track2 = (BmnGlobalTrack*) arr->UncheckedAt(jTrack);

            Double_t _p2, _eta2;
            if (!CheckTrack(track2, fPdgParticle2, _p2, _eta2))
                continue;

            // Array to store V0XZ and V0YZ ...           
            Double_t V0Z[nV0] = {0., 0.}; // V0XZ and V0YZ

            // Go to primary vertex Vp
            Double_t Vpz = isMC ? fMcVertex.Z() : fIsUseRealVertex ? fEventVertex->GetZ() : fEventVertex->GetRoughZ();

            FairTrackParam proton_Vp = KalmanTrackPropagation(track1, fPdgParticle1, Vpz);
            FairTrackParam pion_Vp = KalmanTrackPropagation(track2, fPdgParticle2, Vpz);

            // Calculate V0YZ the YZ-projection ...
            V0Z[1] = SecondaryVertexY(track1->GetParamFirst(), track2->GetParamFirst()).Y(); // V0ZY

            // FIXME!!! Now it does not work correctly!
            /*
            Double_t V0Z = FindV0ByVirtualPlanes(track1, track2, .5 * (Vpz + fDetector->GetGemStation(0)->GetZPosition()));
            if (V0Z < -999.)
                continue;
            */

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

            // Double_t zPartOrigDeath = (pointsAndMinDist[0] + pointsAndMinDist[2]) / 2.0;
            // Double_t xPartOrigDeath = (pointsAndMinDist[1] + pointsAndMinDist[3]) / 2.0;

            V0Z[0] = Min(pointsAndMinDist[0], pointsAndMinDist[2]); // V0ZX
            delete [] pointsAndMinDist;

            // Go to secondary vertex V0
            FairTrackParam proton_V0[nV0], pion_V0[nV0];
            // FairTrackParam proton_V0, pion_V0;
            vector <Double_t> geomTopology[nV0];
            // Description of vector:
            // Distance between Vp and Vp_prot_extrap    [0]
            // Distance between Vp and Vp_prot_extrap    [1]
            // Distance between proton and pion at V0    [2]
            // Distance between V0 and Vp along beamline [3]

            for (Int_t iProj = 0; iProj < nV0; iProj++) {
                proton_V0[iProj] = KalmanTrackPropagation(track1, fPdgParticle1, V0Z[iProj]);
                pion_V0[iProj] = KalmanTrackPropagation(track2, fPdgParticle2, V0Z[iProj]);
                geomTopology[iProj] = GeomTopology(proton_V0[iProj], pion_V0[iProj], proton_Vp, pion_Vp);
            }

            // Double_t protonPz = 1.0 / proton_V0[0].GetQp() / Sqrt(Sqr(proton_V0[0].GetTx()) + Sqr(proton_V0[0].GetTy()) + 1.0);
            // Double_t pionPz = 1.0 / pion_V0[0].GetQp() / Sqrt(Sqr(pion_V0[0].GetTx()) + Sqr(pion_V0[0].GetTy()) + 1.0);
            // Double_t protonPx = proton_V0[0].GetTx() * protonPz;
            // Double_t pionPx = pion_V0[0].GetTx() * pionPz;

            // Double_t txPartOrig = (protonPx + pionPx) / (protonPz + pionPz);
            // Double_t PartOrigBX = txPartOrig * (Vpz - zPartOrigDeath) + xPartOrigDeath;

            BmnParticlePair partPair;

            //            partPair.SetPartOrigB(PartOrigBX, 0.0); //FIXME
            partPair.SetV0XZ(V0Z[0]);
            partPair.SetV0YZ(V0Z[1]);

            partPair.SetDCA1(geomTopology[0].at(0));
            partPair.SetDCA2(geomTopology[0].at(1));
            partPair.SetDCA12(geomTopology[0].at(2), geomTopology[1].at(2));
            partPair.SetPath(geomTopology[0].at(3), geomTopology[1].at(3));

            partPair.SetMomPair(_p1, _p2);
            partPair.SetEtaPair(_eta1, _eta2);

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
            partPair.SetAlpha(armenPodol[0].X(), armenPodol[1].X());
            partPair.SetPtPodol(armenPodol[0].Y(), armenPodol[1].Y());
            partPair.SetInvMass(TLorentzVector((lPos[0] + lNeg[0])).Mag(), TLorentzVector((lPos[1] + lNeg[1])).Mag());

            // To be used for real exp. data
            if (fAnalType[0].Contains("dst") && !fAnalType[0].Contains("eve") && fAnalType[1].Contains("OFF"))
                new((*fParticlePair)[fParticlePair->GetEntriesFast()]) BmnParticlePair(partPair);

            // MC input, no matches
            if (isMC && fAnalType[1].Contains("OFF")) {
                Double_t Pp = partPair.GetMomPart1();
                Double_t Ppi = partPair.GetMomPart2();
                Double_t Etap = partPair.GetEtaPart1();
                Double_t Etapi = partPair.GetEtaPart2();

                Double_t dca1 = partPair.GetDCA1();
                Double_t dca2 = partPair.GetDCA2();
                Double_t dca12 = partPair.GetDCA12("X");
                Double_t path = partPair.GetPath("X");

                partPair.SetMCTrackIdPart1(iTrack);
                partPair.SetMCTrackIdPart2(jTrack);

                new((*fParticlePair_MC_noCuts)[fParticlePair_MC_noCuts->GetEntriesFast()]) BmnParticlePair(partPair);

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

                new((*fParticlePair_MC_withCuts)[fParticlePair_MC_withCuts->GetEntriesFast()]) BmnParticlePair(partPair);
            }

            // Reco input with matches
            if (fAnalType[1].Contains("ON")) {
                // Getting info from a pair we are considering ...
                Double_t Pp = partPair.GetMomPart1();
                Double_t Ppi = partPair.GetMomPart2();
                Double_t Etap = partPair.GetEtaPart1();
                Double_t Etapi = partPair.GetEtaPart2();

                Double_t dca1 = partPair.GetDCA1();
                Double_t dca2 = partPair.GetDCA2();
                Double_t dca12 = partPair.GetDCA12("X");
                Double_t path = partPair.GetPath("X");

                new((*fParticlePair_RECO_noCuts)[fParticlePair_RECO_noCuts->GetEntriesFast()]) BmnParticlePair(partPair);

                //                 Kin. cuts applied ...
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

                partPair.SetMCTrackIdPart1(recoToMcIdx(iTrack));
                partPair.SetMCTrackIdPart2(recoToMcIdx(jTrack));

                partPair.SetRecoTrackIdPart1(iTrack);
                partPair.SetRecoTrackIdPart2(jTrack);

                new((*fParticlePair_RECO_withCuts)[fParticlePair_RECO_withCuts->GetEntriesFast()]) BmnParticlePair(partPair);
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

    TString dataSet = (fMCTracks && fGlobalTracks) ? "eve + dst" :
            (fMCTracks && !fGlobalTracks) ? "eve" :
            (!fMCTracks && fGlobalTracks) ? "dst" : "";

    TString isMatching = fGlobalMatches ? "matchON" : "matchOFF";

    fAnalType.push_back(dataSet);
    fAnalType.push_back(isMatching);

    // Particle pair branch for all data types
    const Char_t* className = "BmnParticlePair";

    Bool_t isWriteEveBranch = (dataSet.Contains("eve") && isMatching.Contains("OFF")) ? kTRUE : kFALSE;
    Bool_t isWriteDstBranch = (dataSet.Contains("dst") && isMatching.Contains("ON")) ? kTRUE : kFALSE;
    Bool_t isWriteBranch = (dataSet.Contains("dst") && isMatching.Contains("OFF")) ? kTRUE : kFALSE; // exp. data or dst without matches

    fParticlePair_MC_noCuts = new TClonesArray(className);
    ioman->Register("ParticlePair_MC_noCuts", "Lambda", fParticlePair_MC_noCuts, isWriteEveBranch);

    fParticlePair_MC_withCuts = new TClonesArray(className);
    ioman->Register("ParticlePair_MC_withCuts", "Lambda", fParticlePair_MC_withCuts, isWriteEveBranch);

    fParticlePair_RECO_noCuts = new TClonesArray(className);
    ioman->Register("ParticlePair_RECO_noCuts", "Lambda", fParticlePair_RECO_noCuts, isWriteDstBranch);

    fParticlePair_RECO_withCuts = new TClonesArray(className);
    ioman->Register("ParticlePair_RECO_withCuts", "Lambda", fParticlePair_RECO_withCuts, isWriteDstBranch);

    fParticlePair = new TClonesArray(className);
    ioman->Register("ParticlePair", "Lambda", fParticlePair, isWriteBranch);

    fParticlePairsInfo = new TClonesArray("BmnParticlePairsInfo");
    ioman->Register("ParticlePairsInfo", "Lambda", fParticlePairsInfo, kTRUE);

    // Save pair info ...
    BmnParticlePairsInfo pairInfo;

    pairInfo.setMomPart1Min(fMom[0][0]);
    pairInfo.setMomPart1Max(fMom[0][1]);
    pairInfo.setMomPart2Min(fMom[1][0]);
    pairInfo.setMomPart2Max(fMom[1][1]);

    pairInfo.setEtaPart1Min(fEta[0][0]);
    pairInfo.setEtaPart1Max(fEta[0][1]);
    pairInfo.setEtaPart2Min(fEta[1][0]);
    pairInfo.setEtaPart2Max(fEta[1][1]);

    pairInfo.setDCAPart1Min(fDCA[0][0]);
    pairInfo.setDCAPart1Max(fDCA[0][1]);
    pairInfo.setDCAPart2Min(fDCA[1][0]);
    pairInfo.setDCAPart2Max(fDCA[1][1]);

    pairInfo.setDCA12Min(fDCA12[0]);
    pairInfo.setDCA12Max(fDCA12[1]);
    pairInfo.setPathMin(fPath[0]);
    pairInfo.setPathMax(fPath[0]);

    new((*fParticlePairsInfo)[fParticlePairsInfo->GetEntriesFast()]) BmnParticlePairsInfo(pairInfo);

    fPDG = TDatabasePDG::Instance();

    fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    fMagField->SetScale(!fAnalType[0].Contains("eve") ? *runInfo->GetFieldVoltage() / 55.87 : 1.3); // FIXME
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

    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnTwoParticleDecay::Exec(Option_t * option) {
    fParticlePair_MC_noCuts->Delete();
    fParticlePair_MC_withCuts->Delete();
    fParticlePair_RECO_noCuts->Delete();
    fParticlePair_RECO_withCuts->Delete();

    fParticlePair->Delete();

    fEventCounter++;
    if (fEventCounter % 100 == 0)
        cout << fEventCounter << endl;

    // In case of MC-data one has to extract coordinates of Vp known exactly ...
    if (fAnalType[0].Contains("eve") && !fAnalType[0].Contains("dst")) {
        for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
            CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->UncheckedAt(iTrack);
            if (mcTrack->GetMotherId() != -1)
                continue;
            fMcVertex.SetXYZ(mcTrack->GetStartX(), mcTrack->GetStartY(), mcTrack->GetStartZ());
            break;
        }
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

TVector2 BmnTwoParticleDecay::SecondaryVertexY(FairTrackParam* param1, FairTrackParam * param2) {
    Double_t Ty1 = param1->GetTy();
    Double_t b1 = param1->GetY() - Ty1 * param1->GetZ();

    Double_t Ty2 = param2->GetTy();
    Double_t b2 = param2->GetY() - Ty2 * param2->GetZ();

    Double_t zV = (b2 - b1) / (Ty1 - Ty2);
    Double_t yV = Ty1 * zV + b1;

    return TVector2(yV, zV);
}

void BmnTwoParticleDecay::CalculateMinDistance(TVector3 paramsCurv1, TVector3 paramsCurv2, Double_t * arr) {
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
    Double_t mom1 = 1. / prot.GetQp();
    Double_t Tx1 = prot.GetTx();
    Double_t Ty1 = prot.GetTy();

    Double_t mom1sq = mom1 * mom1;
    Double_t Pz1 = Abs(mom1) / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
    Double_t Px1 = Pz1 * Tx1;
    Double_t Py1 = Pz1 * Ty1;

    Double_t mom2 = 1. / pion.GetQp();
    Double_t Tx2 = pion.GetTx();
    Double_t Ty2 = pion.GetTy();

    Double_t mom2sq = mom2 * mom2;
    Double_t Pz2 = Abs(mom2) / Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
    Double_t Px2 = Pz2 * Tx2;
    Double_t Py2 = Pz2 * Ty2;

    Double_t momHyp2 = (Px1 + Px2) * (Px1 + Px2) + (Py1 + Py2) * (Py1 + Py2) + (Pz1 + Pz2) * (Pz1 + Pz2);
    Double_t momHyp = Sqrt(momHyp2);
    Double_t oneOver2MomHyp = 1 / (2 * momHyp);
    Double_t L1 = (momHyp2 + mom1sq - mom2sq) * oneOver2MomHyp;
    Double_t L2 = (momHyp2 + mom2sq - mom1sq) * oneOver2MomHyp;
    Double_t alpha = (L1 - L2) / (L1 + L2);
    Double_t Pt = Sqrt((mom1sq + mom2sq + momHyp2) * (mom1sq + mom2sq + momHyp2) - 2 * (mom1sq * mom1sq + mom2sq * mom2sq + momHyp2 * momHyp2)) * oneOver2MomHyp;

    return TVector2(alpha, Pt);
}

// Not tested extensively yet!!!
Double_t BmnTwoParticleDecay::FindV0ByVirtualPlanes(BmnGlobalTrack* track1, BmnGlobalTrack* track2, Double_t z_0, Double_t range) {
    const Int_t nPlanes = 5;

    while (range >= 0.1) {
        Double_t zMax = z_0 + range;
        Double_t zMin = z_0 - range;
        Double_t zStep = (zMax - zMin) / nPlanes;

        Double_t zPlane[nPlanes];
        Double_t Dist[nPlanes];

        FairTrackParam par1 = *(track1->GetParamFirst());
        FairTrackParam par2 = *(track2->GetParamFirst());

        for (Int_t iPlane = 0; iPlane < nPlanes; ++iPlane) {
            zPlane[iPlane] = zMax - iPlane * zStep;
            fKalman->TGeoTrackPropagate(&par1, zPlane[iPlane], 2212, NULL, NULL, kTRUE);
            fKalman->TGeoTrackPropagate(&par2, zPlane[iPlane], 211, NULL, NULL, kTRUE);
            Dist[iPlane] = Sqrt(Sq(par1.GetX() - par2.GetX()) + Sq(par1.GetY() - par2.GetY()));
        }
        //        TCanvas* c = new TCanvas("c1", "c1", 1200, 800);
        //        c->Divide(1, 1);
        //        c->cd(1);
        TGraph* vertex = new TGraph(nPlanes, zPlane, Dist);
        vertex->Fit("pol2", "QF");
        TF1 *fit_func = vertex->GetFunction("pol2");
        Double_t b = fit_func->GetParameter(1);
        Double_t a = fit_func->GetParameter(2);
        Double_t c_ = fit_func->GetParameter(0);

        z_0 = -b / (2 * a);
        Double_t dMin = a * z_0 * z_0 + b * z_0 + c_;
        //        cout << dMin << endl;
        //        if (z_0 < -2.3 || z_0 > 40.) {
        //            delete vertex;
        //            return -1000.;
        //        }
        range /= 2;
        //        vertex->Draw("AP*");
        //        c->SaveAs("tmp.pdf");
        //        getchar(); 

        delete vertex;
    }

    return z_0;
}

ClassImp(BmnTwoParticleDecay);
