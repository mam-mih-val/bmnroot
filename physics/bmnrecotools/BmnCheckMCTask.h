/** 
 * BmnCheckMCTask
 * Task for reconstructability check
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 */

#ifndef BMNCHECKMCTASK_H
#define BMNCHECKMCTASK_H

#include <TClonesArray.h>

#include <FairTask.h>
#include <FairMCApplication.h>

#include "BmnRecoTools.h"
#include "BmnFairRunSim.h"
#include <CbmVertex.h>

using namespace std;

class BmnCheckMCTask : public FairTask {
public:
    BmnCheckMCTask(Long64_t nevents, Int_t minHits);
    virtual ~BmnCheckMCTask();

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
    }
protected:
    Int_t fMinHits;
    Long64_t nMaxValidEvents;
    Long64_t nValidEvents;
    BmnFairRunSim * fRunSimInst;
    TString fMCTracksBranchName;
    TString fGemPointsBranchName;
    TString fSilPointsBranchName;
    TString fCSCPointsBranchName;
    TString fPVertexName;
    TClonesArray* fMCTracks;
    TClonesArray* fGemPoints;
    TClonesArray* fSilPoints;
    TClonesArray* fCSCPoints;
    TClonesArray* fPVertexShow;
    Double_t fVZ;
    
    ClassDef(BmnCheckMCTask, 1);
};

#endif /* BMNCHECKMCTASK_H */

