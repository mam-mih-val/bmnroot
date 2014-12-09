/////////////////////////////////////////////////////////////
//
//  BmnSttContFact
//
//  Factory for the parameter containers in libStt
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "BmnSttContFact.h"
#include "FairRuntimeDb.h"
#include "BmnSttGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnSttContFact)

static BmnSttContFact gBmnSttContFact;

BmnSttContFact::BmnSttContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnSttContFact";
  fTitle="Factory for parameter containers in libStt";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnSttContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    FairContainer* p= new FairContainer("BmnSttGeoPar",
                                          "Stt Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnSttContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"BmnSttGeoPar")==0) {
    p=new BmnSttGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

