/*
 * Base class to store BMN tracks
 * Author: Sergei Merts
 */

#ifndef BMNTRACK_H
#define BMNTRACK_H

#include "FairTrackParam.h"
#include "TArrayI.h"
#include "TObject.h"
#include "FairHit.h"
#include <TClonesArray.h>
#include <map>

using namespace std;

class BmnTrack : public TObject {
public:

    /** Default constructor **/
    BmnTrack();

    /** Destructor **/
    virtual ~BmnTrack();

    /** Associate a FairHit to the track **/
    void AddHit(Int_t hitIndex, FairHit* Hit);

    /** Public method Print
     ** Output to screen 
     **/
    void Print();

    /** Public method SortHits
     ** Sorts the hits in downstream direction
     ** and writes the hit indices into the member fHits
     **/
    void SortHits();
    void DeleteHits() {fHitMap.clear(); fNhits=0;};

    /** Accessors  **/

    Int_t GetNHits() const {
        return fNhits;
    }

    Int_t GetHitIndex(Int_t iHit) const {
        return fHits.At(iHit);
    }

    Int_t GetFlag() const {
        return fFlag;
    }

    Float_t GetChi2() const {
        return fChi2;
    }

    Int_t GetNDF() const {
        return fNDF;
    }

    Float_t GetB() const {
        return fB;
    }

    Float_t GetLength() const {
        return fLength;
    }

    FairTrackParam* GetParamFirst() {
        return &fParamFirst;
    }

    FairTrackParam* GetParamLast() {
        return &fParamLast;
    }
    
    Double_t GetP() {
        return 1.0 / fParamFirst.GetQp();
    }

    /** Modifiers  **/
    void SetParamFirst(FairTrackParam& par) {
        fParamFirst = par;
    }

    void SetParamLast(FairTrackParam& par) {
        fParamLast = par;
    }

    void SetFlag(Int_t flag) {
        fFlag = flag;
    }

    void SetChi2(Double_t chi2) {
        fChi2 = chi2;
    }

    void SetNDF(Int_t ndf) {
        fNDF = ndf;
    }
    
    void SetNHits(Int_t n) {
        fNhits = n;
    }
    
    void SetB(Double_t b) {
        fB = b;
    }

    void SetLength(Double_t length) {
        fLength = length;
    }
    
    Bool_t IsUsed() {
        return fUsing;
    }
    
    void SetUsing(Bool_t flag) {
        fUsing = flag;
    }

private:

    /** Array containing the indices of the STS hits attached to the track **/
    TArrayI fHits;

    /** Track parameters at first and last fitted hit **/
    FairTrackParam fParamFirst;
    FairTrackParam fParamLast;

    /** Quality flag **/
    Int_t fFlag; // 13 for tracks not used in vertex finder

    /** Chi square and NDF of track fit **/
    Float_t fChi2;
    Int_t fNDF;

    /** Impact parameter of track at target z, in units of its error **/
    Float_t fB;

    /** Track length **/
    Float_t fLength;
    
    /** Number of hits on track **/
    Int_t fNhits;

    /** Maps from hit z position to hit index. STL map is used because it
     ** is automatically sorted. Temporary only; not for storage.
     ** The Hit index arrays will be filled by the method SortHits.
     **/
    map<Float_t, Int_t> fHitMap; //!
    
    Bool_t fUsing;

    ClassDef(BmnTrack, 2);

};

#endif /* BMNTRACK_H */

