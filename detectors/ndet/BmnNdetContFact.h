// -------------------------------------------------------------------------
// -----                     BmnNdetContFact header file                -----
// -------------------------------------------------------------------------


#ifndef BMNNDETCONTFACT_H
#define BMNNDETCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnNdetContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnNdetContFact();
  ~BmnNdetContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( BmnNdetContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !BMNNDETCONTFACT_H */
