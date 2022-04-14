// -------------------------------------------------------------------------
// -----                     BmnHodoContFact header file                -----
// -------------------------------------------------------------------------

#ifndef BMNHODOCONTFACT_H
#define BMNHODOCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnHodoContFact : public FairContFact {
   private:
    void setAllContainers();

   public:
    BmnHodoContFact();
    ~BmnHodoContFact() {}
    FairParSet* createContainer(FairContainer*);

    ClassDef(BmnHodoContFact, 0)
};

#endif
