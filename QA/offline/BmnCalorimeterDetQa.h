#include <TNamed.h>
#include <TH1F.h>
#include "BmnQaHistoManager.h"

#include <BmnOfflineQaSteering.h>

#ifndef BMNCALORIMDETQA_H
#define BMNCALORIMDETQA_H 1

using namespace std;

class BmnCalorimeterDetQa : public TNamed {
public:

    /** Default constructor **/
    BmnCalorimeterDetQa() {
    };
    BmnCalorimeterDetQa(TString, UInt_t);

    /** Destructor **/
    virtual ~BmnCalorimeterDetQa() {
    };

    // Setters

    // Getters

    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }


private:
    BmnOfflineQaSteering* fSteering;
    BmnQaHistoManager* fHistoManager;

    void CommonInfo(TString detName) {
        vector <TString> names = {
            Form("%s_1d, Distribution of iX", detName.Data()),
            Form("%s_1d, Distribution of iY", detName.Data()),
            Form("%s_1d, Distribution of X", detName.Data()),
            Form("%s_1d, Distribution of Y", detName.Data()),
            Form("%s_1d, Distribution of channels", detName.Data()),
            Form("%s_1d, Distribution of amplitudes", detName.Data())
        };

        vector <TString> titles = {
            Form("%s, Distribution of iX", detName.Data()),
            Form("%s, Distribution of iY", detName.Data()),
            Form("%s, Distribution of X", detName.Data()),
            Form("%s, Distribution of Y", detName.Data()),
            Form("%s, Distribution of channels", detName.Data()),
            Form("%s, Distribution of amplitudes", detName.Data())
        };

        if (names.size() != titles.size())
            Fatal("BmnCalorimeterDetQa::CommonInfo()", "Sizes do not coincide!!!");

        for (Int_t iHisto = 0; iHisto < names.size(); iHisto++) {
            Int_t nBins = fSteering->GetAxisAttributes(names[iHisto])[0];
            Double_t xLow = fSteering->GetAxisAttributes(names[iHisto])[1];
            Double_t xUp = fSteering->GetAxisAttributes(names[iHisto])[2];
            fHistoManager->Create1 <TH1F> (names[iHisto], titles[iHisto], nBins, xLow, xUp);
        }
    }

    ClassDef(BmnCalorimeterDetQa, 1);

};

#endif