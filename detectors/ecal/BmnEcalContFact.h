// -------------------------------------------------------------------------
// -----                     BmnEcalContFact header file                -----
// -------------------------------------------------------------------------


#ifndef BMNECALCONTFACT_H
#define BMNECALCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnEcalContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnEcalContFact();
  ~BmnEcalContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( BmnEcalContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !BMNECALCONTFACT_H */
