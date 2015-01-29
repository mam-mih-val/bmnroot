// -------------------------------------------------------------------------
// -----                    BmnGlobalTrack header file                 -----
// -----                        Created by S. Merts                    -----
// -------------------------------------------------------------------------

/**  BmnGlobalTrack.h
 *@author S.Merts
 **
 ** Data class for Global BMN track. Data level RECO.
 ** It consists of local tracks in GEM and TOF and DCH hits.
 **
 **/

#ifndef BmnGlobalTrack_H_
#define BmnGlobalTrack_H_ 1

#include "FairTrackParam.h"
#include "BmnFitNode.h"
#include "TObject.h"

class BmnGlobalTrack : public TObject
{

 public:

  /** Default constructor **/
  BmnGlobalTrack();


  /** Destructor **/
  virtual ~BmnGlobalTrack();


  /** Accessors **/
  Int_t GetGemTrackIndex()  const { return fGemTrack; }
  Int_t GetTof1HitIndex()   const { return fTof1Hit;  }
  Int_t GetTof2HitIndex()   const { return fTof2Hit;  }
  Int_t GetDch1HitIndex()   const { return fDch1Hit;  }
  Int_t GetDch2HitIndex()   const { return fDch2Hit;  }
  const FairTrackParam* GetParamFirst() const { return &fParamFirst;  }
  const FairTrackParam* GetParamLast()  const { return &fParamLast;   }
  Double_t GetChi2()        const { return fChi2;     }
  Int_t GetNDF()            const { return fNDF;      }
  Int_t GetFlag()           const { return fFlag;     }
  Double_t GetLength()      const { return fLength;   }
  Float_t GetRefId()       const { return fRefId;    }
  
  const BmnFitNode* GetFitNode(Int_t index) const {return &fFitNodes[index];}
  const vector<BmnFitNode>& GetFitNodes() const {return fFitNodes;}
  Int_t GetNofHits() const {return fNofHits;}


  /** Modifiers **/
  void SetGemTrackIndex(Int_t iGem)  { fGemTrack = iGem;  }
  void SetTof1HitIndex(Int_t iTof1Hit) { fTof1Hit = iTof1Hit; }
  void SetTof2HitIndex(Int_t iTof2Hit) { fTof2Hit = iTof2Hit; }
  void SetDch1HitIndex(Int_t iDch1Hit) { fDch1Hit = iDch1Hit; }
  void SetDch2HitIndex(Int_t iDch2Hit) { fDch2Hit = iDch2Hit; }
  void SetRefId(Int_t ref) { fRefId = ref; }
  
  void SetParamFirst(const FairTrackParam* parFirst) { fParamFirst = *parFirst;}
  void SetParamLast(const FairTrackParam* parLast) { fParamLast = *parLast;}
  void SetChi2(Double_t chi2)        { fChi2     = chi2;  }
  void SetNDF(Int_t ndf)             { fNDF      = ndf;   }
  void SetFlag(Int_t iFlag)          { fFlag     = iFlag; }
  void SetLength(Double_t length)    { fLength   = length;}
  
  void SetFitNodes(const vector<BmnFitNode>& nodes) {fFitNodes = nodes;}
  void SetNofHits(Int_t n) {fNofHits = n;}


  /** Output to screen **/
  void Print() const;


 private:

  /** Indices of local parts of global track **/
  Int_t fGemTrack;
  Int_t fTof1Hit;
  Int_t fTof2Hit;
  Int_t fDch1Hit;
  Int_t fDch2Hit;

  Int_t fNofHits; //number of all hits
  
  /** Global track parameters at first and last plane **/
  FairTrackParam fParamFirst;
  FairTrackParam fParamLast;
  vector<BmnFitNode> fFitNodes; // Array of fit nodes
  
  /** Reference to MCTrack**/
  Int_t fRefId;

  /** Chi2 of global track fit **/
  Double32_t fChi2;

  /** NDF of global track fit **/
  Int_t fNDF;

  /** Quality flag **/
  Int_t fFlag;

  /** Track length **/
  Double32_t fLength;

  ClassDef(BmnGlobalTrack, 1);

};


#endif
