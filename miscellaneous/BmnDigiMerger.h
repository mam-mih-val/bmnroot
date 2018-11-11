#ifndef BMNDIGIMERGER_H
#define BMNDIGIMERGER_H 1

#include "FairTask.h"
#include "FairRootManager.h"

#include "BmnGemStripDigit.h"
#include "BmnSiliconDigit.h"
#include "BmnTrigDigit.h"
#include "BmnEventHeader.h"
#include "FairEventHeader.h"

#include <TClonesArray.h>
#include <TString.h>
#include <TMath.h>
//#include <TROOT.h>
//#include <TSystem.h>
//#include <TLeaf.h>
//#include <TBranch.h>
//
//#include "BmnSiliconStationSet.h"
//#include "BmnGemStripStationSet.h"

#include <vector>
#include <map>

using namespace std;
using namespace TMath;

class BmnDigiMerger : public FairTask {
public:

    BmnDigiMerger() {};
    BmnDigiMerger(TString);

    virtual ~BmnDigiMerger() {}

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
 

private:
    // Inner tracker
    TString fGemBranchIn;
    TString fSiBranchIn;

    TString fGemBranchOut;
    TString fSiBranchOut;

    // Triggers
    TString fSiBranch;
    TString fVetoBranch;
    TString fBC1Branch;
    TString fBC2Branch;
    TString fBC3Branch;
    TString fBDBranch;
    
    TClonesArray* fSiIn;
    TClonesArray* fVetoIn;
    TClonesArray* fBC1In;
    TClonesArray* fBC2In;
    TClonesArray* fBC3In;
    TClonesArray* fBDIn;

    // Inner tracker (in)
    TClonesArray* fGemDigitsIn;
    TClonesArray* fSiDigitsIn;

     // Inner tracker (out)
    TClonesArray* fGemDigitsOut;
    TClonesArray* fSiDigitsOut;

    // Triggers (out)
    TClonesArray* fSiOut;
    TClonesArray* fVetoOut;
    TClonesArray* fBC1Out;
    TClonesArray* fBC2Out;
    TClonesArray* fBC3Out;
    TClonesArray* fBDOut;
    
    TChain* fChain;
    TClonesArray* fSilDigits;
    UInt_t fCurrEvent; 
    
    TClonesArray* fBmnHeaderIn;  
    TClonesArray* fBmnHeaderOut;  

    ClassDef(BmnDigiMerger, 1);
};

#endif
