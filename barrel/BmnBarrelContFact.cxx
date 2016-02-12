/*************************************************************************************
 *
 *         Class BmnBarrelContFact
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

using namespace std;
#include "BmnBarrelContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnBarrelGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnBarrelContFact)

static BmnBarrelContFact gBmnBarrelContFact;

BmnBarrelContFact::BmnBarrelContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnBarrelContFact";
  fTitle="Factory for parameter containers in libBarrel";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnBarrelContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BARREL library.*/

    FairContainer* p= new FairContainer("BmnBarrelGeoPar",
                                          "Barrel Geometry Parameters",
                                          "BarrelDefaultContext");
    p->addContext("BarrelNonDefaultContext");

    containers->Add(p);

    //    p->print();
}

FairParSet* BmnBarrelContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"BmnBarrelGeoPar")==0) {
    p=new BmnBarrelGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
