#ifndef __BMN_TOF_HIT_PRODUCER_IDEAL_H
#define __BMN_TOF_HIT_PRODUCER_IDEAL_H 1

#include "BmnTask.h"

#include <TClonesArray.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TList.h>
#include <TVector3.h>

#include <iostream>

class BmnTofHitProducerIdeal : public BmnTask {
  protected:
    TClonesArray *aMcPoints;     //! <--- MC input
    TClonesArray *aMcTracks;     //! <--- MC input
    TClonesArray *aExpDigits;    //! <--- Exp input
    TClonesArray *aExpDigitsT0;  //! <--- Exp input
    TClonesArray *aTofHits;      //! ---> output

    Bool_t fDoTest;
    Bool_t fDoMergeHits;
    Bool_t fUseMCData;
    Bool_t fOnlyPrimary;
    TString fTestFlnm;

    // QA test histos
    TList fList;              //!
    TH1D *h1TestOccup;        //!
    TH2D *h2TestMergedTimes;  //!

    void AddHit(Int_t detUID, const TVector3 &posHit, const TVector3 &posHitErr, Int_t pointIndex, Int_t trackIndex,
                Double_t time);
    Int_t CompressHits();
    Int_t MergeHitsOnStrip();  // save only the fastest hit in the strip

  public:
    BmnTofHitProducerIdeal(const char *name = "TOF Ideal HitProducer", Bool_t useMCdata = true, Int_t verbose = 1,
                           Bool_t DoTest = false, Bool_t DoMergeHits = false);
    virtual ~BmnTofHitProducerIdeal();

    virtual InitStatus Init();
    virtual void Exec(Option_t *option);
    virtual void Finish();

    void SetOnlyPrimary(Bool_t opt) { fOnlyPrimary = opt; }
    void Dump(const char *comment = nullptr, std::ostream &out = std::cout) const;
    void SetTestFlnm(const char *flnm) { fTestFlnm = flnm; };

  ClassDef(BmnTofHitProducerIdeal, 1);
};

#endif
