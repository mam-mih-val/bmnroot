/**
 * MpdHistoGenerator.cxx
 * *@author A.Zinchenko <Alexander.Zinchenko@jinr.ru>
 * The MpdHistoGenerator produces particles according to their Eta-Pt histogram
 **/

#include "MpdHistoGenerator.h"
#include "FairMCEventHeader.h"
#include "FairPrimaryGenerator.h"

#include "TDatabasePDG.h"
#include "TFile.h"
#include "TH2D.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVirtualMC.h"

#include <fstream>
#include <iostream>
#include <iomanip>
// #define debug_hsd

using namespace std;

// ------------------------------------------------------------------------
MpdHistoGenerator::MpdHistoGenerator() : FairGenerator(), 
    fPdgCode(-211), fMult(1), fHist(NULL)
{
  // Constructor

  //gRandom->SetSeed(0); // change random seed

  // Get histograms Pt vs Eta (He3)
  TFile f("he3_eta_pt.root");
  f.ReadAll();
  fHist = (TH2D*) f.FindObject("hPtEta");
  fHist->SetDirectory(0);
}

// ------------------------------------------------------------------------
MpdHistoGenerator::MpdHistoGenerator(Int_t pdgid, Int_t mult) : FairGenerator(), 
		   fPdgCode(pdgid), fMult(mult), fHist(NULL)
{
  // Constructor

  //gRandom->SetSeed(0); // change random seed

  // Get histograms Pt vs Eta (He3)
  TFile f("he3_eta_pt.root");
  f.ReadAll();
  fHist = (TH2D*) f.FindObject("hPtEta");
  fHist->SetDirectory(0);
}

// ------------------------------------------------------------------------
MpdHistoGenerator::~MpdHistoGenerator()
{
  // Destructor

}

//-------------------------------------------------------------------------
Bool_t MpdHistoGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
  // Generate event 

  static Int_t first = 1;

  if (first) {
    first = 0;
    //cout << " histo " << fHist << " " << fHist->GetNbinsX() << " " << fHist->GetNbinsY() << endl;
    if (TDatabasePDG::Instance()->GetParticle(fPdgCode) == NULL) {
      // Define particle with fPdgCode (take it from DCM-QGSM input file)
      // To be done ...
    }
  }

  Double_t pt, eta, phi;
  TVector3 part;

  for (Int_t i  = 0; i < fMult; ++i) {
    phi = gRandom->Uniform(TMath::TwoPi());
    fHist->GetRandom2(eta, pt);
  
    part.SetPtEtaPhi(pt, eta, phi);

    //FairMCEventHeader* eventHeader = primGen->GetEvent();
    /* Set event impact parameter in MCEvent if not yet done */
    /*
      if (eventHeader && (!eventHeader->IsSet()) ) {
      eventHeader->SetB(fB);
      eventHeader->MarkSet(kTRUE);
      }
    */

    //primGen->AddTrack(1000020030, He3.Px(), He3.Py(), He3.Pz(), 0., 0., 0., -1);
    primGen->AddTrack(fPdgCode, part.Px(), part.Py(), part.Pz(), 0., 0., 0., -1);
  }

  return kTRUE;
}

// ------------------------------------------------------------------------

