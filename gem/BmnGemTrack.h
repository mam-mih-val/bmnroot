

#ifndef BMNGEMTRACK_H
#define BMNGEMTRACK_H

#include "BmnGemHit.h"
#include "FairTrackParam.h"
#include "BmnFitNode.h"

#include "TArrayI.h"
#include "TObject.h"

#include <map>

//class FairHit;

class BmnGemTrack : public TObject {
public:

    /** Default constructor **/
    BmnGemTrack();

    /** Destructor **/
    virtual ~BmnGemTrack();

    /** Associate a BmnGemHit to the track **/
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

    /** Accessors  **/
    Int_t GetRef() const {
        return fRef;
    }

    Int_t GetNHits() const {
        return fHits.GetSize();
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

    Bool_t IsUsed() const {
        return fUsed;
    }

    FairTrackParam* GetParamFirst() {
        return &fParamFirst;
    }

    FairTrackParam* GetParamLast() {
        return &fParamLast;
    }

    const BmnFitNode* GetFitNode(Int_t index) const {
        return &fFitNodes[index];
    }

    const vector<BmnFitNode>& GetFitNodes() const {
        return fFitNodes;
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

    void SetB(Double_t b) {
        fB = b;
    }

    void SetFitNodes(const vector<BmnFitNode>& nodes) {
        fFitNodes = nodes;
    }

    void SetRef(Int_t ref) {
        fRef = ref;
    }

    void SetUsing(Bool_t use) {
        fUsed = use;
    }

    void SetLength(Double_t length) {
        fLength = length;
    }



private:

    /** Array containing the indices of the STS hits attached to the track **/
    TArrayI fHits;

    /** Reference Id **/
    Int_t fRef;

    /** Track parameters at first and last fitted hit **/
    FairTrackParam fParamFirst;
    FairTrackParam fParamLast;
    vector<BmnFitNode> fFitNodes; // Array of fit nodes

    /** Quality flag **/
    Int_t fFlag;

    /** Chi square and NDF of track fit **/
    Float_t fChi2;
    Int_t fNDF;

    /** Impact parameter of track at target z, in units of its error **/
    Float_t fB;

    /** Track length **/
    Float_t fLength;

    /** Maps from hit z position to hit index. STL map is used because it
     ** is automatically sorted. Temporary only; not for storage.
     ** The Hit index arrays will be filled by the method SortHits.
     **/
    std::map<Float_t, Int_t> fHitMap; //! 

    Bool_t fUsed; //needed to check seeds splitting

    ClassDef(BmnGemTrack, 1);

};

#endif
