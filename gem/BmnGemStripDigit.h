#ifndef BMNGEMSTRIPDIGIT_H
#define	BMNGEMSTRIPDIGIT_H

#include <BmnStripDigit.h>

using namespace std;

class BmnGemStripDigit : public BmnStripDigit {
public:
    BmnGemStripDigit();
    BmnGemStripDigit(BmnGemStripDigit* digit);
    BmnGemStripDigit(Int_t iStation, Int_t iModule, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal);

    virtual ~BmnGemStripDigit();

private:

    ClassDef(BmnGemStripDigit,2);
};

#endif

