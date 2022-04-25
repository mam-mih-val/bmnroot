#ifndef BMNEFFPROB_H
#define BMNEFFPROB_H 1

#include <iostream>
#include <TNamed.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TF1.h>
#include <TFile.h>
#include <TRandom.h>

using namespace std;

//static Double_t pdf(Double_t* x, Double_t* p) {
//    return 1.;
//}

class BmnEfficiencyProbability : public TNamed {
public:
    
    BmnEfficiencyProbability() {;}
    BmnEfficiencyProbability(TString);
    
    // Main function for use ...
    Double_t GetProbability();
    
    // Setters to be used ...
    void SetDetector(TString det) {
        fDet = det;
    }
    
    void SetStation(Int_t stat) {
        fStat = stat;
    }
    
    void SetModule(Int_t mod) {
        fMod = mod;
    }
    
    void SetZone(TString zone) {
        fZone = zone;
    }

    virtual ~BmnEfficiencyProbability() {
        if (fFile)
            delete fFile;    
    }
    

private: 
    TFile* fFile;
    TH1F* hHisto;
    
    // BM@N Central Tracker elements ....
    TString fDet;
    Int_t fStat;
    Int_t fMod;
    TString fZone;
    
    ClassDef(BmnEfficiencyProbability, 1)
};

#endif
