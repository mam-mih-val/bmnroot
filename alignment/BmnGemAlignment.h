#ifndef BMNGEMALIGNMENT_H
#define BMNGEMALIGNMENT_H 1

#include <TMath.h>
#include <TNamed.h>
#include <TChain.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include "BmnGemTrack.h"
#include "FairTrackParam.h"
#include "BmnAlignmentContainer.h"
#include <TH2.h>
#include <limits.h>
#include <TString.h>
#include <TSystem.h>
#include <TFile.h>
#include "BmnMille.h"
#include "BmnMath.h"
#include "FairTask.h"
#include "BmnGemAlignmentCorrections.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunSummer2016.h"

#include <iomanip>
#include <stdio.h>
#include <algorithm>
#include <sstream>
#include <TRandom1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>

using namespace std;
using namespace TMath;

class BmnGemAlignment : public FairTask {
public:

    BmnGemAlignment(); // Default constructor

    virtual ~BmnGemAlignment() {
        delete fDetector;
    }

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish() {
    }

    void SetDebugInfo(Bool_t val) {
        fDebugInfo = val;
    }

    void SetRunType(TString type) {
        fRunType = type;
        if (fRunType == "")
            Fatal("BmnGemReco()", "Specify a run type!!!!!!!!!");

        if (fRunType == "beam") {
            fBeamRun = kTRUE;
            fAlignmentType = "xy";
        }
        else {
            fBeamRun = kFALSE;
            fAlignmentType = "xyz";
        }

        cout << "Type " << fAlignmentType << " established" << endl;
    }

    void SetNumIterations(Int_t num) {
        fIterationsNum = num;
    }

    // Strongly depends on current geometry

    void SetStatNumFixed(TString* st) {
        cout << "Alignment conditions: " << endl;
        for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
            cout << iStat << " " << st[iStat] << endl;
            if (st[iStat] == "fixed")
                fFixedStats.push_back(iStat);
        }
    }

    void SetPreSigma(Double_t presigma) {
        fPreSigma = presigma;
    }

    void SetAccuracy(Double_t accuracy) {
        fAccuracy = accuracy;
    }

    void SetNumOfIterations(UInt_t num) {
        fNumOfIterations = num;
    }
    
    void SetAlignmentTypeByHands(TString type) {
        fAlignmentType = type;
    }

    void StartMille();
    void StartPede();

private:

    void BinFilePede();
    void DebugInfo(Int_t, Int_t, Int_t, Int_t, Double_t*, Double_t*, Double_t, Double_t);
    void ReadPedeOutput(ifstream&, Int_t);
    void ReadPedeOutput(ifstream&, vector <Double_t>&);
    void MakeSteerFile(Int_t);

    TString fRecoFileName;

    //    vector <Int_t> fNumStatUsed;

    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fContainer;

    Double_t fSigma;
    Double_t fPreSigma;
    Double_t fAccuracy;
    UInt_t fNumOfIterations;
    vector <Int_t> fFixedStats;

    Bool_t fDebugInfo;
    Bool_t fBeamRun; // if true then it corresponds to 61 - 65 files
    TString fRunType;
    TString fSteerFileName;

    TString fAlignmentType;
    TString fCommandToRunPede;

    Int_t fNGL;
    Int_t fIterationsNum; // Number of iterations for PEDE to update align.params

    TString hitsBranch;
    TString tracksBranch;
    TString tracksSelectedBranch;
    TString alignCorrBranch;
    Int_t nSelectedTracks;

    static Int_t fCurrentEvent;
    FILE* fin_txt;

    TString fName;
    TChain* fChain;

    TClonesArray* fAlignCorr;
    BmnGemStripStationSet* fDetector; // Detector geometry
    // BmnGemAlignmentCorrections* fCorrections;

    ClassDef(BmnGemAlignment, 1)
};

#endif