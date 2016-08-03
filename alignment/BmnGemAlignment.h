#ifndef BMNGEMALIGNMENT_H
#define BMNGEMALIGNMENT_H 1

#include <TMath.h>
#include <TNamed.h>
#include "BmnGemStripReadoutModule.h"
#include "BmnGemStripDigit.h"
#include "BmnGemStripHit.h"
#include <TChain.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include "BmnGemTrack.h"
#include "FairTrackParam.h"
#include "BmnAlignmentContainer.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunSummer2016.h"
#include <TH2.h>
#include <limits.h>
#include <TString.h>

using namespace std;
using namespace TMath;

class BmnGemAlignment : public TNamed {
public:

    BmnGemAlignment() {};
    
    // Constructor to be used in case of digits
    BmnGemAlignment(Char_t*, Char_t*);
    
    
    virtual ~BmnGemAlignment();

    // Getters
    void SetDebugInfo(Bool_t val) {
        fDebugInfo = val;
    }

    // Setters
    void SetNofEvents(Int_t val) {
        if (val < 1)
            fNumEvents = fChainIn->GetEntries();
        else 
            fNumEvents = val;   
    }
    
    void SetNstat(Int_t val) {
        fNstat = val;
    }
    
    void SetMaxNofHitsPerEvent(Int_t val) {
        fMaxNofHits = val;
    }
    
    void SetSignalToNoise(Double_t val) {
        fSignalToNoise = val;
    }
    
    void SetChi2Max(Float_t val) {
        fChi2Max = val;
    }
    
    void SetThreshold(Double_t val) {
        fThreshold = val;
    }
    
    void SetMinHitsAccepted(Int_t val) {
        fMinHitsAccepted = val;
    }
    
    void SetXhitMinMax(Float_t min, Float_t max) {
        fXhitMin = min;
        fXhitMax = max;  
    }
    
    void SetYhitMinMax(Float_t min, Float_t max) {
        fYhitMin = min;
        fYhitMax = max;  
    }
    
    void PrepareData();
    void StartMille();
    void StartPede() {};
   
   
private:

    void LineFit3D(TLorentzVector*, TLorentzVector*, TVector3&, TVector3&, TClonesArray*, TClonesArray*) {};
    Float_t LineFit3D(vector <BmnGemStripHit*>, TVector3&, TVector3&);
    void CreateTrack(TVector3, TVector3, BmnGemTrack&, FairTrackParam&, Float_t, Int_t);
    Bool_t isOneTrack(TClonesArray*);
    
    void goToStations(vector<BmnGemStripHit*>&, vector<BmnGemStripHit*>*, Int_t);
    void DeriveFoundTrackParams(vector<BmnGemStripHit*>);
    
    Char_t* fDigiFilename;
    TChain* fChainIn;
    TChain* fChainOut;
    
    TFile* fRecoFile;
    TTree* fRecoTree;
    Char_t* fRecoFileName;
        
    Int_t fNumEvents;
    Int_t fNstat;
    Int_t fMaxNofHits;
    
    Double_t fSignalToNoise;
    Double_t fThreshold;
    Float_t fChi2Max;
    Int_t fMinHitsAccepted;
    
    Float_t fXhitMin;
    Float_t fXhitMax;
    Float_t fYhitMin;
    Float_t fYhitMax;
    
    TClonesArray* fGemDigits;
    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fContainer;
    
    TClonesArray* fTrHits;
    
    Bool_t fDebugInfo;
    ClassDef(BmnGemAlignment, 1)
};

#endif