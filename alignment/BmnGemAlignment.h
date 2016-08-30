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
#include <stdio.h>
#include <algorithm>
#include <sstream> 
#include <TRandom1.h>
#include <TGraphErrors.h>

using namespace std;
using namespace TMath;

class BmnGemAlignment : public TNamed {
public:

    BmnGemAlignment() {
    };

    // Constructor to be used in case of digits
    BmnGemAlignment(Char_t*, Char_t*, Bool_t);


    virtual ~BmnGemAlignment();

    // Getters

    void SetDebugInfo(Bool_t val) {
        fDebugInfo = val;
    }

    TString GetRunType() {
        return fRunType;
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

    void SetMaxHitsAccepted(Int_t val) {
        fMaxHitsAccepted = val;
    }

    void SetXMinMax(Double_t min, Double_t max) {
        fXMin = min;
        fXMax = max;
    }

    void SetYMinMax(Double_t min, Double_t max) {
        fYMin = min;
        fYMax = max;
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
    
    TString GetAlignmentDim() {
        return fAlignmentType;
    }

    void SetSteerFile(vector <TString> fileNames) {
        fSteerFileNames = fileNames;
    }

    void SetTxMinMax(Double_t min, Double_t max) {
        fTxMin = min;
        fTxMax = max;
    }

    void SetTyMinMax(Double_t min, Double_t max) {
        fTyMin = min;
        fTyMax = max;
    }

    void SetXresMax(Double_t val) {
         fXresMax = val;
    }

    void SetYresMax(Double_t val) {
        fYresMax = val;
    }

    void SetRunType(TString type) {
        fRunType = type;
        if (type == "beam")
            fBeamRun = kTRUE;
        else if (type == "target")
            fBeamRun = kFALSE;
        else {
            cout << "Specify a run type" << endl;
            throw;
        }
    }

    void PrepareData();
    void StartMille();
    void StartPede();

private:

    vector <TString> GetSteerFileNames() {
        return fSteerFileNames;
    }

    Double_t LineFit3D(vector <BmnGemStripHit*>, TVector3&, TVector3&);
    void CreateTrack(TVector3, TVector3, BmnGemTrack&, FairTrackParam&, Double_t, Int_t);
    Bool_t isOneTrack(TClonesArray*);

    void goToStations(vector<BmnGemStripHit*>&, vector<BmnGemStripHit*>*, Int_t);
    void DeriveFoundTrackParams(vector<BmnGemStripHit*>);

    Bool_t CheckSignalToNoizeRatio(BmnGemStripDigit*) {
    };

    void AlignmentdXdY(ifstream&, Int_t, Int_t, Int_t, Int_t, TString);
    void DebugInfo(Int_t, Int_t, Int_t, Double_t*, Double_t*, Double_t, Double_t);
    
    void GraphDrawAttibuteSetter(TGraphErrors*, TString);

    BmnAlignmentContainer* fAlignCont;

    Char_t* fDigiFilename;
    TChain* fChainIn;
    // TChain* fChainOut;

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
    Int_t fMaxHitsAccepted;

    Double_t fXMin;
    Double_t fXMax;
    Double_t fYMin;
    Double_t fYMax;
    Double_t fTxMin;
    Double_t fTxMax;
    Double_t fTyMin;
    Double_t fTyMax;
    Double_t fXresMax;
    Double_t fYresMax;

    TClonesArray* fGemDigits;
    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fContainer;
    TClonesArray* fTrHits;

    Double_t fSigmaX;
    Double_t fSigmaY;
    Int_t fNumLabels;

    Bool_t fDebugInfo;
    Bool_t fOnlyMille;
    Bool_t fBeamRun; // if true then it corresponds to 61 - 65 files 
    TString fRunType;
    vector <TString> fSteerFileNames;

    TString fAlignmentType;
    TString fCommandToRunPede;
    
    ClassDef(BmnGemAlignment, 1)
};

#endif