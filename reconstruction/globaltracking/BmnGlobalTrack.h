/**  BmnGlobalTrack.h
 *@author S.Merts
 **
 ** Data class for Global BMN track. Data level RECO.
 **
 **/

#ifndef BmnGlobalTrack_H_
#define BmnGlobalTrack_H_ 1

#include "BmnTrack.h"

enum PidParticles{
    PidProton,
    PidPion,
    PidKaon,
    PidElectron,
    PidDeuteron,
    PidTriton,
    PidHelium3,
    PidHelium4,
    EndPidEnum
};

class BmnGlobalTrack : public BmnTrack{
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
    vector<Int_t> GetVectorCscHitIndices() const { return fCscHit; }
    Int_t GetCscHitIndex(Int_t idx) const { return fCscHit.at(idx); }
    Int_t GetUpstreamTrackIndex() const { return fUpstreamTrack; }

    Double_t GetBeta(Int_t tofID) const { return (tofID == 1) ? fBeta400 : (tofID == 2) ? fBeta700 : -1000.0; }
    Double_t GetMass2(Int_t tofID);
    Double_t GetdQdNUpper() const { return fdQdNUpper; }
    Double_t GetdQdNLower() const { return fdQdNLower; }
    Int_t GetA() { return fA; }
    Int_t GetZ() { return fZ; }
    Double_t GetAoverZ() { return (Double_t)fA / fZ; }
    Int_t GetPDG() { return fPDG; }

    vector<Double_t> GetPidVectorTof400() {return fPidTof400;}
    vector<Double_t> GetPidVectorTof700() {return fPidTof700;}

    PidParticles GetParticleTof400();
    PidParticles GetParticleTof700();
    PidParticles GetParticle();
    
    Int_t GetRefIndex() { return fRefIndex; }
    Double_t GetChi2InVertex() { return fChi2InVertex; }


    Bool_t IsPrimary() const { return fIsPrimary;}

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
    //void SetCscHitIndex(Int_t iCsc) { fCscHit = iCsc; }
    void AddCscHitIndex(Int_t i, Int_t iCsc) { fCscHit[i] = iCsc; }
    void SetUpstreamTrackIndex(Int_t iUp) { fUpstreamTrack = iUp; }

    void SetBeta(Double_t b, Int_t tofID);
    void SetA(Int_t a) { fA = a; }
    void SetZ(Int_t z) { fZ = z; }
    void SetPDG(Int_t pdg) { fPDG = pdg; }
    void SetdQdNLower(Double_t q) { fdQdNLower = q; }
    void SetdQdNUpper(Double_t q) { fdQdNUpper = q; }

    void SetPrimaryMark(Bool_t p) { fIsPrimary = p; }

    void SetPidVectorTof400(vector<Double_t> v) {fPidTof400 = v;}
    void SetPidVectorTof700(vector<Double_t> v) {fPidTof700 = v;}
    
    void SetRefIndex(Int_t idx) { fRefIndex = idx; }
    void SetChi2InVertex(Double_t chi) { fChi2InVertex = chi; }

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
    Int_t fUpstreamTrack;
    
    //vector of indices for 4 zones: 
    //0. Small CSC between Magnet and TOF400
    //1. Small CSC between TOF400 and TOF700
    //2. Big CSC between TOF400 and TOF700
    //3. Big CSC between TOF700 and FHCal
    // So size of vector is 4
    vector<Int_t> fCscHit; 
        

    // members for PID
    Double_t fBeta400;    // l/t/c //for tof-400
    Double_t fBeta700;    // l/t/c //for tof-700
    Double_t fdQdNUpper;  //cluster charge over number of hits in GEM detector for X' strips
    Double_t fdQdNLower;  //cluster charge over number of hits in GEM detector for X strips
    Int_t fA;
    Int_t fZ;
    Int_t fPDG;
    Double_t fChi2InVertex;
   
    // fPidTof400 - probability for particle species TOF400
    // fPidTof700 - probability for particle species TOF700
    vector<Double_t> fPidTof400;
    vector<Double_t> fPidTof700;

    Bool_t fIsPrimary;  // decision after vertex finder task
    
    Int_t fRefIndex; //Index of MC track

    ClassDef(BmnGlobalTrack, 3);
};

#endif
