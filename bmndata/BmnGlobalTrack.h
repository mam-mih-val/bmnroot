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
    Int_t GetUpstreamTrackIndex() const { return fUpstreamTrack; }

    Double_t GetBeta(Int_t tofID) const { return (tofID == 1) ? fBeta400 : (tofID == 2) ? fBeta700 : -1000.0; }
    Double_t GetMass2(Int_t tofID);
    Double_t GetdQdNUpper() const { return fdQdNUpper; }
    Double_t GetdQdNLower() const { return fdQdNLower; }
    Int_t GetA() { return fA; }
    Int_t GetZ() { return fZ; }
    Double_t GetAoverZ() { return (Double_t)fA / fZ; }
    Int_t GetPDG() { return fPDG; }

    Double_t GetPDGIndexInPidWeights(Int_t index) const { return fPidWeights[index].first; }
    Double_t GetPidWeights(Int_t index, Int_t tofID) const { return fPidWeights[index].second[tofID-1]; }
    Double_t GetPidWeightByPDG(Int_t pdgCode, Int_t tofID);
    Int_t GetMaxWeightInd(Int_t tofID);
    Double_t GetMaxWeight(Int_t tofID);
    Double_t GetSumWeight(Int_t tofID);
    Int_t GetMostPossiblePDG(Int_t tofID);    

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
    void SetUpstreamTrackIndex(Int_t iUp) { fUpstreamTrack = iUp; }

    void SetBeta(Double_t b, Int_t tofID);
    void SetA(Int_t a) { fA = a; }
    void SetZ(Int_t z) { fZ = z; }
    void SetPDG(Int_t pdg) { fPDG = pdg; }
    void SetdQdNLower(Double_t q) { fdQdNLower = q; }
    void SetdQdNUpper(Double_t q) { fdQdNUpper = q; }

    void SetPrimaryMark(Bool_t p) { fIsPrimary = p; }

    void AddPDGMatch(Int_t i, Int_t pdgCode) { fPidWeights[i].first = pdgCode; }
    void SetPidWeight(Int_t tofID, Int_t index, Double_t weight) { fPidWeights[index].second[tofID-1] = weight; }
    void NormalizeWeights();
    void PrintWeights(Int_t tofID);
    void ResizePidVectors(Int_t size);


    /** Output to screen **/
    //void Print() const;

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
    Int_t fUpstreamTrack;

    // members for PID
    Double_t fBeta400;    // l/t/c //for tof-400
    Double_t fBeta700;    // l/t/c //for tof-700
    Double_t fdQdNUpper;  //cluster charge over number of hits in GEM detector for X' strips
    Double_t fdQdNLower;  //cluster charge over number of hits in GEM detector for X strips
    Int_t fA;
    Int_t fZ;
    Int_t fPDG;

    // Int_t in pair - PDG code of particle sort
    // array[0] - weight for TOF400
    // array[1] - weight for TOF700
    vector<pair<Int_t,array<Double_t,2>>> fPidWeights = vector<pair<Int_t,array<Double_t,2>>>(50);
   

    Bool_t fIsPrimary;  // decision after vertex finder task

    ClassDef(BmnGlobalTrack, 2);
};

#endif
