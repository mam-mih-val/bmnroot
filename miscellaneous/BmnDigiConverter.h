#ifndef BMNDIGICONVERTER_H
#define BMNDIGICONVERTER_H 1

#include "FairTask.h"
#include "FairRootManager.h"

#include "BmnGemStripDigit.h"
#include "BmnSiliconDigit.h"
#include "BmnTrigDigit.h"
#include "BmnTof1Digit.h"
#include "BmnEventHeader.h"
#include "FairEventHeader.h"

#include <TClonesArray.h>
#include <TString.h>
#include <TMath.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TLeaf.h>
#include <TBranch.h>

#include "BmnSiliconStationSet.h"
#include "BmnGemStripStationSet.h"

#include <vector>
#include <map>

using namespace std;
using namespace TMath;

class BmnDigiConverter : public FairTask {
public:

    BmnDigiConverter();

    virtual ~BmnDigiConverter();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    void SetDetectors(Bool_t trig, Bool_t gem, Bool_t sil, Bool_t tof400) {
        isTrig = trig;
        isGem = gem;
        isSil = sil;    
        isTof400 = tof400;    
    }

private:

    UInt_t fEventNo;
    
    TClonesArray* fBmnHeaderIn;
    TClonesArray* fBmnHeaderOut;
    
    // Common detectors (BM@N + SRC) - GEM, SILICON, TOF400 
    // input
    TString fGemBranchIn;
    TString fSiBranchIn;
    TString fTOF400BranchIn;
    // output 
    TString fGemBranchOut;
    TString fSiBranchOut;   
    TString fTOF400BranchOut;
    // arrays for input and output
    TClonesArray* fGemDigitsIn;
    TClonesArray* fSiDigitsIn;
    TClonesArray* fTOF400DigitsIn;
    TClonesArray* fGemDigitsOut;
    TClonesArray* fSiDigitsOut;
    TClonesArray* fTOF400DigitsOut;
    
    // Common triggers (BM@N + SRC) - BC1, BC2, VC
    // input
    TString fBC1BranchIn;
    TString fBC2BranchIn;
    TString fVetoBranchIn;
    // output
    TString fBC1BranchOut;
    TString fBC2BranchOut;
    TString fVetoBranchOut;
    // arrays for input and output
    TClonesArray* fBC1In;
    TClonesArray* fBC2In;
    TClonesArray* fVetoIn;
    TClonesArray* fBC1Out;
    TClonesArray* fBC2Out;
    TClonesArray* fVetoOut;
    
    // SRC triggers - BC3, BC4, X1L, X2L, Y1L, Y2L, X1R, X2R, Y1R, Y2R
    // input
    TString fBC3BranchIn;
    TString fBC4BranchIn;
    TString fX1LBranchIn;
    TString fX2LBranchIn;
    TString fY1LBranchIn;
    TString fY2LBranchIn;
    TString fX1RBranchIn;
    TString fX2RBranchIn;
    TString fY1RBranchIn;
    TString fY2RBranchIn;   
    // output 
    TString fBC3BranchOut;
    TString fBC4BranchOut;
    TString fX1LBranchOut;
    TString fX2LBranchOut;
    TString fY1LBranchOut;
    TString fY2LBranchOut;
    TString fX1RBranchOut;
    TString fX2RBranchOut;
    TString fY1RBranchOut;
    TString fY2RBranchOut;   
    // arrays for input and output
    TClonesArray* fBC3In;
    TClonesArray* fBC4In;
    TClonesArray* fX1LIn;
    TClonesArray* fX2LIn;
    TClonesArray* fY1LIn;
    TClonesArray* fY2LIn;
    TClonesArray* fX1RIn;
    TClonesArray* fX2RIn;
    TClonesArray* fY1RIn;
    TClonesArray* fY2RIn;    
    TClonesArray* fBC3Out;
    TClonesArray* fBC4Out;
    TClonesArray* fX1LOut;
    TClonesArray* fX2LOut;
    TClonesArray* fY1LOut;
    TClonesArray* fY2LOut;
    TClonesArray* fX1ROut;
    TClonesArray* fX2ROut;
    TClonesArray* fY1ROut;
    TClonesArray* fY2ROut;
    
    // BM@N triggers
    // input
    TString fSiTrigBranchIn;   
    TString fBDBranchIn;     
    // output 
    TString fSiTrigBranchOut;   
    TString fBDBranchOut;       
    // arrays for input and output
    TClonesArray* fSiTrigIn;
    TClonesArray* fBDIn;
    TClonesArray* fSiTrigOut;
    TClonesArray* fBDOut; 

    BmnGemStripStationSet* fDetectorGEM; // GEM-geometry
    BmnSiliconStationSet* fDetectorSI; // SI-geometry   

    // map <Int_t, Int_t> fSiMods;
    map <Int_t, Int_t> fGemStats;
    map <Int_t, Int_t> fSilStats;
    map <TClonesArray*, TClonesArray*> fTriggers;
       
    inline Int_t GemStatPermutation(Int_t stat) {
        return fGemStats.find(stat)->second;
    }
    
    inline Int_t SiliconStatPermutation(Int_t stat) {
        return fSilStats.find(stat)->second;
    }
    
    void ConvertTriggers(map <TClonesArray*, TClonesArray*>);
    
    Bool_t isTrig;
    Bool_t isGem;
    Bool_t isSil;    
    Bool_t isTof400;    

    FairRootManager* ioman;
    
    Bool_t isSRC;
    Bool_t isBMN;
    
    void Run7(Int_t*, Int_t*, Int_t*, Int_t*);

    ClassDef(BmnDigiConverter, 1);
};

#endif
