// -------------------------------------------------------------------------
// -----                     BmnZdcContFact header file                -----
// -------------------------------------------------------------------------


#ifndef BMNZDCCONTFACT_H
#define BMNZDCCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnZdcContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnZdcContFact();
  ~BmnZdcContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( BmnZdcContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !BMNZDCCONTFACT_H */
