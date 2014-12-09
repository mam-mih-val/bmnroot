/////////////////////////////////////////////////////////////
//
//  BmnDch1ContFact
//
//  Factory for the parameter containers in libDCH1
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "BmnDch1ContFact.h"
#include "FairRuntimeDb.h"
#include "BmnDch1GeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnDch1ContFact)

static BmnDch1ContFact gBmnDch1ContFact;

BmnDch1ContFact::BmnDch1ContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnDch1ContFact";
  fTitle="Factory for parameter containers in libDCH1";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnDch1ContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    FairContainer* p= new FairContainer("BmnDch1GeoPar",
                                          "DCH1 Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnDch1ContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"BmnDch1GeoPar")==0) {
    p=new BmnDch1GeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

