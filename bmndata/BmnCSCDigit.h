#ifndef BMNCSCDIGIT_H
#define	BMNCSCDIGIT_H

#include <BmnStripDigit.h>

using namespace std;

class BmnCSCDigit : public BmnStripDigit {
public:
    BmnCSCDigit();
    BmnCSCDigit(BmnCSCDigit* digit);
    BmnCSCDigit(Int_t iStation, Int_t iModule, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal);

    virtual ~BmnCSCDigit();
    
private:

    ClassDef(BmnCSCDigit,2);
};

#endif

