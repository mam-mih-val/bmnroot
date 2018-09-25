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

#include "BmnTrack.h"
#include "BmnFitNode.h"

class BmnGlobalTrack : public BmnTrack {

 public:

  /** Default constructor **/
  BmnGlobalTrack();


  /** Destructor **/
  virtual ~BmnGlobalTrack();


  /** Accessors **/
  Int_t GetGemTrackIndex()  const { return fGemTrack;  }
  Int_t GetSilHitIndex()    const { return fSilHit;    }
  vector <Int_t> GetSilHitIndices()  { return fSilHits;}
  Int_t GetTof1HitIndex()   const { return fTof1Hit;   }
  Int_t GetTof2HitIndex()   const { return fTof2Hit;   }
  Int_t GetDch1HitIndex()   const { return fDch1Hit;   }
  Int_t GetDch2HitIndex()   const { return fDch2Hit;   }
  Int_t GetDchTrackIndex()  const { return fDchTrack;  }
  Int_t GetMwpcTrackIndex() const { return fMwpcTrack; }
  
  BmnFitNode* GetFitNode(Int_t index) {return &fFitNodes[index];}
  vector<BmnFitNode>& GetFitNodes() {return fFitNodes;}

  /** Modifiers **/
  void SetGemTrackIndex(Int_t iGem)    { fGemTrack  = iGem;     }
  void SetSilHitIndex(Int_t iSilHit)   { fSilHit   = iSilHit;   }
  void SetSilHitIndices(Int_t iSilHit) { fSilHits.push_back(iSilHit); }
  void SetTof1HitIndex(Int_t iTof1Hit) { fTof1Hit   = iTof1Hit; }
  void SetTof2HitIndex(Int_t iTof2Hit) { fTof2Hit   = iTof2Hit; }
  void SetDch1HitIndex(Int_t iDch1Hit) { fDch1Hit   = iDch1Hit; }
  void SetDch2HitIndex(Int_t iDch2Hit) { fDch2Hit   = iDch2Hit; }
  void SetDchTrackIndex(Int_t iDch)    { fDchTrack  = iDch;     }
  void SetMwpcTrackIndex(Int_t iMwpc)  { fMwpcTrack = iMwpc;    }
  
  void SetFitNodes(const vector<BmnFitNode>& nodes) {fFitNodes = nodes;}


  /** Output to screen **/
  void Print() const;


 private:

  /** Indices of local parts of global track **/
  Int_t fGemTrack;
  Int_t fSilHit;
  Int_t fTof1Hit;
  Int_t fTof2Hit;
  Int_t fDch1Hit;
  Int_t fDch2Hit;
  Int_t fDchTrack;
  Int_t fMwpcTrack;
  
  vector<BmnFitNode> fFitNodes; // Array of fit nodes
  vector <Int_t> fSilHits;

  ClassDef(BmnGlobalTrack, 1);

};


#endif
