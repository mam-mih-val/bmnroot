//Author: Vasilisa Lenivenko (VBLHEP) <vasilisa@jinr.ru> 2021-03-11
#ifndef BmnMwpcHitProducer_H
#define BmnMwpcHitProducer_H 1

#include "BmnEnums.h"
#include "BmnMwpcDigit.h"
#include "CbmTofPoint.h"
#include "CbmTofHit.h"

#include "FairTask.h"

#include "TRandom.h"
#include "TClonesArray.h"
#include "TString.h"
#include "TH2D.h"

#include <math.h>
#include <iostream>
#include <vector>
using namespace std;

class BmnMwpcHitProducer : public FairTask
{
  public:
    /** Default constructor **/
    BmnMwpcHitProducer();
    /** Destructor **/
    virtual ~BmnMwpcHitProducer();

    /** Virtual method Init **/
    virtual InitStatus Init();
    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    /** Virtual method Finish **/
    virtual void Finish();

    // Setters
    void SetOnlyPrimary(Bool_t opt = kFALSE) {
        fOnlyPrimary = opt;
    }

    BmnStatus ProcessPoints();
    BmnStatus ProcessDigits();

  private:
    UInt_t  fEventNo; // event counter
    Bool_t  fDebug = 0;
    TString fOutputFileName;
    TString fInputBranchName;
    TString fInputMCBranchName;
    TString fInputDigiBranchName;
    TString fOutputHitsBranchName;
    TList fList;

    /** Input array of MWPC Points **/
    TClonesArray* fBmnMwpcPointsArray;
    /** Input array of Points **/
    TClonesArray* fBmnPointsArray;
    /** Input array of MWPC Digits **/
    TClonesArray* fBmnMwpcDigitsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of MWPC Hits **/
    TClonesArray* fBmnMwpcHitsArray;
    /** Output array of Hits **/
    TClonesArray* fBmnHitsArray;

    Bool_t fOnlyPrimary;
    TString fRunType; //"points" or "digits"
    Int_t fMwpcNum;
    TRandom rand_gen;
    
    TH2D *hY_vsX2,*hY_vsX3, *hY_vsX_pl0, *hY_vsX_pl1 ,*hY_vsX_pl2, *hY_vsX_star2, *hY_vsX_star3;    //!
    
    const Int_t PDG_B11 = 1000050110;//B11
    const Int_t PDG_Li7 = 1000030070;//Li7
    const Int_t PDG_He4 = 1000020040;//He4
    const Int_t PDG_Li8 = 1000030080;//Li8
    const Int_t PDG_Be9 = 1000040090;//Be9
    const Int_t PDG_H2  = 1000010020;//H2

  ClassDef(BmnMwpcHitProducer, 1);
};

#endif
