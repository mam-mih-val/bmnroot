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
#include <TSystem.h>
#include <TFile.h>
#include "BmnMille.h"

#include <iomanip>
#include <algorithm>

using namespace std;
using namespace TMath;

class BmnGemAlignment : public TNamed {
public:

    BmnGemAlignment() {
    };

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

    void SetChi2Max(Double_t val) {
        fChi2Max = val;
    }

    void SetThreshold(Double_t val) {
        fThreshold = val;
    }

    void SetSignalToNoise(Double_t val1, Double_t val2, Double_t val3,
            Double_t val4, Double_t val5, Double_t val6, Double_t val7) {
        const Int_t nStat = 7;
        Double_t ratios[nStat] = {val1, val2, val3, val4, val5, val6, val7};
        for (Int_t iStat = 0; iStat < nStat; iStat++)
            fThresh[iStat] = ratios[iStat];
    }

    void SetMinHitsAccepted(Int_t val) {
        fMinHitsAccepted = val;
    }

    void SetXhitMinMax(Double_t min, Double_t max) {
        fXhitMin = min;
        fXhitMax = max;
    }

    void SetYhitMinMax(Double_t min, Double_t max) {
        fYhitMin = min;
        fYhitMax = max;
    }

    void SetAlignmentDim(TString dim) {
        if (dim == "xy" || dim == "xyz") {
            fAlignmentType = dim;
            cout << "Type " << fAlignmentType << " established" << endl;
        } else {
            cout << "Specify a correct type of alignment" << endl;
            throw;
        }
    }

    void SetSteerFile(const Char_t* file) {
        fSteerFileName = file;
    }
    
    void SetTxMinMax(Double_t min, Double_t max) {
        fTxMin = min;
        fTxMax = max;
    }
    
    void SetTyMinMax(Double_t min, Double_t max) {
        fTyMin = min;
        fTyMax = max;
    }

    void PrepareData();
    void StartMille();
    void StartPede(Bool_t flag) {
        if (flag)
            system(fCommandToRunPede);
        else
            return;
    }


private:

    const Char_t* GetSteerFileName() {
        return fSteerFileName;
    }

    void LineFit3D(TLorentzVector*, TLorentzVector*, TVector3&, TVector3&, TClonesArray*, TClonesArray*) {
    };
    Double_t LineFit3D(vector <BmnGemStripHit*>, TVector3&, TVector3&);
    void CreateTrack(TVector3, TVector3, BmnGemTrack&, FairTrackParam&, Double_t, Int_t);
    Bool_t isOneTrack(TClonesArray*);

    void goToStations(vector<BmnGemStripHit*>&, vector<BmnGemStripHit*>*, Int_t);
    void DeriveFoundTrackParams(vector<BmnGemStripHit*>);

    Bool_t CheckSignalToNoizeRatio(BmnGemStripDigit*) {};

    Char_t* fDigiFilename;
    TChain* fChainIn;
    TChain* fChainOut;

    TFile* fRecoFile;
    TTree* fRecoTree;
    Char_t* fRecoFileName;
 
    Int_t fNumEvents;
    Int_t fNstat;
    Int_t fStatUsed;
    vector <Int_t> fNumStatUsed;
    Int_t fMaxNofHits;

    Double_t fSignalToNoise;
    Double_t fThreshold;

    Double_t fThresh[7];

    Double_t fChi2Max;
    Int_t fMinHitsAccepted;

    Double_t fXhitMin;
    Double_t fXhitMax;
    Double_t fYhitMin;
    Double_t fYhitMax;
    Double_t fTxMin;
    Double_t fTxMax;
    Double_t fTyMin;
    Double_t fTyMax;

    TClonesArray* fGemDigits;
    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fContainer;

    TClonesArray* fTrHits;

    Bool_t fDebugInfo;
    const Char_t* fSteerFileName;

    TString fAlignmentType;
    TString fCommandToRunPede;
    ClassDef(BmnGemAlignment, 1)
};

#endif