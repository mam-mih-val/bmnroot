#include <TNamed.h>
#include <TH1F.h>
#include "BmnQaHistoManager.h"

#ifndef BMNTIMEDETQA_H
#define BMNTIMEDETQA_H 1

using namespace std;

class BmnTimeDetQa : public TNamed {
public:

    /** Default constructor **/
    BmnTimeDetQa() {
    };
    BmnTimeDetQa(TString);

    /** Destructor **/
    virtual ~BmnTimeDetQa() {
    };

    // Setters

    // Getters

    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

private:
    BmnQaHistoManager* fHistoManager;

    void CommonInfo(TString detName) {
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of times", detName.Data()), Form("Distribution of times"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of planes", detName.Data()), Form("Distribution of planes"), 100, 0., 0.);
    }

    void MwpcDchInfo(TString detName) {
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of wires", detName.Data()), Form("Distribution of wires"), 100, 0., 0.);
    }

    void TofInfo(TString detName) {
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of strips", detName.Data()), Form("Distribution of strips"), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%s, Distribution of amplitudes", detName.Data()), Form("Distribution of amplitudes"), 100, 0., 0.);
    }

    ClassDef(BmnTimeDetQa, 1);

};

#endif