#ifndef BMNDIGICONVERTER_H
#define BMNDIGICONVERTER_H 1

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
    
    void SetDetectors(Bool_t trig, Bool_t gem, Bool_t sil) {
        isTrig = trig;
        isGem = gem;
        isSil = sil;    
    }

private:

    UInt_t fEventNo;
    
    TClonesArray* fBmnHeaderIn;
    TClonesArray* fBmnHeaderOut;
    
    TString fSiBranch;
    TString fVetoBranch;
    TString fBC1Branch;
    TString fBC2Branch;
    TString fBDBranch;

    TString fGemBranchIn;
    TString fSiBranchIn;

    TString fGemBranchOut;
    TString fSiBranchOut;

    TClonesArray* fGemDigitsIn;
    TClonesArray* fSiDigitsIn;

    TClonesArray* fGemDigitsOut;
    TClonesArray* fSiDigitsOut;
    
    TClonesArray* fSiIn;
    TClonesArray* fVetoIn;
    TClonesArray* fBC1In;
    TClonesArray* fBC2In;
    TClonesArray* fBDIn;
    
    TClonesArray* fSiOut;
    TClonesArray* fVetoOut;
    TClonesArray* fBC1Out;
    TClonesArray* fBC2Out;
    TClonesArray* fBDOut;

    BmnGemStripStationSet* fDetectorGEM; // GEM-geometry
    BmnSiliconStationSet* fDetectorSI; // SI-geometry   

    map <Int_t, Int_t> fSiMods;
    map <Int_t, Int_t> fGemStats;
    map <TClonesArray*, TClonesArray*> fTriggers;
    
    inline Int_t SiModPermutation(Int_t mod) {
        return fSiMods.find(mod)->second;
    }
    
    inline Int_t GemStatPermutation(Int_t stat) {
        return fGemStats.find(stat)->second;
    }
    
    void ConvertTriggers(map <TClonesArray*, TClonesArray*>);
    
    Bool_t isTrig;
    Bool_t isGem;
    Bool_t isSil;    

    FairRootManager* ioman;

    ClassDef(BmnDigiConverter, 1);
};

#endif
