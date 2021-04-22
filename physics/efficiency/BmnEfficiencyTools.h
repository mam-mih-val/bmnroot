#include <iostream>
#include <TNamed.h>

#include <BmnDataTriggerInfo.h>
#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>
#include <BmnInnerTrackerGeometryDraw.h>

#include <FairRunAna.h>
#include <TGraph.h>

#ifndef BMNEFFTOOLS_H
#define BMNEFFTOOLS_H 1

using namespace std;

class BmnEfficiencyTools : public TNamed {
public:

    BmnEfficiencyTools();
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

    void SetDstPath(TString path) {
        fDataPath = path;
    }

    //--------

    void doNormalization(Bool_t flag) {
        isDoNormalization = flag;
    }

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

    BmnDataTriggerInfo* fRunTrigInfo;

    TString fDataPath;
    TString fDstName;

    Bool_t isDoNormalization;
    Bool_t isMc;
    
    
private: 
    void DoNormalization(TH1F*);

    ClassDef(BmnEfficiencyTools, 1)
};

#endif