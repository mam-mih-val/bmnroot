#ifndef CBM2BMNHITCONVERTER_H
#define CBM2BMNHITCONVERTER_H 1

#include <vector>
#include <map>

#include <TNamed.h>
#include "Rtypes.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "FairTask.h"
#include "FairRootManager.h"

#include "BmnEnums.h"
#include "CbmStsHit.h"
#include "BmnGemStripHit.h"
#include "BmnSiliconHit.h"

using namespace std;

class Cbm2BmnHitConverter : public FairTask {
public:
    Cbm2BmnHitConverter();
    virtual ~Cbm2BmnHitConverter();
    InitStatus Init();
    void Exec(Option_t* opt);
    void Finish();

    void SetDetectors(Bool_t gem, Bool_t sil) {
        isGem = gem;
        isSil = sil;
    }
private:
    UInt_t fEventNo;

    Bool_t isGem;
    Bool_t isSil;

    map <Int_t, Int_t> fSiMods;
    map <Int_t, Int_t> fGemStats;
    map <TClonesArray*, TClonesArray*> fTriggers;

    TString fInputStsHitsBranchName;
    TString fOutputGemHitsBranchName;
    TString fOutputSilHitsBranchName;

    FairRootManager* ioman;

    /** Input CBM array of Hits **/
    TClonesArray* fCbmStsHitsArray;
    /** Output array of Gem Hits **/
    TClonesArray* fBmnGemStripHitsArray;
    /** Output array of Silicon Hits **/
    TClonesArray* fBmnSiliconHitsArray;

    inline Int_t GemStatPermutation(Int_t stat) {
        Int_t id = -1;
        map <Int_t, Int_t>::iterator it = fGemStats.find(stat);
        if (it != fGemStats.end())
            id = it->second;
        else
            fprintf(stderr, "Error! Station %d not found!\nPossible wrong mapping\n", stat);
        return id;
    }
    
    ClassDef(Cbm2BmnHitConverter, 1);
};

#endif /* CBM2BMNHITCONVERTER_H */

