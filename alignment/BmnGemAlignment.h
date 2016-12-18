#ifndef BMNGEMALIGNMENT_H
#define BMNGEMALIGNMENT_H 1

#include  <vector>
#include  <fstream>

#include  <TChain.h>
#include  <TClonesArray.h>
#include  <TString.h>

#include  "FairTask.h"

#include  "BmnGemStripStationSet.h"
#include  "BmnGemStripStationSet_RunSummer2016.h"
#include  "BmnGemStripStationSet_RunWinter2016.h"

using namespace std;

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

    void SetResultName(TString resultName) {
        fResultName = resultName;
    }

    void SetRunType(TString type) {
        fRunType = type;
        if (fRunType == "")
            Fatal("BmnGemReco()", "Specify the run type!!!!!!!!!");
        if (fRunType == "beam") {
            fBeamRun = kTRUE;
            fAlignmentType = "xy";
        } else {
            fBeamRun = kFALSE;
            fAlignmentType = "xyz";
        }
        cout << "Type " + fAlignmentType + " established" << endl;
    }

    void SetStatNumFixed(TString* st) {
        cout << "Alignment conditions: " << endl;
        for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++) {
            cout << iStat << " " + st[iStat] << endl;
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

    void SetGeometry(BmnGemStripConfiguration::GEM_CONFIG config) {
        fGeometry = config;
    }

    void SetChi2MaxPerNDF(Double_t val) {
        fChi2MaxPerNDF = val;
    }

    void SetMinHitsAccepted(Int_t val) {
        fMinHitsAccepted = val;
    }

    void SetTxMinMax(Double_t min, Double_t max) {
        fTxMin = min;
        fTxMax = max;
    }

    void SetTyMinMax(Double_t min, Double_t max) {
        fTyMin = min;
        fTyMax = max;
    }

    void SetUseRealHitErrors(Bool_t flag) {
        fIsUseRealHitErrors = flag;
    }

    void SetUseRegularization(Bool_t flag) {
        fIsRegul = flag;
    }

    void SetHugecut(Double_t val) {
        fHugecut = val;
    }

    void SetChisqcut(Double_t val1, Double_t val2) {
        fChisqcut[0] = val1;
        fChisqcut[1] = val2;
    }

    void SetEntriesPerParam(Int_t entries) {
        fEntries = entries;
    }

    void SetOutlierdownweighting(Int_t n) {
        fOutlierdownweighting = n;
    }

    void SetDwfractioncut(Double_t fraction) {
        fDwfractioncut = fraction;
    }

    void SetFixDetector(Bool_t flag1, Bool_t flag2, Bool_t flag3) {
        fFixX = flag1;
        fFixY = flag2;
        fFixZ = flag3;
    }
    
    void SetUseTrackWithMinChi2(Bool_t flag) {
        fUseTrackWithMinChi2 = flag;
    }

    void StartMille();
    void StartPede();

private:

    void BinFilePede();
    void DebugInfo(Int_t, Int_t, Int_t, Int_t, Double_t*, Double_t*, Double_t, Double_t);
    void ReadPedeOutput(ifstream&, vector <Double_t>&);
    void MakeSteerFile();

    TString fRecoFileName;

    TClonesArray* fGemHits;
    TClonesArray* fGemTracks;
    TClonesArray* fContainer;

    Double_t fPreSigma;
    Double_t fAccuracy;
    UInt_t fNumOfIterations;
    vector <Int_t> fFixedStats;

    Bool_t fDebugInfo;
    Bool_t fBeamRun; // if true then it corresponds to no-target and no-field runs
    // middle part of the result file name that identifies the variant (run and addInfo)
    TString fResultName;
    TString fRunType;
    TString fSteerFileName;

    TString fAlignmentType;
    TString fCommandToRunPede;

    Int_t fNGL;

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
    BmnGemStripConfiguration::GEM_CONFIG fGeometry;

    Bool_t fIsUseRealHitErrors;  // errors are taken from hit finder algorithm
    Bool_t fUseTrackWithMinChi2; // in case of target select a track with man.value of chi2

    // Restrictions on track params
    Double_t fChi2MaxPerNDF;
    Double_t fMinHitsAccepted;
    Double_t fTxMin;
    Double_t fTxMax;
    Double_t fTyMin;
    Double_t fTyMax;

    // Millepede params
    Bool_t fIsRegul; // use regularization or not
    Double_t fHugecut; // cut factor in iteration 0
    Double_t fChisqcut[2]; // cut factor in iterations 1 and 2
    Int_t fEntries; // lower limit on number of entries/parameter
    Int_t fOutlierdownweighting; // number of internal iterations (> 1)
    Double_t fDwfractioncut; // reject all cases with a down-weight fraction >= val

    // Fix overall shift in x,y,z-directions by constraints like a1 + a2 + ... + aN = 0
    Bool_t fFixX;
    Bool_t fFixY;
    Bool_t fFixZ; // meaningless in case of 2d-alignment (true/false)

    ClassDef(BmnGemAlignment, 1)
};

#endif
