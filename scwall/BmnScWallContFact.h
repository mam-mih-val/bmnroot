// -------------------------------------------------------------------------
// -----                     BmnScWallContFact header file                -----
// -------------------------------------------------------------------------

#ifndef BMNSCWALLCONTFACT_H
#define BMNSCWALLCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnScWallContFact : public FairContFact {
   private:
    void setAllContainers();

   public:
    BmnScWallContFact();
    ~BmnScWallContFact() {}
    FairParSet* createContainer(FairContainer*);

    ClassDef(BmnScWallContFact, 0)
};

#endif
