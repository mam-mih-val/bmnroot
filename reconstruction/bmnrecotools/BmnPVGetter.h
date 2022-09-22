/** 
 * BmnPVGetter
 * Task for Primary Vertex parameters extraction from DST file
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 */

#ifndef BMNPVGETTER_H
#define BMNPVGETTER_H

#include <limits.h>

#include <TClonesArray.h>
#include <TFile.h>

#include <FairTask.h>

#include <FairPrimaryGenerator.h>
#include <CbmVertex.h>
#include "BmnRecoTools.h"
#include "BmnFairRunSim.h"


class BmnPVGetter : public FairTask {
public:
    BmnPVGetter(TString fname, Long64_t nEvs = 0);
    virtual ~BmnPVGetter();
    
    InitStatus Init();
    //InitStatus ReInit();
    void Exec(Option_t *option);
    void Finish();
    void FinishEvent();
    
    BmnFairRunSim* GetRunSimInst() {
        return fRunSimInst;
    }
    void SetRunSimInst(BmnFairRunSim* v) {
        fRunSimInst = v;
        printf("fri name %s\n", fRunSimInst->GetName());
    }
    
    FairPrimaryGenerator* GetGenerator() {
        return fGen;
    }
    void SetGenerator(FairPrimaryGenerator* v) {
        fGen = v;
    }
    Long64_t GetNFileEvents() {
        return nFEvents;
    }
protected:
    void NextFileEvent();
    
    BmnFairRunSim * fRunSimInst;///<- bad practise, but this is FairRoot
    FairPrimaryGenerator* fGen;
    TString fDstTreeName;
    TString fDstFileName;
    TString fPVertexName;
    TString fEvHeaderName;
    TFile *fDstFile;
    TTree *fDstTree;
    DstEventHeader* fEvHeader;
    DstEventHeader* fEvHeaderShow;
    TClonesArray* fPVertex;
    TClonesArray* fPVertexShow;
    Long64_t iEv;
    Long64_t nFEvents;
    Double_t fVZ;
    Bool_t doNext;

    ClassDef(BmnPVGetter, 1);
};

#endif /* BMNPVGETTER_H */

