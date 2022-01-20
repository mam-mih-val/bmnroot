#ifndef BMNSiBTDIGIT_H
#define	BMNSiBTDIGIT_H

#include <BmnStripDigit.h>

using namespace std;

class BmnSiBTDigit : public BmnStripDigit {
public:
    BmnSiBTDigit();
    BmnSiBTDigit(BmnSiBTDigit* digit);
    BmnSiBTDigit(Int_t iStation, Int_t iModule, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal);

    virtual ~BmnSiBTDigit();

private:

    ClassDef(BmnSiBTDigit,2);
};

#endif

