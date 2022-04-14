// -------------------------------------------------------------------------
// -----                     BmnFHCalContFact header file                -----
// -------------------------------------------------------------------------

#ifndef BMNFHCALCONTFACT_H
#define BMNFHCALCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnFHCalContFact : public FairContFact {
private:
    void setAllContainers();

public:
    BmnFHCalContFact();
    ~BmnFHCalContFact() {}
    FairParSet* createContainer(FairContainer*);

    ClassDef(BmnFHCalContFact, 0)
};

#endif
