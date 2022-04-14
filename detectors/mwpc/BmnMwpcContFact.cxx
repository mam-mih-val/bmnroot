using namespace std;
#include "BmnMwpcContFact.h"
#include "FairRuntimeDb.h"
#include "BmnMwpcGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnMwpcContFact)

static BmnMwpcContFact gBmnMwpcContFact;

BmnMwpcContFact::BmnMwpcContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnMwpcContFact";
  fTitle="Factory for parameter containers in libMwpc";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnMwpcContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    FairContainer* p= new FairContainer("BmnMwpcGeoPar",
                                          "Mwpc Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnMwpcContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"BmnMwpcGeoPar")==0) {
    p=new BmnMwpcGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
