#ifndef BMNArmTrigHit_H
#define BMNArmTrigHit_H
#include "BmnHit.h"

using namespace std;

class BmnArmTrigHit : public BmnHit {
public:
    BmnArmTrigHit();
    BmnArmTrigHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index);

    virtual ~BmnArmTrigHit();

private:

    ClassDef(BmnArmTrigHit, 1);
};

#endif /* BMNArmTrigHit_H */

