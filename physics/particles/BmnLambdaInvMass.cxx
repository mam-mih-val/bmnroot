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
fEventCounter(0),
fMomProtMin(-DBL_MAX),
fMomProtMax(DBL_MAX),
fMomPionMin(-DBL_MAX),
fMomPionMax(DBL_MAX),
fGeometry(config) {
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
        // BmnGemStripHit* lastHit1 = (BmnGemStripHit*) fGemHits->UncheckedAt(track1->GetHitIndex(track1->GetNHits() - 1));
        // if (track1->GetParamLast()->GetTx() < 0.)
        //   continue;
        //

        Double_t Tx1 = track1->GetParamFirst()->GetTx();
        Double_t Ty1 = track1->GetParamFirst()->GetTy();
        Double_t mom1 = 1. / track1->GetParamFirst()->GetQp();

        // Select protons here. mom1 < 0 means a particle not to be a proton
        if (mom1 < 0. || Abs(mom1) < fMomProtMin || Abs(mom1) > fMomProtMax)
            continue;

        Double_t fact1 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
        lPos.SetXYZM(Tx1 * fact1 * mom1, Ty1 * fact1 * mom1, mom1 * fact1, fPDG->GetParticle(2212)->Mass());

        for (Int_t jTrack = 0; jTrack < fGemTracks->GetEntriesFast(); jTrack++) {
            if (iTrack == jTrack)
                continue;

            BmnGemTrack* track2 = (BmnGemTrack*) fGemTracks->UncheckedAt(jTrack);
            // BmnGemStripHit* lastHit2 = (BmnGemStripHit*) fGemHits->UncheckedAt(track2->GetHitIndex(track2->GetNHits() - 1));
            // if (track2->GetParamLast()->GetTx() > 0.)
            //   continue;
            //

            Double_t Tx2 = track2->GetParamFirst()->GetTx();
            Double_t Ty2 = track2->GetParamFirst()->GetTy();
            Double_t mom2 = 1. / track2->GetParamFirst()->GetQp();

            // Select protons here. mom2 < 0 means a particle not to be a pion
            if (mom2 > 0. || Abs(mom2) < fMomPionMin || Abs(mom2) > fMomPionMax)
                continue;

            Double_t fact2 = 1. / Sqrt(Tx2 * Tx2 + Ty2 * Ty2 + 1);
            lNeg.SetXYZM(Tx2 * fact2 * mom2, Ty2 * fact2 * mom2, mom2 * fact2, fPDG->GetParticle(-211)->Mass());
            fLambdaInvMass->Fill(TLorentzVector((lPos + lNeg)).Mag());
        }
    }
}

void BmnLambdaInvMass::McAnalysis() {
    TLorentzVector lPos, lNeg;
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* track1 = (CbmMCTrack*) fGemTracks->UncheckedAt(iTrack);
        if (track1->GetPdgCode() != 2212)
            continue;
        
//        Int_t motherId1 = track1->GetMotherId();
//              if (motherId1 != -1 and ((CbmMCTrack*) fGemTracks->UncheckedAt(motherId1))->GetPdgCode() != 3122) 
//                  continue;

        Double_t Px1 = track1->GetPx();
        Double_t Py1 = track1->GetPy();
        Double_t Pz1 = track1->GetPz();
        Double_t P1 = track1->GetP();

        if (P1 < fMomProtMin || P1 > fMomProtMax)
            continue;
        // Double_t y1 = track1->GetRapidity();
        lPos.SetXYZM(Px1, Py1, Pz1, fPDG->GetParticle(2212)->Mass());
      
        for (Int_t jTrack = 0; jTrack < fGemTracks->GetEntriesFast(); jTrack++) {
            if (iTrack == jTrack)
                continue;

            CbmMCTrack* track2 = (CbmMCTrack*) fGemTracks->UncheckedAt(jTrack);
            if (track2->GetPdgCode() != -211)
                continue;

            //            Int_t motherId2 = track2->GetMotherId();
            //              if (motherId2 != -1 and ((CbmMCTrack*) fGemTracks->UncheckedAt(motherId2))->GetPdgCode() != 3122) 
            //                  continue;

            Double_t Px2 = track2->GetPx();
            Double_t Py2 = track2->GetPy();
            Double_t Pz2 = track2->GetPz();
            Double_t P2 = track2->GetP();

            if (P2 < fMomPionMin || P2 > fMomPionMax)
                continue;
            // Double_t y2 = track2->GetRapidity();
            lNeg.SetXYZM(Px2, Py2, Pz2, fPDG->GetParticle(-211)->Mass());
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

    fLambdaInvMass = new TH1F("LambdaInvMass", "#Lambda -> #pi^{-}p, Inv. mass", 500, 0., 0.);
    fPDG = TDatabasePDG::Instance();

    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnLambdaInvMass::Exec(Option_t * option) {
    fEventCounter++;
    if (fEventCounter % 1000 == 0)
        cout << fEventCounter << endl;

    if (!fUseMc)
        RecoAnalysis();
    else
        McAnalysis();
}

// -------------------------------------------------------------------

void BmnLambdaInvMass::Finish() {

    Double_t scale = 1. / (Double_t) fEventCounter;
    fLambdaInvMass->Scale(scale);

    TCanvas* c = new TCanvas("c1", "c1", 1200, 800);
    fLambdaInvMass->Draw("P");
    fLambdaInvMass->SetMarkerStyle(20);
    fLambdaInvMass->SetMarkerSize(1.1);
    c->SaveAs(fOutFileName.Data());
    delete c;

    cout << "\n-I- [BmnLambdaInvMass::Finish] " << endl;
}

ClassImp(BmnLambdaInvMass);
