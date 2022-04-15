
/* 
 * File:   BmnSiliconTrack.h
 * Author: Sergey Merts
 *
 * Created on October 27, 2017, 10:47 AM
 */

#ifndef BMNSILICONTRACK_H
#define BMNSILICONTRACK_H

#include "BmnTrack.h"
#include "BmnFitNode.h"

class BmnSiliconTrack : public BmnTrack {
public:

    /** Default constructor **/
    BmnSiliconTrack();

    /** Destructor **/
    virtual ~BmnSiliconTrack();

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
    
private:

    vector<BmnFitNode> fFitNodes; // Array of fit nodes    
    Bool_t fStoreHits;
    Bool_t fUsed; //needed to check seeds splitting    

    ClassDef(BmnSiliconTrack, 1);

};

#endif /* BMNSILICONTRACK_H */

