// -------------------------------------------------------------------------
// -----                     BmnRecoilContFact header file                -----
// -------------------------------------------------------------------------


#ifndef BMNRECOILCONTFACT_H
#define BMNRECOILCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnRecoilContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnRecoilContFact();
  ~BmnRecoilContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( BmnRecoilContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !BMNRECOILCONTFACT_H */
