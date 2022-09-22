/*************************************************************************************
 *
 *         Class BmnNdetContFact
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015   
 *  Modified by M.Golubeva July 2022
 *
 ************************************************************************************/

using namespace std;
#include "BmnNdetContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnNdetGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnNdetContFact)

static BmnNdetContFact gBmnNdetContFact;

BmnNdetContFact::BmnNdetContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnNdetContFact";
  fTitle="Factory for parameter containers in libNdet";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnNdetContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the NDET library.*/

    FairContainer* p= new FairContainer("BmnNdetGeoPar",
                                          "Ndet Geometry Parameters",
                                          "NdetDefaultContext");
    p->addContext("NdetNonDefaultContext");

    containers->Add(p);

    //    p->print();
}

FairParSet* BmnNdetContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"BmnNdetGeoPar")==0) {
    p=new BmnNdetGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
