#ifndef CBMSTSCONTFACT_H
#define CBMSTSCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class CbmStsContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmStsContFact();
  ~CbmStsContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( CbmStsContFact,0) // Factory for all STS parameter containers
};

#endif  /* !CBMSTSCONTFACT_H */
