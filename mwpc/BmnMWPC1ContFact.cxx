/////////////////////////////////////////////////////////////
//
//  BmnMWPC1ContFact
//
//  Factory for the parameter containers in libMWPC1
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "BmnMWPC1ContFact.h"
#include "FairRuntimeDb.h"
#include "BmnMWPC1GeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnMWPC1ContFact)

static BmnMWPC1ContFact gBmnMWPC1ContFact;

BmnMWPC1ContFact::BmnMWPC1ContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnMWPC1ContFact";
  fTitle="Factory for parameter containers in libMWPC1";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnMWPC1ContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    FairContainer* p= new FairContainer("BmnMWPC1GeoPar",
                                          "MWPC1 Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnMWPC1ContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"BmnMWPC1GeoPar")==0) {
    p=new BmnMWPC1GeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

