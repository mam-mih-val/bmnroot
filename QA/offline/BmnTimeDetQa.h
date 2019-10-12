#include <TNamed.h>
#include <TH1F.h>
#include "BmnQaHistoManager.h"

#include <BmnOfflineQaSteering.h>

#ifndef BMNTIMEDETQA_H
#define BMNTIMEDETQA_H 1

using namespace std;

class BmnTimeDetQa : public TNamed {
public:

    /** Default constructor **/
    BmnTimeDetQa() {
    };
    BmnTimeDetQa(TString, UInt_t);

    /** Destructor **/
    virtual ~BmnTimeDetQa() {
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
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of times", detName.Data()), 
                Form("%s, Distribution of times", detName.Data()), 
                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of times", detName.Data()))[0], 
                        fSteering->GetAxisAttributes(Form("%s_1d, Distribution of times", detName.Data()))[1], 
                                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of times", detName.Data()))[2]);
        
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of planes", detName.Data()), 
                Form("%s, Distribution of planes", detName.Data()), 
                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of planes", detName.Data()))[0], 
                        fSteering->GetAxisAttributes(Form("%s_1d, Distribution of planes", detName.Data()))[1], 
                                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of planes", detName.Data()))[2]);
    }

    void MwpcDchInfo(TString detName) {
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of wires", detName.Data()), 
                Form("%s, Distribution of wires", detName.Data()), 
                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of wires", detName.Data()))[0], 
                        fSteering->GetAxisAttributes(Form("%s_1d, Distribution of wires", detName.Data()))[1], 
                                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of wires", detName.Data()))[2]);
    }

    void TofInfo(TString detName) {
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of strips", detName.Data()), 
                Form("%s, Distribution of strips", detName.Data()), 
                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of strips", detName.Data()))[0], 
                        fSteering->GetAxisAttributes(Form("%s_1d, Distribution of strips", detName.Data()))[1], 
                                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of strips", detName.Data()))[2]);
        fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of amplitudes", detName.Data()), 
                Form("%s, Distribution of amplitudes", detName.Data()), 
                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of amplitudes", detName.Data()))[0], 
                        fSteering->GetAxisAttributes(Form("%s_1d, Distribution of amplitudes", detName.Data()))[1], 
                                fSteering->GetAxisAttributes(Form("%s_1d, Distribution of amplitudes", detName.Data()))[2]);
    }

    ClassDef(BmnTimeDetQa, 1);

};

#endif