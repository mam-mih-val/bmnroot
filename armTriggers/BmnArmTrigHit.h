#ifndef BMNArmTrigHit_H
#define BMNArmTrigHit_H
#include "BmnHit.h"

using namespace std;

class BmnArmTrigHit : public BmnHit {
public:
    BmnArmTrigHit();
    BmnArmTrigHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Int_t pdg);
    virtual ~BmnArmTrigHit();

    Int_t getPdgId(){return fPdgId;}
    void setPdgId(Int_t pdg){fPdgId = pdg;}

private:

    ClassDef(BmnArmTrigHit, 1);
    Int_t fPdgId;
};

#endif /* BMNArmTrigHit_H */

