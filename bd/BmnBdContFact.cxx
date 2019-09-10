
using namespace std;
#include "BmnBdContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnBdGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnBdContFact)

static BmnBdContFact gBmnBdContFact;

BmnBdContFact::BmnBdContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnBdContFact";
  fTitle="Factory for parameter containers in libBd";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnBdContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BD1 library.*/

    FairContainer* t= new FairContainer("BmnBdGeoPar",
                                          "BD Geometry Parameters",
                                          "BdDefaultContext");
    t->addContext("BdNonDefaultContext");

    containers->Add(t);

    //    p->print();
}

FairParSet* BmnBdContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* t=NULL;
  if (strcmp(name,"BmnBdGeoPar")==0) {
    t=new BmnBdGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return t;
}
