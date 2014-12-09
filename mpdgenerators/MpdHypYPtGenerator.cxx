// -------------------------------------------------------------------------
// -----         CbmAnaHypYPtGenerator source file                     -----
// -----          Created 03/10/04  by E. Kryshen                      -----
// -----  Updated 10/02/10  by A. Zinchenko for MPD (with name change) -----
// -------------------------------------------------------------------------

#include "TRandom.h"
#include "MpdHypYPtGenerator.h"
#include "FairPrimaryGenerator.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TF1.h"
#include "TMath.h"

// ------------------------------------------------------------------------
MpdHypYPtGenerator::MpdHypYPtGenerator():FairGenerator(){
  // Default constructor
  fPDGType =  -1;
  fMult    =   0;
}

// ------------------------------------------------------------------------
MpdHypYPtGenerator::MpdHypYPtGenerator(Int_t pdgid, Int_t mult):FairGenerator(){
  // Constructor. Set default distributions
  fPDGType = pdgid;
  fMult    = mult;
  SetDistributionPt();
  SetDistributionY();
  SetRangePt();
}
// ------------------------------------------------------------------------
Bool_t  MpdHypYPtGenerator::Init(){   // EL
  // Initialize generator
  // Check for particle type
  TDatabasePDG* pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (! particle)  {
    Fatal("CbmAnaHypYPtGenerator","PDG code %d not defined.",fPDGType);
    return kFALSE;  // EL
  }
  fPDGMass = particle->Mass();
  //gRandom->SetSeed(0);
  fDistPt = new TF1("distPt","x*exp(-sqrt(x*x+[1]*[1])/[0])",fPtMin,fPtMax);
  fDistPt->SetParameters(fT,fPDGMass,fY0,fSigma);
  Info("Init","pdg=%i y0=%4.2f sigma_y=%4.2f T_pt=%6.4f",fPDGType,fY0,fSigma,fT);
  return kTRUE;   // EL
}

// ------------------------------------------------------------------------
Bool_t MpdHypYPtGenerator::ReadEvent(FairPrimaryGenerator* primGen){
  Double_t phi, pt, y, mt, px, py, pz;

  // Generate particles
  for (Int_t k = 0; k < fMult; k++) {

    phi = gRandom->Uniform(0,TMath::TwoPi());
    pt  = fDistPt->GetRandom(fPtMin,fPtMax);
    px  = pt*TMath::Cos(phi);
    py  = pt*TMath::Sin(phi);
    y   = gRandom->Gaus(fY0,fSigma);
    mt  = TMath::Sqrt(fPDGMass*fPDGMass + pt*pt);
    pz  = mt * TMath::SinH(y);
    Info("ReadEvent","Particle generated: pdg=%i pt=%f y=%f\n",fPDGType,pt,y);
    primGen->AddTrack(fPDGType, px, py, pz, 0, 0, 0);
  }
  return kTRUE;

}
// ------------------------------------------------------------------------

ClassImp(MpdHypYPtGenerator)

