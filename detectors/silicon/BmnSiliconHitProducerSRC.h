#ifndef BMNSILICONHITPRODUCERSRC_H
#define BMNSILICONHITPRODUCERSRC_H 1

#include "BmnSiliconStationSet.h"
#include "BmnSiliconConfiguration.h"
#include "BmnHit.h"
#include "CbmStsPoint.h"
#include "CbmStsTrack.h"

#include "FairTask.h"

#include "TRandom.h"
#include "TList.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"

using namespace std;

class BmnSiliconHitProducerSRC : public FairTask
{
  public:
    /** Default constructor **/
    BmnSiliconHitProducerSRC();
    /** Destructor **/
    virtual ~BmnSiliconHitProducerSRC();

    /** Virtual method Init **/
    virtual InitStatus Init();
    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    /** Virtual method Finish **/
    virtual void Finish();

  private:
    UInt_t  fEventNo; // event counter
    Bool_t  fDebug = 0;
    TList   fList;
    TString fInputBranchName;
    TString fTracksBranchName;
    TString fOutputHitsBranchName;
    TString fOutputHitsBranchName2;
    TString fOutputFileName;

    /** Input array of Silicon Points **/
    TClonesArray* fBmnPointsArray;              //!

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;               //!

    /** Output array of Silicon Hits **/
    TClonesArray* fBmnHitsArray;                //!
    TClonesArray* fBmnHitsArray2;               //!
    
    BmnSiliconStationSet *SiliconStationSet;    //! Entire GEM detector
    TRandom rand_gen;
    TH1D *hdX, *hdXp;                           //!
    
    const Int_t PDG_B11 = 1000050110;//B11
    const Int_t PDG_Li7 = 1000030070;//Li7
    const Int_t PDG_He4 = 1000020040;//He4
    const Int_t PDG_Li8 = 1000030080;//Li8
    const Int_t PDG_Be9 = 1000040090;//Be9
    const Int_t PDG_H2  = 1000010020;//H2
    
  ClassDef(BmnSiliconHitProducerSRC, 1);
};

#endif
