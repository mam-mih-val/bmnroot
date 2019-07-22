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
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of iX", detName.Data()), Form("%s, Distribution of iX", detName.Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of iY", detName.Data()), Form("%s, Distribution of iY", detName.Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of X", detName.Data()), Form("%s, Distribution of X", detName.Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of Y", detName.Data()), Form("%s, Distribution of Y", detName.Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of channels", detName.Data()), Form("%s, Distribution of channels", detName.Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of amplitudes", detName.Data()), Form("%s, Distribution of amplitudes", detName.Data()), 100, 0., 0.);
    }

    ClassDef(BmnCalorimeterDetQa, 1);

};

#endif