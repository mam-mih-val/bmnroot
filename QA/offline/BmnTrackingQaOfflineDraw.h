#include <iostream>
#include <TNamed.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TFitResult.h>
#include <TF1.h>
#include <TH2.h>
#include <TStyle.h>

#ifndef BMNTRACKINGQAOFFLINEDRAW_H
#define BMNTRACKINGQAOFFLINEDRAW_H 1

using namespace std;

class BmnTrackingQaOfflineDraw : public TNamed {
public:

    BmnTrackingQaOfflineDraw();

    virtual ~BmnTrackingQaOfflineDraw() {
        ;
    }
    
    void ProcessHistos(Bool_t anal1, Bool_t anal2, Bool_t anal3);
    void PrepareHistos();
    
    TH1F***** GetH1() {
        return h;
    }
    
    TH2F* GetH2() {
        return h2;
    }
    
    TH2F* GetH3() {
        return h3;
    }
    
    TH2F***** GetH4() {
        return h4;
    }
    
    TH1F***** GetH5() {
        return h5;
    }

private:
    TH1F***** h;
    TH2F* h2;
    TH2F* h3;
    
    TH2F***** h4;
    TH1F***** h5;
    
    
public:
    TString usedOutExtension;
      
    vector <TString> fBeams;
    vector <TString> fTargets;
    vector <TString> fTriggers;

private: 
    
    ClassDef(BmnTrackingQaOfflineDraw, 1)
};

#endif