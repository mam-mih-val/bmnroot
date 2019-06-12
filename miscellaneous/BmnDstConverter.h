#ifndef BMNDSTCONVERTER_H
#define BMNDSTCONVERTER_H 1

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairEventHeader.h"

#include <TClonesArray.h>
#include <TString.h>
#include <TMath.h>
#include <TROOT.h>
#include <TSystem.h>

#include "CbmStsHit.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "BmnVertex.h"

#include "BmnGemStripHit.h"
#include "BmnSiliconHit.h"
#include "BmnGemTrack.h"
#include "BmnSiliconTrack.h"
#include "BmnGlobalTrack.h"

#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"

#include <map>
#include <iterator>

using namespace std;
using namespace TMath;

class BmnDstConverter : public FairTask {
public:

    BmnDstConverter();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    virtual ~BmnDstConverter();


private:
    void Run7(Int_t*, Int_t*, Int_t*, Int_t*);

    inline Int_t GemStatPermutation(Int_t stat) {
        return fGemStats.find(stat)->second;
    }

    inline Int_t SiliconStatPermutation(Int_t stat) {
        return fSilStats.find(stat)->second;
    }

    inline Int_t GemModPermutation(Int_t stat, Int_t mod) {
        return fGemMods.find(pair <Int_t, Int_t> (stat, mod))->second;
    }

    map <Int_t, Int_t> fGemStats;
    map <pair <Int_t, Int_t>, Int_t> fGemMods;
    map <Int_t, Int_t> fSilStats;

    BmnGemStripStationSet* fDetectorGEM; // GEM-geometry
    BmnSiliconStationSet* fDetectorSI; // SI-geometry

    TString fBranchInnHitsIn;
    TClonesArray* fInnHitsIn;

    TString fBranchVertexIn;
    CbmVertex* fVertexIn;

    TString fBranchTracksIn;
    TClonesArray* fTracksIn;

    TString fBranchGemHitsOut;
    TClonesArray* fGemHitsOut;

    TString fBranchSilHitsOut;
    TClonesArray* fSilHitsOut;

    TString fBranchVertexOut;
    TClonesArray* fVertexOut;

    TString fBranchGemTracksOut;
    TClonesArray* fGemTracksOut;

    TString fBranchSilTracksOut;
    TClonesArray* fSilTracksOut;

    TString fBranchGlobTracksOut;
    TClonesArray* fGlobTracksOut;

    TString fBranchEventHeaderOut;
    
    UInt_t fEventNo;
    FairRootManager* ioman;

    ClassDef(BmnDstConverter, 1);
};

#endif