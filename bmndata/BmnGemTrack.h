

#ifndef BMNGEMTRACK_H
#define BMNGEMTRACK_H

#include "BmnTrack.h"
#include "BmnFitNode.h"

class BmnGemTrack : public BmnTrack {
public:

    /** Default constructor **/
    BmnGemTrack();

    /** Destructor **/
    virtual ~BmnGemTrack();

    /** Accessors  **/
    Bool_t IsUsed() const {
        return fUsed;
    }

    BmnFitNode* GetFitNode(Int_t index) {
        return &fFitNodes[index];
    }

    vector<BmnFitNode>& GetFitNodes() {
        return fFitNodes;
    }

    /** Modifiers  **/
    void SetFitNodes(const vector<BmnFitNode>& nodes) {
        fFitNodes = nodes;
    }

    void SetUsing(Bool_t use) {
        fUsed = use;
    }
    
    void Print();
    
    //AM 7.08.2017
    vector<Double_t> parabolaParameters;
    vector<Double_t> lineParameters;
    double covP[3][3]; // parabola covariance matrix
    double covL[2][2]; // line covariance matrix

private:

    vector<BmnFitNode> fFitNodes; // Array of fit nodes    
    Bool_t fStoreHits;
    Bool_t fUsed; //needed to check seeds splitting    

    ClassDef(BmnGemTrack, 1);

};

#endif
