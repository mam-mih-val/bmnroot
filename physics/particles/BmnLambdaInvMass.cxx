// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-04-14

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnLambdaInvMass                                                           //
//                                                                            //
//  A supplementary class for Lambda-reconstruction                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include <TCanvas.h>
#include "BmnLambdaInvMass.h"

BmnLambdaInvMass::BmnLambdaInvMass(BmnGemStripConfiguration::GEM_CONFIG config) :
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
fYPionMin(DBL_MAX),
fGeometry(config),
fDebugCalculations(kFALSE),
fCutsDistrOnly(kFALSE) {
    fBranchGemHits = "BmnGemStripHit";

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

BmnLambdaInvMass::~BmnLambdaInvMass() {
    delete fDetector;
}

void BmnLambdaInvMass::RecoAnalysis() {
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

        Double_t A1 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
        lPos.SetXYZM(Tx1 * A1 * p1, Ty1 * A1 * p1, p1 * A1, fPDG->GetParticle(fPdgProton)->Mass());

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

vector <Double_t> BmnLambdaInvMass::DebugCalculations(BmnGemTrack* track1, BmnGemTrack* track2, vector <Double_t> dP, vector <Double_t> dT) {
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

void BmnLambdaInvMass::McAnalysis() {
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

        if (P1 < fMomProtMin || P1 > fMomProtMax ||
                Tx1 < fTxProtMin || Tx1 > fTxProtMax ||
                Ty1 < fTyProtMin || Ty1 > fTyProtMax ||
                Y1 < fYProtMin || Y1 > fYProtMax)
            continue;
        lPos.SetXYZM(Px1, Py1, Pz1, fPDG->GetParticle(fPdgProton)->Mass());

        for (Int_t jTrack = 0; jTrack < fGemTracks->GetEntriesFast(); jTrack++) {
            if (iTrack == jTrack)
                continue;

            CbmMCTrack* track2 = (CbmMCTrack*) fGemTracks->UncheckedAt(jTrack);
            if (track2->GetPdgCode() != fPdgPionMinus)
                continue;

            Double_t Px2 = track2->GetPx();
            Double_t Py2 = track2->GetPy();
            Double_t Pz2 = track2->GetPz();
            Double_t P2 = track2->GetP();
            Double_t Tx2 = Px2 / Pz2;
            Double_t Ty2 = Py2 / Pz2;
            Double_t Y2 = track2->GetRapidity();

            if (P2 < fMomPionMin || P2 > fMomPionMax ||
                    Tx2 < fTxPionMin || Tx2 > fTxPionMax ||
                    Ty2 < fTyPionMin || Ty2 > fTyPionMax || 
                    Y2 < fYPionMin || Y2 > fYPionMax)
                continue;
            lNeg.SetXYZM(Px2, Py2, Pz2, fPDG->GetParticle(fPdgPionMinus)->Mass());
            fLambdaInvMass->Fill(TLorentzVector((lPos + lNeg)).Mag());
        }
    }
}

// -------------------------------------------------------------------

InitStatus BmnLambdaInvMass::Init() {
    cout << "\nBmnLambdaInvMass::Init()" << endl;

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

    if (!fUseMc)
        fGemHits = (TClonesArray*) ioman->GetObject(fBranchGemHits.Data());
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

    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnLambdaInvMass::Exec(Option_t* option) {
    fEventCounter++;
    if (fEventCounter % 1000 == 0)
        cout << fEventCounter << endl;

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

void BmnLambdaInvMass::ObtainCuts() {
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* track = (CbmMCTrack*) fGemTracks->UncheckedAt(iTrack);

        if (track->GetPdgCode() != fPdgLambda)
            continue;

        if (track->GetStartZ() > fDetector->GetGemStation(0)->GetZPosition())
            continue;

        CbmMCTrack* proton = NULL;
        CbmMCTrack* pion = NULL;
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

            fNhitsPerProton->Fill(proton->GetNPoints(DetectorId::kGEM));
            fNhitsPerPion->Fill(pion->GetNPoints(DetectorId::kGEM));

            fNhitsPerProtonVsP->Fill(proton->GetNPoints(DetectorId::kGEM), proton->GetP());
            fNhitsPerPionVsP->Fill(pion->GetNPoints(DetectorId::kGEM), pion->GetP());
        }
    }
}


// -------------------------------------------------------------------

void BmnLambdaInvMass::Finish() {

    // Double_t scale = 1. / (Double_t) fEventCounter;
    // fLambdaInvMass->Scale(scale);

    if (!fCutsDistrOnly) {
        TF1* fI = new TF1("dblgaus", "[0] * TMath::Exp(-(x - [1]) * (x - [1]) / [2] / [2]) + [3] * TMath::Exp(-(x - [4]) * (x - [4]) / [5] / [5])", 1, 1.2);
        fI->SetParameter(1, 1.115);
        fI->SetParameter(4, 1.115);
        // fI->Set
        fLambdaInvMass->Fit("dblgaus", "WW", "", 1.05, 1.15);

        TCanvas* c = new TCanvas("c1", "c1", 1200, 800);
        fLambdaInvMass->Draw("P");
        fLambdaInvMass->SetMarkerStyle(20);
        fLambdaInvMass->SetMarkerSize(1.1);
        c->SaveAs(fOutFileName.Data());
        delete c;
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

    cout << "\n-I- [BmnLambdaInvMass::Finish] " << endl;
}

ClassImp(BmnLambdaInvMass);
