/////////////////////////////////////////////////////////////
//
//  BmnMWPC2ContFact
//
//  Factory for the parameter containers in libMWPC2
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "BmnMWPC2ContFact.h"
#include "FairRuntimeDb.h"
#include "BmnMWPC2GeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnMWPC2ContFact)

static BmnMWPC2ContFact gBmnMWPC2ContFact;

BmnMWPC2ContFact::BmnMWPC2ContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnMWPC2ContFact";
  fTitle="Factory for parameter containers in libMWPC2";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnMWPC2ContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    FairContainer* p= new FairContainer("BmnMWPC2GeoPar",
                                          "MWPC2 Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnMWPC2ContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"BmnMWPC2GeoPar")==0) {
    p=new BmnMWPC2GeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

