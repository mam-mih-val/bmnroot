#ifndef CBMPSDCONTFACT_H
#define CBMPSDCONTFACT_H

#include "FairContFact.h"

class FairContainer;
class FairParSet;

class CbmPsdContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmPsdContFact();
  ~CbmPsdContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( CbmPsdContFact,0) // Factory for all PSD parameter containers
};

#endif  /* !CBMPSDCONTFACT_H */
