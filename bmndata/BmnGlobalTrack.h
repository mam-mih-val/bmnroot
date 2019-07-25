/**  BmnGlobalTrack.h
 *@author S.Merts
 **
 ** Data class for Global BMN track. Data level RECO.
 **
 **/

#ifndef BmnGlobalTrack_H_
#define BmnGlobalTrack_H_ 1

#include "BmnTrack.h"

class BmnGlobalTrack : public BmnTrack {
   public:
    /** Default constructor **/
    BmnGlobalTrack();

    /** Destructor **/
    virtual ~BmnGlobalTrack();

    /** Accessors **/
    Int_t GetGemTrackIndex() const { return fGemTrack; }
    Int_t GetSilTrackIndex() const { return fSilTrack; }
    Int_t GetSsdTrackIndex() const { return fSsdTrack; }
    Int_t GetSilHitIndex() const { return -1; }
    Int_t GetTof1HitIndex() const { return fTof1Hit; }
    Int_t GetTof2HitIndex() const { return fTof2Hit; }
    Int_t GetDch1TrackIndex() const { return fDch1Track; }
    Int_t GetDch2TrackIndex() const { return fDch2Track; }
    Int_t GetDchTrackIndex() const { return fDchTrack; }
    Int_t GetMwpc1TrackIndex() const { return fMwpc1Track; }
    Int_t GetMwpc2TrackIndex() const { return fMwpc2Track; }
    Int_t GetCscHitIndex() const { return fCscHit; }

    Double_t GetTime() const { return fTime; }
    Double_t GetBeta() const { return fBeta; }
    Double_t GetMass2() const { return fMass2; }
    Double_t GetdQdNUpper() const { return fdQdNUpper; }
    Double_t GetdQdNLower() const { return fdQdNLower; }

    Bool_t IsPrimary() const { return fIsPrimary; }

    /** Modifiers **/
    void SetGemTrackIndex(Int_t iGem) { fGemTrack = iGem; }
    void SetSilTrackIndex(Int_t iSil) { fSilTrack = iSil; }
    void SetSsdTrackIndex(Int_t iSsd) { fSsdTrack = iSsd; }
    void SetTof1HitIndex(Int_t iTof1Hit) { fTof1Hit = iTof1Hit; }
    void SetTof2HitIndex(Int_t iTof2Hit) { fTof2Hit = iTof2Hit; }
    void SetDch1TrackIndex(Int_t iDch1Track) { fDch1Track = iDch1Track; }
    void SetDch2TrackIndex(Int_t iDch2Track) { fDch2Track = iDch2Track; }
    void SetDchTrackIndex(Int_t iDch) { fDchTrack = iDch; }
    void SetMwpc1TrackIndex(Int_t iMwpc) { fMwpc1Track = iMwpc; }
    void SetMwpc2TrackIndex(Int_t iMwpc) { fMwpc2Track = iMwpc; }
    void SetCscHitIndex(Int_t iCsc) { fCscHit = iCsc; }

    void SetTime(Double_t t) { fTime = t; }
    void SetBeta(Double_t b) { fBeta = b; }
    void SetMass2(Double_t m2) { fMass2 = m2; }
    void SetdQdNLower(Double_t q) { fdQdNLower = q; }
    void SetdQdNUpper(Double_t q) { fdQdNUpper = q; }

    void SetPrimaryMark(Bool_t p) { fIsPrimary = p; }

    /** Output to screen **/
    void Print() const;

   private:
    /** Indices of local parts of global track **/
    Int_t fGemTrack;
    Int_t fSsdTrack;
    Int_t fSilTrack;
    Int_t fTof1Hit;
    Int_t fTof2Hit;
    Int_t fDch1Track;
    Int_t fDch2Track;
    Int_t fDchTrack;
    Int_t fMwpc1Track;
    Int_t fMwpc2Track;
    Int_t fCscHit;

    // members for PID
    Double_t fTime;       // information from TOF
    Double_t fBeta;       // l/t/c
    Double_t fMass2;      // p^2/gamma^2/beta^2
    Double_t fdQdNUpper;  //cluster charge over number of hits in GEM detector for X' strips
    Double_t fdQdNLower;  //cluster charge over number of hits in GEM detector for X strips

    Bool_t fIsPrimary;  // decision after vertex finder task

    ClassDef(BmnGlobalTrack, 1);
};

#endif
