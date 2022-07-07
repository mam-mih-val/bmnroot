#ifndef BMNEMBEDDING_H
#define BMNEMBEDDING_H


#include <TClonesArray.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TChain.h>
#include <TGeoTrack.h>
#include <TFile.h>
#include <TH2F.h>

#include <FairMCEventHeader.h>
#include <CbmMCTrack.h>
#include <CbmStsPoint.h>

#include <DigiRunHeader.h>
#include <BmnFieldPar.h>
#include <BmnMatch.h>
#include <BmnSiliconPoint.h>
#include <BmnCSCPoint.h>
#include <BmnMath.h>
#include <BmnEventHeader.h>
#include <DstEventHeader.h>
#include <BmnSiliconDigit.h>
#include <BmnGemStripDigit.h>
#include <BmnCSCDigit.h>
#include <BmnSiliconHit.h>
#include <BmnGemStripHit.h>
#include <BmnCSCHit.h>
#include "BmnSiliconStationSet.h"
#include "BmnGemStripStationSet.h"
#include <BmnRecoTools.h>
#include <BmnTrigUnion.h>

/**
 * BmnEmbedding - operational class for digit level embedding procedure
 * 
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 */
class BmnEmbedding {
public:
    BmnEmbedding();
    virtual ~BmnEmbedding();

    BmnStatus Embed(
            TString inSourceName = "eve-lam-box.root", TString inBaseName = "evetest-p.root", TString destName = "merged-digi.root",
            Int_t code = 3122, vector<Int_t> outCodes = {2212, -211},
    Bool_t turnOffBaseDigits = kFALSE);
    
private:
    BmnStatus GetNextValidSourceEvent();
    BmnStatus CloneSelected(TString BaseName, TString TempBaseName);

    TTree * fInTreeSource;
    TTree * fInTreeBase;
    TTree * fDestTree;

    UInt_t iSourceEvent = 0;

    Int_t fCode = 0;
    vector<Int_t> fOutCodes;

    vector<TClonesArray*> digiSourceArs; ///<- source digi arrays
    vector<TClonesArray*> digiBaseArs; ///<- base digi arrays
    vector<TClonesArray*> digiDestArs; ///<- destination digi arrays
    vector<TClonesArray*> digiBaseArsOther; ///<- base digi arrays (not from inner tracker)
    vector<TClonesArray*> digiDestArsOther; ///<- destination digi arrays (not from inner tracker)
    vector<TBranch*> digiBaseBrs; ///<- base digi branches
    vector<TBranch*> digiDestBrs; ///<- destination digi branches
    vector<TClonesArray*> matchSourceArs; ///<- source match arrays
    vector<TClonesArray*> matchBaseArs; ///<- source base arrays
    vector<TClonesArray*> matchDestArs; ///<- destination match arrays
    vector<TBranch*> matchDestBrs; ///<- destination match branches
    TClonesArray * mcTracks = nullptr;
    TClonesArray* stsPoints = nullptr;
    TClonesArray* silPoints = nullptr;
    TClonesArray* cscPoints = nullptr;
    DstEventHeader * copyBaseEH = nullptr;
    DstEventHeader * copyDestEH = nullptr;
    FairMCEventHeader * mcEH = nullptr;
    TBranch *EHBranch = nullptr;
    const TString EHMCName = "MCEventHeader.";
    const TString EHDigiName = "BmnEventHeader.";
    const TString RHDigiName = "DigiRunHeader";
    const TString FieldParName = "BmnFieldPar";

    ClassDef(BmnEmbedding, 1);
};

#endif /* BMNEMBEDDING_H */

