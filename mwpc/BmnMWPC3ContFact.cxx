/////////////////////////////////////////////////////////////
//
//  BmnMWPC3ContFact
//
//  Factory for the parameter containers in libMWPC3
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "BmnMWPC3ContFact.h"
#include "FairRuntimeDb.h"
#include "BmnMWPC3GeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnMWPC3ContFact)

static BmnMWPC3ContFact gBmnMWPC3ContFact;

BmnMWPC3ContFact::BmnMWPC3ContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnMWPC3ContFact";
  fTitle="Factory for parameter containers in libMWPC3";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnMWPC3ContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    FairContainer* p= new FairContainer("BmnMWPC3GeoPar",
                                          "MWPC3 Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnMWPC3ContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"BmnMWPC3GeoPar")==0) {
    p=new BmnMWPC3GeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

