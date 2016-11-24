#ifndef BMNGEMRECO_H
#define BMNGEMRECO_H 1

#include <TMath.h>
#include <TNamed.h>
#include "BmnGemStripModule.h"
#include "BmnGemStripLayer.h"
#include "BmnGemStripDigit.h"
#include "BmnGemStripHit.h"
#include <TChain.h>
#include <TTree.h>
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
#include "BmnMath.h"
#include "FairTask.h"

#include <iomanip>
#include <stdio.h>
#include <algorithm>
#include <sstream>
#include <TRandom1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>

using namespace std;
using namespace TMath;

class BmnGemReco : public FairTask {
public:

    BmnGemReco(); // Default constructor

    virtual ~BmnGemReco() {
    };

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void SetDebugInfo(Bool_t val) {
        fDebugInfo = val;
    }

    void SetMaxNofHitsPerEvent(Int_t val) {
        fMaxNofHits = val;
    }

    void SetChi2MaxPerNDF(Double_t val) {
        fChi2MaxPerNDF = val;
    }

    void SetMinHitsAccepted(Int_t val) {
        fMinHitsAccepted = val;
    }

    void SetXMinMax(Double_t min, Double_t max) {
        fXMin = min;
        fXMax = max;
    }

    void SetYMinMax(Double_t min, Double_t max) {
        fYMin = min;
        fYMax = max;
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

private:

    void goToStations(vector<BmnGemStripHit*>&, vector<BmnGemStripHit*>*, Int_t);
    void DeriveFoundTrackParams(vector<BmnGemStripHit*>);
    void ReadFileCorrections(BmnGemStripStationSet*); // read corrections from the file

    static Int_t fCurrentEvent;
    // Int_t fNstat;
    Int_t fMaxNofHits;

    Double_t fChi2MaxPerNDF;
    Int_t fMinHitsAccepted;

    Double_t fXMin;
    Double_t fXMax;
    Double_t fYMin;
    Double_t fYMax;
    Double_t fTxMin;
    Double_t fTxMax;
    Double_t fTyMin;
    Double_t fTyMax;

    TClonesArray* fGemDigits;
    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fContainer;

    Bool_t fDebugInfo;
    Bool_t fBeamRun; // if true then it corresponds to 61 - 65 files

    Double_t*** corr; // array to store the corrections

    TString hitsBranch;
    TString tracksBranch;
    TString tracksSelectedBranch;

    BmnGemStripStationSet* fDetector; // Detector geometry

    ClassDef(BmnGemReco, 1)
};

#endif