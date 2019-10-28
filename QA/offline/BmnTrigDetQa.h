#include <TNamed.h>
#include <TH1F.h>
#include <TClonesArray.h>
#include "BmnQaHistoManager.h"

#include <BmnOfflineQaSteering.h>

#ifndef BMNTRIGDETQA_H
#define BMNTRIGDETQA_H 1

using namespace std;

class BmnTrigDetQa : public TNamed {
public:

    /** Default constructor **/
    BmnTrigDetQa() {};
    BmnTrigDetQa(vector <TString>, UInt_t);
    BmnTrigDetQa(map <TClonesArray*, TString>, UInt_t);

    /** Destructor **/
    virtual ~BmnTrigDetQa() {};

    // Setters
   
    // Getters
    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

private:
    BmnOfflineQaSteering* fSteering;
    BmnQaHistoManager* fHistoManager;
    
    void CommonInfo(TString detName, TString prefix) {
        fHistoManager->Create1 <TH1F> (Form("%sTRIGGERS_1d, %s, Distribution of inn. channels", prefix.Data(), detName.Data()), Form("Distribution of inn. channels"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%sTRIGGERS_1d, %s, Distribution of times", prefix.Data(), detName.Data()), Form("Distribution of times"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%sTRIGGERS_1d, %s, Distribution of amplitudes", prefix.Data(), detName.Data()), Form("Distribution of amplitudes"), 100, 0., 0.);      
    }
   
    ClassDef(BmnTrigDetQa, 1);

};

#endif