#include <TNamed.h>
#include <TH1F.h>
#include "BmnQaHistoManager.h"

#ifndef BMNCALORIMDETQA_H
#define BMNCALORIMDETQA_H 1

using namespace std;

class BmnCalorimeterDetQa : public TNamed {
public:

    /** Default constructor **/
    BmnCalorimeterDetQa() {
    };
    BmnCalorimeterDetQa(TString);

    /** Destructor **/
    virtual ~BmnCalorimeterDetQa() {
    };

    // Setters

    // Getters

    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }


private:
    BmnQaHistoManager* fHistoManager;

    void CommonInfo(TString detName) {
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of iX", detName.Data()), Form("Distribution of iX"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of iY", detName.Data()), Form("Distribution of iY"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of X", detName.Data()), Form("Distribution of X"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of Y", detName.Data()), Form("Distribution of Y"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of channels", detName.Data()), Form("Distribution of channels"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of amplitudes", detName.Data()), Form("Distribution of amplitudes"), 100, 0., 0.);
    }

    ClassDef(BmnCalorimeterDetQa, 1);

};

#endif