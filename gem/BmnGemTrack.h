

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

    const BmnFitNode* GetFitNode(Int_t index) const {
        return &fFitNodes[index];
    }

    const vector<BmnFitNode>& GetFitNodes() const {
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
    
private:

    vector<BmnFitNode> fFitNodes; // Array of fit nodes    
    Bool_t fStoreHits;
    Bool_t fUsed; //needed to check seeds splitting    

    ClassDef(BmnGemTrack, 1);

};

#endif
