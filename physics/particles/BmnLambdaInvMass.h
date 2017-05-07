// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-04-14

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnLambdaInvMass                                                           //
//                                                                            //
//  A supplementary class for Lambda-reconstruction                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNLAMBDAINVMASS_H
#define BMNLAMBDAINVMASS_H 1

#include <iostream>
#include "FairTask.h"
#include <TDatabasePDG.h>
#include <TH1F.h>
#include <TMath.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TVector2.h>

#include "BmnGemTrack.h"
#include "CbmMCTrack.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunWinter2016.h"
#include "BmnGemStripStationSet_RunSpring2017.h"

using namespace std;
using namespace TMath;

class BmnLambdaInvMass : public FairTask {
private:
    UInt_t fEventCounter; //! event counter
    TDatabasePDG* fPDG; //!  PDG database

    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;

    TH1F* fLambdaInvMass;
    TH1F* fStartZproton;
    TH1F* fStartZpion;
  //  TH1F* fRoInvMassMC;

    TString fBranchGemHits;
    TString fBranchGemTracks;
    
    BmnGemStripStationSet* fDetector; // Detector geometry
    BmnGemStripConfiguration::GEM_CONFIG fGeometry;

public:
    BmnLambdaInvMass() {};
    BmnLambdaInvMass(BmnGemStripConfiguration::GEM_CONFIG);
    virtual ~BmnLambdaInvMass();

    virtual void Exec(Option_t * option);
    virtual InitStatus Init();
    virtual void Finish();

    ClassDef(BmnLambdaInvMass, 0)
};

#endif
