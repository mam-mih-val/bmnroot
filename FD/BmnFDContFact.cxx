
using namespace std;
#include "BmnFDContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnFDGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnFDContFact)

static BmnFDContFact gBmnBD1ContFact;

BmnFDContFact::BmnFDContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnFDContFact";
  fTitle="Factory for parameter containers in libFD";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnFDContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the FD library.*/

    FairContainer* t= new FairContainer("BmnFDGeoPar",
                                          "FD Geometry Parameters",
                                          "FDDefaultContext");
    t->addContext("FDNonDefaultContext");

    containers->Add(t);

    //    p->print();
}

FairParSet* BmnFDContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* t=NULL;
  if (strcmp(name,"BmnFDGeoPar")==0) {
    t=new BmnFDGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return t;
}
