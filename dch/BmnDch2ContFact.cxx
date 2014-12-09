/////////////////////////////////////////////////////////////
//
//  BmnDch2ContFact
//
//  Factory for the parameter containers in libDCH2
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "BmnDch2ContFact.h"
#include "FairRuntimeDb.h"
#include "BmnDch2GeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnDch2ContFact)

static BmnDch2ContFact gBmnDch2ContFact;

BmnDch2ContFact::BmnDch2ContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnDch2ContFact";
  fTitle="Factory for parameter containers in libDCH2";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnDch2ContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    FairContainer* p= new FairContainer("BmnDch2GeoPar",
                                          "DCH2 Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnDch2ContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"BmnDch2GeoPar")==0) {
    p=new BmnDch2GeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

