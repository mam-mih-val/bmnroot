#ifndef MPDUNIGENGENERATOR_H
#define MPDUNIGENGENERATOR_H

#include <iostream>
#include <FairGenerator.h>
#include <FairPrimaryGenerator.h>
#include <FairMCEventHeader.h>
#include <FairRunSim.h>
#include "FairIon.h"
#include "FairParticle.h"

#include "UParticle.h"
#include "UEvent.h"
#include "URun.h"

#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include <TClonesArray.h>
#include <TRandom.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>

class TVirtualMCStack;
class FairPrimaryGenerator;

class MpdUnigenGenerator : public FairGenerator {
public:

  MpdUnigenGenerator();
  explicit MpdUnigenGenerator(TString fileName, Bool_t isSpectator = kFALSE);
  ~MpdUnigenGenerator() override = default;

  Bool_t ReadEvent(FairPrimaryGenerator* primGen) override;
  Int_t RegisterIons();

  void SkipEvents(Int_t ev) {
      fEventNumber = ev;
      std::cout << "NUMBER OF SKIPPED EVENTS = " << ev << std::endl;
  }

  void SetEventPlane(Double_t phiMin, Double_t phiMax) {
      fPhiMin = phiMin;
      fPhiMax = phiMax;
      fEventPlaneSet = kTRUE;
  }
private:
  Long64_t fEventNumber; //!
  Long64_t fNEntries; //!
  TFile *fInFile;
  TTree *fInTree;
  UEvent *fEvent;
  URun *fRun;
  UParticle *fParticle;
  Bool_t fSpectatorsON;

  Double_t fPhiMin, fPhiMax; // Limits of event plane angle
  Bool_t fEventPlaneSet; // Flag whether event plane angle is used

  std::map<TString, FairIon*> fIonMap;  //!< Map from ion name to FairIon

  static const Int_t kPdgLambda = 10000000;
  static const Int_t kPdgCharge = 10000;
  static const Int_t kPdgMass   = 10;

  Double_t fGammaCM = 0.;
  Double_t fBetaCM = 0.;

  Int_t GetIonCharge(Int_t pdgCode) const { return (pdgCode % kPdgLambda) / kPdgCharge; }
  Int_t GetIonLambdas(Int_t pdgCode) { return (pdgCode % (10 * kPdgLambda)) / kPdgLambda; }
  Int_t GetIonMass(Int_t pdgCode) { return (pdgCode % kPdgCharge) / kPdgMass; }

  MpdUnigenGenerator(const MpdUnigenGenerator&);
  MpdUnigenGenerator& operator=(const MpdUnigenGenerator&);

  ClassDef(MpdUnigenGenerator, 1);
};

#endif
