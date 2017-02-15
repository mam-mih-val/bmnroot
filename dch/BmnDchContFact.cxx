/////////////////////////////////////////////////////////////
//
//  BmnDchContFact
//
//  Factory for the parameter containers in libDCH
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "BmnDchContFact.h"
#include "FairRuntimeDb.h"
#include "BmnDchGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnDchContFact)

static BmnDchContFact gBmnDchContFact;

BmnDchContFact::BmnDchContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnDchContFact";
  fTitle="Factory for parameter containers in libDCH";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnDchContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    FairContainer* p= new FairContainer("BmnDchGeoPar",
                                          "DCH Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnDchContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"BmnDchGeoPar")==0) {
    p=new BmnDchGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

