#ifndef BMNEFFTOOLS_H
#define BMNEFFTOOLS_H 1

#include <iostream>
#include <TNamed.h>
#include <TH2.h>
#include <TH1.h>

#include <BmnDataTriggerInfo.h>
#include <UniDetectorParameter.h>
#include <UniRun.h>
#include <BmnInnerTrackerGeometryDraw.h>

#include <FairRunAna.h>
#include <TGraph.h>
#include <TEfficiency.h>

using namespace std;

class BmnEfficiencyTools : public TNamed {
public:

    BmnEfficiencyTools();
    BmnEfficiencyTools(FairRunAna*);
    BmnEfficiencyTools(TString); // For MC use ...

    void SetTargets(vector <TString> targs) {
        fTargets = targs;
    }

    void SetTriggers(vector <TString> triggs) {
        fTriggers = triggs;
    }

    void SetBeams(vector <TString> beams) {
        fBeams = beams;
    }

    // Setting manual file list if needed ...

    void SetFilelist(vector <Int_t> list) {
        fManualList = list;
    }

    void SetDstPath(TString path) {
        fDataPath = path;
    }

    void SetGeometryFile(TString file) {
        fGeomFile = file;
    }

    void isL1Input(Bool_t flag) {
        isL1 = flag;
    }

    // Setting Y-ranges ...
    void SetGemYRanges(map <Int_t, vector <pair <Double_t, Double_t>>> rMap) {
        fYRangesGem = rMap;
    }
    
    void SetSiliconYRanges(map <Int_t, vector <pair <Double_t, Double_t>>> rMap) {
        fYRangesSilicon = rMap;
    }
   
    void SetOutputFile(TString f) {
        fOutFile = f;
    }
   
    //-------

    virtual ~BmnEfficiencyTools() {
        if (fInnTracker)
            delete fInnTracker;

        if (fRunTrigInfo)
            delete fRunTrigInfo;
    }

    void Process();

private:
    FairRunAna* fAna;
    BmnInnerTrackerGeometryDraw* fInnTracker;

    Int_t fPeriod;
    Int_t startRun;
    Int_t finishRun;

    vector <TString> fBeams;
    vector <TString> fTargets;
    vector <TString> fTriggers;

    vector <Int_t> fManualList;

    BmnDataTriggerInfo* fRunTrigInfo;

    TString fDataPath;
    TString fDstName;
    TString fGeomFile;
    
    TString fOutFile;

    Bool_t isMc;
    Bool_t isL1;

    TClonesArray* effGem;
    TClonesArray* effSilicon;

    // Y-ranges (GEM, SILICON)
    map <Int_t, vector <pair <Double_t, Double_t>>> fYRangesGem;
    map <Int_t, vector <pair <Double_t, Double_t>>> fYRangesSilicon;

public:
    void DoNormalization(TH1D*);

    ClassDef(BmnEfficiencyTools, 1)
};

#endif
