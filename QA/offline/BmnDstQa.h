#include <TNamed.h>
#include <TH1F.h>
#include "BmnQaHistoManager.h"

#ifndef BMNDSTQA_H
#define BMNDSTQA_H 1

using namespace std;

class BmnDstQa : public TNamed {
public:

    /** Default constructor **/
    BmnDstQa();
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
    BmnQaHistoManager* fHistoManager;
    
    void GlobalTracksDistributions();
    void InnerTracksDistributions();

    ClassDef(BmnDstQa, 1);

};

#endif