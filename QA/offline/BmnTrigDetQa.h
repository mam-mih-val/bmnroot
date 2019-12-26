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
    BmnTrigDetQa() {
    };
    BmnTrigDetQa(vector <TString>, UInt_t);
    BmnTrigDetQa(map <TClonesArray*, TString>, UInt_t);

    /** Destructor **/
    virtual ~BmnTrigDetQa() {
    };

    // Setters

    // Getters

    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

private:
    BmnOfflineQaSteering* fSteering;
    BmnQaHistoManager* fHistoManager;

    void CommonInfo(TString detName, TString prefix) {
        vector <TString> names = {
            Form("%sTRIGGERS_1d, %s, Distribution of inn. channels", prefix.Data(), detName.Data()),
            Form("%sTRIGGERS_1d, %s, Distribution of times", prefix.Data(), detName.Data()),
            Form("%sTRIGGERS_1d, %s, Distribution of amplitudes", prefix.Data(), detName.Data())
        };

        vector <TString> titles = {
            Form("%s, Distribution of inn. channels", detName.Data()),
            Form("%s, Distribution of times", detName.Data()),
            Form("%s, Distribution of amplitudes", detName.Data())
        };

        if (names.size() != titles.size())
            Fatal("BmnTrigDetQa::CommonInfo()", "Sizes do not coincide!!!");
        
        for (Int_t iHisto = 0; iHisto < names.size(); iHisto++) {
            Int_t nBins = fSteering->GetAxisAttributes(names[iHisto])[0];
            Double_t xLow = fSteering->GetAxisAttributes(names[iHisto])[1];
            Double_t xUp = fSteering->GetAxisAttributes(names[iHisto])[2];
            fHistoManager->Create1 <TH1F> (names[iHisto], titles[iHisto], nBins, xLow, xUp);
        }
    }

    ClassDef(BmnTrigDetQa, 1);

};

#endif