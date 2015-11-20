/*************************************************************************************
 *
 *         Class BmnZdcContFact
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  06-11-2015   
 *
 ************************************************************************************/

using namespace std;
#include "BmnZdcContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnZdcGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnZdcContFact)

static BmnZdcContFact gBmnZdcContFact;

BmnZdcContFact::BmnZdcContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnZdcContFact";
  fTitle="Factory for parameter containers in libZdc";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnZdcContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the ZDC library.*/

    FairContainer* p= new FairContainer("BmnZdcGeoPar",
                                          "Zdc Geometry Parameters",
                                          "ZdcDefaultContext");
    p->addContext("ZdcNonDefaultContext");

    containers->Add(p);

    //    p->print();
}

FairParSet* BmnZdcContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"BmnZdcGeoPar")==0) {
    p=new BmnZdcGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
