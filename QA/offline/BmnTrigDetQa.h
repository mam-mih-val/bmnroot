#include <TNamed.h>
#include <TH1F.h>
#include <TClonesArray.h>
#include "BmnQaHistoManager.h"

#ifndef BMNTRIGDETQA_H
#define BMNTRIGDETQA_H 1

using namespace std;

class BmnTrigDetQa : public TNamed {
public:

    /** Default constructor **/
    BmnTrigDetQa() {};
    BmnTrigDetQa(vector <TString>);
    BmnTrigDetQa(map <TClonesArray*, TString>);

    /** Destructor **/
    virtual ~BmnTrigDetQa() {};

    // Setters
   
    // Getters
    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

private:
    BmnQaHistoManager* fHistoManager;
    
    void CommonInfo(TString detName) {
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of inn. channels", detName.Data()), Form("Distribution of inn. channels"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of times", detName.Data()), Form("Distribution of times"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of amplitudes", detName.Data()), Form("Distribution of amplitudes"), 100, 0., 0.);      
    }
   
    ClassDef(BmnTrigDetQa, 1);

};

#endif