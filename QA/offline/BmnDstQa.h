#include <TNamed.h>
#include <TH1F.h>
#include <TH2F.h>
#include "BmnQaHistoManager.h"

#include <BmnOfflineQaSteering.h>

#ifndef BMNDSTQA_H
#define BMNDSTQA_H 1

using namespace std;

class BmnDstQa : public TNamed {
public:

    /** Default constructor **/
    BmnDstQa() {}
    BmnDstQa(UInt_t);
    //BmnDstQa(TString);

    /** Destructor **/
    virtual ~BmnDstQa() {
    };

    // Setters

    // Getters

    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

private:
    BmnOfflineQaSteering* fSteering;
    BmnQaHistoManager* fHistoManager;
    
    void GlobalTracksDistributions(TString);
    void InnerTracksDistributions(TString);

    ClassDef(BmnDstQa, 1);

};

#endif