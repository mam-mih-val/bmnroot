#ifndef BMNSILICONDIGIT_H
#define BMNSILICONDIGIT_H

#include <BmnStripDigit.h>

using namespace std;

class BmnSiliconDigit : public BmnStripDigit {
public:
    BmnSiliconDigit();
    BmnSiliconDigit(BmnSiliconDigit* digit);
    BmnSiliconDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal);

    virtual ~BmnSiliconDigit();

private:
//    Int_t fStripLayer; // 0 - lower layer, 1 - upper layer
    ClassDef(BmnSiliconDigit, 2);
};

#endif /* BMNSILICONDIGIT_H */

