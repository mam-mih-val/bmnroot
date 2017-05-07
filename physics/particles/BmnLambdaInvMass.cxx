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
fGeometry(config) {
    fBranchGemHits = "BmnGemStripHit";
    //    fBranchGemTracks = "BmnGemTrack";
    fBranchGemTracks = "MCTrack";

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

// -------------------------------------------------------------------

InitStatus BmnLambdaInvMass::Init() {
    cout << "\nBmnLambdaInvMass::Init()" << endl;

    FairRootManager* ioman = FairRootManager::Instance();
    fGemHits = (TClonesArray*) ioman->GetObject(fBranchGemHits.Data());
    //fGemTracks = (TClonesArray*) ioman->GetObject(fBranchGemTracks.Data());
    fGemTracks = (TClonesArray*) ioman->GetObject(fBranchGemTracks.Data());

    //
    fLambdaInvMass = new TH1F("LambdaInvMass", "#Lambda -> #pi^{-}p, Inv. mass", 200, 1.05, 1.3);
    fStartZproton = new TH1F("StartZ", "StartZ", 400, 0., 40.);
    //    fRoInvMass->GetXaxis()->SetTitle("M_{inv}");
    //    fRoInvMassMC = new TH1F("RoInvMassMC", "Invariant Mass (MC): (#pi_{+}#pi_{-}) 90%CL", 200, 0., 2.);
    //    fRoInvMassMC->GetXaxis()->SetTitle("M_{inv}");
    //    fRoInvMassMC->SetLineColor(kRed);

    fPDG = TDatabasePDG::Instance();

    return kSUCCESS;
}

// -------------------------------------------------------------------

void BmnLambdaInvMass::Exec(Option_t * option) {
    fEventCounter++;
    if (fEventCounter % 1000 == 0)
        cout << fEventCounter << endl;

    //MpdEvent *mpdEvent = fDstEvent; // one event by one event
    //event->Dump(); 
    //    TClonesArray *mpdTracks = fDstEvent->GetGlobalTracks();
    //    Int_t nTracks = mpdTracks->GetEntriesFast();
    //
    //    cout << "N of Reconstructed tracks = " << nTracks << endl;

    //mpdTracks->Dump();

    // Calculate invariant mass of rho0 meson (lambda -> pi- + p)
    TLorentzVector lPos, lNeg;

    /* Events loop */
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        //        BmnGemTrack* track1 = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
        CbmMCTrack* track1 = (CbmMCTrack*) fGemTracks->UncheckedAt(iTrack);
        if (track1->GetPdgCode() != 2212)
            continue;

        Int_t motherId = track1->GetMotherId();
  
        if (motherId != -1 and ((CbmMCTrack*)fGemTracks->UncheckedAt(motherId))->GetPdgCode() == 3122) {
            fStartZproton->Fill(track1->GetStartZ());
        
        }
        
        // Extract track1. params. at last hit
        // Double_t Tx1 = track1->GetParamLast()->GetTx();
        // Double_t Ty1 = track1->GetParamLast()->GetTy();
        Double_t Px1 = track1->GetPx();
        Double_t Py1 = track1->GetPy();
        Double_t Pz1 = track1->GetPz();
        Double_t P1 = track1->GetP();
        Double_t y1 = track1->GetRapidity();
//        if (P1 > 3. || P1 < 0.1 || Abs(y1) > 3.5)
//            continue;
        if (track1->GetStartZ() > 40. || track1->GetStartZ() < 0.1)
            continue;

        // Select protons here. Tx1 < 0 means a particle not to be a proton
        //        if (Tx1 < 0)
//        if (Px1 < 0)
//            continue;

        //        Double_t mom1 = 1. / track1->GetParamLast()->GetQp();
        //        Double_t fact1 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
        //        lPos.SetXYZM(Tx1 * fact1 * mom1, Ty1 * fact1 * mom1, mom1 * fact1,
        //                fPDG->GetParticle(2212)->Mass() * fPDG->GetParticle(2212)->Mass());
        lPos.SetXYZM(Px1, Py1, Pz1, fPDG->GetParticle(2212)->Mass());

        for (Int_t jTrack = 0; jTrack < fGemTracks->GetEntriesFast(); jTrack++) {
            // BmnGemTrack* track2 = (BmnGemTrack*) fGemTracks->UncheckedAt(jTrack);
            CbmMCTrack* track2 = (CbmMCTrack*) fGemTracks->UncheckedAt(jTrack);
            if (track2->GetPdgCode() != -211)
                continue;
            // Extract track1. params. at last hit
            //            Double_t Tx2 = track2->GetParamLast()->GetTx();
            //            Double_t Ty2 = track2->GetParamLast()->GetTy();
            Double_t Px2 = track2->GetPx();
            Double_t Py2 = track2->GetPy();
            Double_t Pz2 = track2->GetPz();
            Double_t P2 = track2->GetP();
            Double_t y2 = track2->GetRapidity();
//            if (P2 > 1. || P2 < 0.1 || Abs(y2) > 3.5)
//                continue;
            if (track2->GetStartZ() > 40. || track2->GetStartZ() < 0.1)
            continue;
            // Select neg. pions here. Tx2 > 0 means a particle not to be a neg. pion
            //            if (Tx2 > 0)
//           if (Px2 > 0)
//                continue;

            //            Double_t mom2 = 1. / track2->GetParamLast()->GetQp();
            //            Double_t fact2 = 1. / Sqrt(Tx1 * Tx1 + Ty1 * Ty1 + 1);
            //            lNeg.SetXYZM(Tx2 * fact2 * mom2, Ty2 * fact2 * mom2, mom2 * fact2,
            //                    fPDG->GetParticle(-211)->Mass() * fPDG->GetParticle(-211)->Mass());
            lNeg.SetXYZM(Px2, Py2, Pz2, fPDG->GetParticle(-211)->Mass());

            Float_t minv = TLorentzVector((lPos + lNeg)).Mag();
            fLambdaInvMass->Fill(minv);
        }     
    }
}

// -------------------------------------------------------------------

void BmnLambdaInvMass::Finish() {

    Double_t scale = 1. / (Double_t) fEventCounter;
    fLambdaInvMass->Scale(scale);

    TCanvas* c = new TCanvas("c1", "c1", 1200, 800);
    fLambdaInvMass->Draw("P");
    fLambdaInvMass->SetMarkerStyle(20);
    fLambdaInvMass->SetMarkerSize(1.1);
    c->SaveAs("lambda.root");
    delete c;
    
    TCanvas* c1 = new TCanvas("c1", "c1", 1200, 800);
    fStartZproton->Draw();
    //fLambdaInvMass->Draw("P");
    //fLambdaInvMass->SetMarkerStyle(20);
    //fLambdaInvMass->SetMarkerSize(1.1);
    
    c1->SaveAs("startZ.root");
    delete c1;
    
    /*
    //cout<<"\n-I- [BmnLambdaInvMass::Finish] "<< endl;

    //TFile fileOut("MinvRho0.root","recreate");
    Double_t scale = 1. / (Double_t) fEventCounter;

    fRoInvMass->Scale(scale); // scale to total number of events
    fRoInvMassMC->Scale(scale); // scale to total number of events

    fRoInvMass->Write("");
    fRoInvMassMC->Write("");
     */
}

// ClassImp(BmnLambdaInvMass);
