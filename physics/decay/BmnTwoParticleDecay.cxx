// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-12-27

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnTwoParticleDecay                                                        //
//                                                                            //
//  A supplementary class for two-body decay reconstruction                   //
//                                                                            //
//////////  ////////////////////////////////////////////////////////////////////
#include <TCanvas.h>
#include "TFile.h"
#include <TGeoManager.h>
#include <Fit/FitResult.h>
#include "BmnTwoParticleDecay.h"
#include "BmnParticlePairsInfo.h"

BmnTwoParticleDecay::BmnTwoParticleDecay(BmnGemStripConfiguration::GEM_CONFIG config, Int_t runId) :
fDstHeader(nullptr),
fPhysInfo(nullptr),
// Particles set by default:
fPDG1(2212), // proton
fPDG2(-211), // pion
fPdgParticle1(fPDG1),
fPdgParticle2(fPDG2),
fEventCounter(0),
fGeometry(config),
fKalman(nullptr),
fField(nullptr),
fParticlePair_MC(nullptr),
fParticlePair_RECO(nullptr),
fParticlePair(nullptr),
fVertex(nullptr),
fGlobalMatches(nullptr),
fIsCbmDst(kFALSE),
fGeomFile(""), fDigiDir(""), fEoSNode("root://ncm.jinr.ru/") {
    fMcVertex.SetXYZ(0., 0., 0.);
    fRunId = runId;

    if (config == BmnGemStripConfiguration::GEM_CONFIG::RunSpring2018)
        fRunPeriod = 7;

    else if (config == BmnGemStripConfiguration::GEM_CONFIG::RunSpring2017)
        fRunPeriod = 6;

    else {
        cout << "BmnGemStripConfiguration not defined !!!" << endl;
        throw;
    }

    // Create GEM detector ------------------------------------------------------

    TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");
    gPathGemConfig += "/parameters/gem/XMLConfigs/";
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
        X = fEventVertex->GetX();
        Y = fEventVertex->GetY();
        Z = fEventVertex->GetZ();
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
    // Double_t vertexDiff = proton_V0.GetZ() - Z;

    vector <Double_t> cuts;
    cuts.push_back(protonVpVp);
    cuts.push_back(pionVpVp);
    cuts.push_back(protonV0PionV0);
    // cuts.push_back(Abs(vertexDiff));

    return cuts;
}

FairTrackParam BmnTwoParticleDecay::KalmanTrackPropagation(BmnGlobalTrack* track, Int_t pdg, Double_t Z) {
    FairTrackParam parPredict = *(track->GetParamFirst());
    fKalman->TGeoTrackPropagate(&parPredict, Z, pdg, nullptr, nullptr, kTRUE);
    // track->SetParamFirst(parPredict);
    return parPredict;
}

BmnStatus BmnTwoParticleDecay::FindFirstPointOnMCTrack(Int_t iTrack, BmnGlobalTrack* track, Int_t sign) {
    FairTrackParam param;
    map <Double_t, FairMCPoint*> firstPoint;

    for (Int_t iPoint = 0; iPoint < fSilPoints->GetEntriesFast(); iPoint++) {
        BmnSiliconPoint* silPoint = (BmnSiliconPoint*) fSilPoints->UncheckedAt(iPoint);
        Int_t TrackID = silPoint->GetTrackID();

        if (TrackID != iTrack)
            continue;

        firstPoint[silPoint->GetZIn()] = silPoint;
    }

    FairMCPoint* point = nullptr;
    if (firstPoint.size() == 0)
        for (Int_t iPoint = 0; iPoint < fGemPoints->GetEntriesFast(); iPoint++) {
            CbmStsPoint* gemPoint = (CbmStsPoint*) fGemPoints->UncheckedAt(iPoint);
            Int_t TrackID = gemPoint->GetTrackID();

            if (TrackID != iTrack)
                continue;

            firstPoint[gemPoint->GetZIn()] = gemPoint;
        }

    point = firstPoint.begin()->second;

    if (!point)
        return kBMNERROR;

    Double_t Px = point->GetPx();
    Double_t Py = point->GetPy();
    Double_t Pz = point->GetPz();
    // cout << point << " " << point->GetZ() << endl;

    param.SetTx(Px / Pz);
    param.SetTy(Py / Pz);
    param.SetQp(sign / Sqrt(Px * Px + Py * Py + Pz * Pz));
    param.SetX(point->GetX());
    param.SetY(point->GetY());
    param.SetZ(point->GetZ());

    if (IsParCorrect(&param, kTRUE)) {
        track->SetParamFirst(param);
        return kBMNSUCCESS;
    } else
        return kBMNERROR;
}

Bool_t BmnTwoParticleDecay::CheckTrack(BmnGlobalTrack* track, Int_t pdgCode, Double_t& mom, Double_t& eta) {
    Double_t Tx = track->GetParamFirst()->GetTx();
    Double_t Ty = track->GetParamFirst()->GetTy();
    Double_t p = 1. / track->GetParamFirst()->GetQp();

    Double_t Pz = Abs(p) / Sqrt(1 + Tx * Tx + Ty * Ty);

    Int_t sign = CheckSign(fPDG->GetParticle(pdgCode)->Charge());

    mom = Abs(p);
    eta = 0.5 * Log((Abs(p) + Pz) / (Abs(p) - Pz));

    if (sign * p < 0)
        return kFALSE;
    else
        return kTRUE;
}

void BmnTwoParticleDecay::Analysis() {
    TLorentzVector lPos, lNeg;

    TClonesArray* arr = (fAnalType[1].Contains("ON") || fAnalType[0].Contains("dst")) ? fGlobalTracks : fMCTracks;
    Bool_t isMC = fAnalType[0].Contains("eve") && !fAnalType[0].Contains("dst"); // only MC

    for (Int_t iTrack = 0; iTrack < arr->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack Track1;
        BmnGlobalTrack* track1 = &Track1;

        if (isMC) {
            TParticlePDG* particle1 = fPDG->GetParticle(((CbmMCTrack*) arr->UncheckedAt(iTrack))->GetPdgCode());
            if (!particle1)
                continue;

            Double_t Q1 = particle1->Charge();
            if (!(Q1 > 0))
                continue;

            if (FindFirstPointOnMCTrack(iTrack, track1, CheckSign(Q1)) == kBMNERROR)
                continue;
        } else
            track1 = (BmnGlobalTrack*) arr->UncheckedAt(iTrack);

        if (fIsCbmDst && track1->GetNHits() < 4)
            continue;

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

                if (FindFirstPointOnMCTrack(jTrack, track2, CheckSign(Q2)) == kBMNERROR)
                    continue;
            } else
                track2 = (BmnGlobalTrack*) arr->UncheckedAt(jTrack);

            if (fIsCbmDst && track2->GetNHits() < 4)
                continue;

            Double_t _p2, _eta2;
            if (!CheckTrack(track2, fPdgParticle2, _p2, _eta2))
                continue;

            // Go to primary vertex Vp
            Double_t Vpz = isMC ? fMcVertex.Z() : fEventVertex->GetZ();
            FairTrackParam proton_Vp = KalmanTrackPropagation(track1, fPdgParticle1, Vpz);
            FairTrackParam pion_Vp = KalmanTrackPropagation(track2, fPdgParticle2, Vpz);

            Double_t V0Z = FindV0ByVirtualPlanes(track1, track2, .5 * (Vpz + fDetector->GetGemStation(0)->GetZPosition()));
            if (V0Z < Vpz - 10. || V0Z > fDetector->GetGemStation(5)->GetZPosition()) // FIXME!
                continue;

            // Go to secondary vertex V0
            // FairTrackParam proton_V0, pion_V0;
            // vector <Double_t> geomTopology;
            // Description of vector:
            // DCA1  --> [0]
            // DCA2  --> [1]
            // DCA12 --> [2]

            // Go to secondary vertex V0
            FairTrackParam proton_V0, pion_V0;
            proton_V0 = KalmanTrackPropagation(track1, fPdgParticle1, V0Z);
            pion_V0 = KalmanTrackPropagation(track2, fPdgParticle2, V0Z);

            Double_t V0X = .5 * (proton_V0.GetX() + pion_V0.GetX());
            Double_t V0Y = .5 * (proton_V0.GetY() + pion_V0.GetY());

            Double_t xCutMin = fDetector->GetStation(0)->GetXMinStation();
            Double_t xCutMax = fDetector->GetStation(0)->GetXMaxStation();
            Double_t yCutMin = fDetector->GetStation(0)->GetYMinStation();
            Double_t yCutMax = fDetector->GetStation(0)->GetYMaxStation();

            if (V0X < xCutMin || V0X > xCutMax || V0Y < yCutMin || V0Y > yCutMax)
                continue;

            vector <Double_t> geomTopology = GeomTopology(proton_V0, pion_V0, proton_Vp, pion_Vp);

            BmnParticlePair partPair;

            // trying to get tof info from tracks ...

            enum {
                tof400 = 1, tof700 = 2
            };
            partPair.SetBeta400Pair(track1->GetBeta(tof400), track2->GetBeta(tof400));
            partPair.SetBeta700Pair(track1->GetBeta(tof700), track2->GetBeta(tof700));

            partPair.SetV0Z(V0Z);
            partPair.SetV0X(V0X);
            partPair.SetV0Y(V0Y);

            partPair.SetDCA1(geomTopology.at(0));
            partPair.SetDCA2(geomTopology.at(1));
            partPair.SetDCA12(geomTopology.at(2));

            TVector3 V0(V0X, V0Y, V0Z);
            TVector3 Vp;
            Vp.SetX(isMC ? fMcVertex.X() : fEventVertex->GetX());
            Vp.SetY(isMC ? fMcVertex.Y() : fEventVertex->GetY());
            Vp.SetZ(isMC ? fMcVertex.Z() : fEventVertex->GetZ());
            // TVector3 Vp(fEventVertex->GetX(), fEventVertex->GetY(), fEventVertex->GetZ());
            Double_t path = TVector3(V0 - Vp).Mag();

            partPair.SetPath(path);

            partPair.SetMomPair(_p1, _p2);
            partPair.SetEtaPair(_eta1, _eta2);

            // partPair.SetNHitsPair(track1->GetNHits(), track2->GetNHits());

            // Getting gem part of glob. track ...
            Int_t gemIdx1 = track1->GetGemTrackIndex();
            BmnTrack* gemTrack1 = (BmnTrack*) fGemTracks->UncheckedAt(gemIdx1);
            Int_t nHitsGem1 = gemTrack1->GetNHits();

            Int_t gemIdx2 = track2->GetGemTrackIndex();
            BmnTrack* gemTrack2 = (BmnTrack*) fGemTracks->UncheckedAt(gemIdx2);
            Int_t nHitsGem2 = gemTrack2->GetNHits();

            // Getting sil. part of glob. track ...
            Int_t nHitsSil1 = 0;
            Int_t silIdx1 = track1->GetSilTrackIndex();
            if (silIdx1 != -1) {
                BmnTrack* silTrack1 = (BmnTrack*) fSiliconTracks->UncheckedAt(silIdx1);
                nHitsSil1 = silTrack1->GetNHits();
            }

            Int_t nHitsSil2 = 0;
            Int_t silIdx2 = track2->GetSilTrackIndex();
            if (silIdx2 != -1) {
                BmnTrack* silTrack2 = (BmnTrack*) fSiliconTracks->UncheckedAt(silIdx2);
                nHitsSil2 = silTrack2->GetNHits();
            }

            vector <Int_t> particle1{nHitsSil1, nHitsGem1};
            vector <Int_t> particle2{nHitsSil2, nHitsGem2};

            partPair.SetNHitsPair(particle1, particle2);

            // Skipping tracks having more than three silicon hits ...
            if (partPair.GetNHitsPart1("SILICON") > 3 || partPair.GetNHitsPart2("SILICON") > 3) // FIXME
                continue;

            // Track params. are redefined
            Double_t Tx1, Ty1, Tx2, Ty2, p1, p2;
            Double_t A1, A2;

            TVector2 armenPodol;

            Tx1 = proton_V0.GetTx();
            Ty1 = proton_V0.GetTy();
            Tx2 = pion_V0.GetTx();
            Ty2 = pion_V0.GetTy();
            p1 = 1. / proton_V0.GetQp();
            p2 = 1. / pion_V0.GetQp();

            armenPodol = ArmenterosPodol(proton_V0, pion_V0);

            A1 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
            lPos.SetXYZM(Tx1 * A1 * p1, Ty1 * A1 * p1, p1 * A1,
                    fPDG->GetParticle(fPdgParticle1)->Mass());

            p2 *= -1.; // Since in the calculations pos. mom. values should be used

            A2 = 1. / Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
            lNeg.SetXYZM(Tx2 * A2 * p2, Ty2 * A2 * p2, p2 * A2,
                    fPDG->GetParticle(fPdgParticle2)->Mass());

            partPair.SetAlpha(armenPodol.X());
            partPair.SetPtPodol(armenPodol.Y());
            partPair.SetInvMass(TLorentzVector((lPos + lNeg)).Mag());

            // Part to be used when estimating omega ... (dca0 needed), 0 means a primary particle to be decayed...
            partPair.SetTxPair(Tx1, Tx2);
            partPair.SetTyPair(Ty1, Ty2);

            Double_t PzPart1 = Abs(p1) * A1;
            Double_t PzPart2 = Abs(p2) * A2;

            Double_t PzPart0 = PzPart1 + PzPart2;
            Double_t PxPart0 = PzPart1 * Tx1 + PzPart2 * Tx2;
            Double_t PyPart0 = PzPart1 * Ty1 + PzPart2 * Ty2;

            Double_t TxPart0 = PxPart0 / PzPart0;
            Double_t TyPart0 = PyPart0 / PzPart0;

            Double_t xPart0 = TxPart0 * (Vp.Z() - V0.Z()) + V0.X();
            Double_t yPart0 = TyPart0 * (Vp.Z() - V0.Z()) + V0.Y();

            Double_t dca0 = Sqrt(Sq(xPart0 - Vp.X()) + Sq(yPart0 - Vp.Y()));

            partPair.SetDCA0(dca0);

            // To be used for real exp. data
            if (fAnalType[0].Contains("dst") && !fAnalType[0].Contains("eve") && fAnalType[1].Contains("OFF"))
                new((*fParticlePair)[fParticlePair->GetEntriesFast()]) BmnParticlePair(partPair);

            // MC input, no matches
            if (isMC && fAnalType[1].Contains("OFF")) {
                partPair.SetMCTrackIdPart1(iTrack);
                partPair.SetMCTrackIdPart2(jTrack);

                new((*fParticlePair_MC)[fParticlePair_MC->GetEntriesFast()]) BmnParticlePair(partPair);
            }

            // Reco input with matches
            if (fAnalType[1].Contains("ON")) {
                partPair.SetMCTrackIdPart1(recoToMcIdx(iTrack));
                partPair.SetMCTrackIdPart2(recoToMcIdx(jTrack));

                partPair.SetRecoTrackIdPart1(iTrack);
                partPair.SetRecoTrackIdPart2(jTrack);

                new((*fParticlePair_RECO)[fParticlePair_RECO->GetEntriesFast()]) BmnParticlePair(partPair);
            }
        }
    }
}
// -------------------------------------------------------------------

InitStatus BmnTwoParticleDecay::Init() {
    cout << "\nBmnTwoParticleDecay::Init()" << endl;

    FairRootManager* ioman = FairRootManager::Instance();
    TString inFile = (TString) ioman->GetInFile()->GetName();

    if (inFile.Contains("cbm"))
        fIsCbmDst = kTRUE;

    if (fIsCbmDst) {

        if (fGeomFile.IsNull())
            Fatal("BmnTwoParticleDecay::Init()", "No geometry file passed!!!");
        else
            TGeoManager::Import(fGeomFile.Data());
    } else {
        // Read current geometry from database
        Char_t* geoFileName = (Char_t*) "current_geo_file.root";
        Int_t res_code = UniRun::ReadGeometryFile(fRunPeriod, fRunId, geoFileName);
        if (res_code != 0) {
            cout << "Geometry file can't be read from the database" << endl;
            exit(-1);
        }
        TGeoManager::Import(geoFileName);
    }

    // Get run info..
    UniRun* runInfo = UniRun::GetRun(fRunPeriod, fRunId);
    if (!runInfo) {
        cout << "Something is wrong when getting run info from DB..." << endl;
        throw;
    }

    fBranchGemPoints = "StsPoint";
    fBranchSilPoints = "SiliconPoint";
    fBranchGlobalTracks = "BmnGlobalTrack";
    fBranchMCTracks = "MCTrack";
    fBranchGlobalMatch = "BmnGlobalTrackMatch";
    fBranchVertex = "BmnVertex";

    fDstHeader = (DstEventHeader*) ioman->GetObject("DstEventHeader.");

    fGemPoints = (TClonesArray*) ioman->GetObject(fBranchGemPoints.Data());
    fSilPoints = (TClonesArray*) ioman->GetObject(fBranchSilPoints.Data());

    fGlobalTracks = (TClonesArray*) ioman->GetObject(fBranchGlobalTracks.Data());
    fGemTracks = (TClonesArray*) ioman->GetObject("BmnGemTrack");
    fSiliconTracks = (TClonesArray*) ioman->GetObject("BmnSiliconTrack");
    fSilHits = (TClonesArray*) ioman->GetObject("BmnSiliconHit");

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

    fParticlePair_MC = new TClonesArray(className);
    ioman->Register("ParticlePair_MC", "Lambda", fParticlePair_MC, isWriteEveBranch);

    fParticlePair_RECO = new TClonesArray(className);
    ioman->Register("ParticlePair_RECO", "Lambda", fParticlePair_RECO, isWriteDstBranch);

    fParticlePair = new TClonesArray(className);
    ioman->Register("ParticlePair", "Lambda", fParticlePair, isWriteBranch);

    // Adding extended event header to output ....
    fPhysInfo = new DstEventHeaderExtended();
    ioman->Register("DstEventHeaderExtended.", "Lambda", fPhysInfo, isWriteBranch);

    fPDG = TDatabasePDG::Instance();

    fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    fMagField->SetScale(!fAnalType[0].Contains("eve") ? *runInfo->GetFieldVoltage() / 55.87 : 1.33); // FIXME
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

    // Let's look whether a digi file exists to be connected to anal ...
    if (!fDigiDir.IsNull()) {
        
        Bool_t isEoS = (fDigiDir.Contains("/eos")) ? kTRUE : kFALSE;
        
        TString f = (isEoS ? fEoSNode : "") + fDigiDir + TString::Format("bmn_run%d_digi.root", fRunId);
        
        cout << f<< endl;
 
        TChain* ch = new TChain("bmndata");
        ch->Add(f.Data());

        BmnEventHeader* eHeader = nullptr;
        TClonesArray* trigFD = nullptr;
        TClonesArray* trigBD = nullptr;

        ch->SetBranchAddress("BmnEventHeader.", &eHeader);
        ch->SetBranchAddress("BD", &trigBD);
        ch->SetBranchAddress("SI", &trigFD);

        for (Int_t iEvent = 0; iEvent < ch->GetEntries(); iEvent++) {
            ch->GetEntry(iEvent);
            
            if (iEvent % 10000 == 0)
                cout << "digiFile#, processing event: " << iEvent << endl;

            fTrigCountMap[eHeader->GetEventId()] = make_pair(trigBD->GetEntriesFast(), trigFD->GetEntriesFast());
        }
        
        delete ch;
    }

    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnTwoParticleDecay::Exec(Option_t * option) {
    fParticlePair_MC->Delete();
    fParticlePair_RECO->Delete();
    fParticlePair->Delete();
    
    UInt_t id = fDstHeader->GetEventId();
    fPhysInfo->SetEventId(id);
    fPhysInfo->SetRunId(fDstHeader->GetRunId());

    if (!fDigiDir.IsNull()) {                
        pair <Int_t, Int_t> trigPair = fTrigCountMap.find(id)->second;
        fPhysInfo->SetBd(trigPair.first);
        fPhysInfo->SetFd(trigPair.second);    
    }
    
    fEventCounter++;

    if (fEventCounter % 500 == 0)
        cout << "Event# " << fEventCounter << endl;

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
        fEventVertex = (BmnVertex*) fVertex->UncheckedAt(0);
        fPhysInfo->SetVp(fEventVertex->GetX(), fEventVertex->GetY(), fEventVertex->GetZ());        
        fPhysInfo->SetNTracks(fEventVertex->GetNTracks());

        if (fEventVertex->GetNTracks() < 2) // Num of tracks to be used for Vp reconstruction
            return;
        
        TVector3 realVert(fEventVertex->GetX(), fEventVertex->GetY(), fEventVertex->GetZ());
        TVector3 roughVert(0., 0., 0.);

        const Double_t vertexCut = 100.; // Difference between reconstructed Vp and its approximate position

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

Double_t BmnTwoParticleDecay::FindV0ByVirtualPlanes(BmnGlobalTrack* track1, BmnGlobalTrack* track2, Double_t z_0, Double_t range) {
    const Int_t nPlanes = 10; // FIXME

    Bool_t isBadPair = kFALSE;

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
            BmnStatus stat1 = fKalman->TGeoTrackPropagate(&par1, zPlane[iPlane], fPdgParticle1, nullptr, nullptr, kTRUE);
            BmnStatus stat2 = fKalman->TGeoTrackPropagate(&par2, zPlane[iPlane], fPdgParticle2, nullptr, nullptr, kTRUE);

            if (stat1 == kBMNERROR || stat2 == kBMNERROR) {
                isBadPair = kTRUE;
                break;
            }

            Dist[iPlane] = Sqrt(Sq(par1.GetX() - par2.GetX()) + Sq(par1.GetY() - par2.GetY()));
        }

        if (isBadPair)
            return DBL_MAX;

        TGraph V0(nPlanes, zPlane, Dist);
        V0.Fit("pol2", "QF");
        TF1 *fit_func = V0.GetFunction("pol2");
        Double_t a = fit_func->GetParameter(2);

        if (a < 0.) {
            isBadPair = kTRUE;
            break;
        }

        map <Double_t, Double_t> dist_zPlanes; // distance --> z-position

        for (Int_t iPlane = 0; iPlane < nPlanes; iPlane++)
            dist_zPlanes[Dist[iPlane]] = zPlane[iPlane];

        z_0 = dist_zPlanes.begin()->second;

        range /= 2;
    }

    if (!isBadPair)
        return z_0;
    else
        return DBL_MAX;
}

ClassImp(BmnTwoParticleDecay);
