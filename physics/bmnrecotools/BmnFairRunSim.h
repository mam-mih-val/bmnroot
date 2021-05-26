/** 
 * BmnFairRunSim
 * FairRunSim with additional execution control features
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru>
 */

#ifndef BMNFAIRRUNSIM_H
#define BMNFAIRRUNSIM_H

#include <FairRunSim.h>

#define CutExistentVertex  -100.0
#define CutValidZ  -3.0
#define CutPVMinTracks  2

class BmnFairRunSim : public FairRunSim {
public:
    BmnFairRunSim();
    virtual ~BmnFairRunSim();

    void SetSaveEvent(Bool_t set) {
        fApp->SetSaveCurrentEvent(set);
        saveCurEvent = set;
    }

    Bool_t GetSaveEvent() {
        return saveCurEvent;
    }

    FairMCApplication* GetMCApp() {
        return fApp;
    }
private:
    Bool_t saveCurEvent;

    ClassDef(BmnFairRunSim, 1);
};

#endif /* BMNFAIRRUNSIM_H */

