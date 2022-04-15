/*************************************************************************************
 *
 *         Class BmnFdContFact
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

using namespace std;
#include "BmnSiMDContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnSiMDGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnSiMDContFact)

static BmnSiMDContFact gBmnSiMDContFact;

BmnSiMDContFact::BmnSiMDContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnSiMDContFact";
  fTitle="Factory for parameter containers in libSiMD";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnSiMDContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the SiMD library.*/

    FairContainer* p= new FairContainer("BmnSiMDGeoPar",
                                          "SiMD Geometry Parameters",
                                          "SiMDDefaultContext");
    p->addContext("SiMDNonDefaultContext");

    containers->Add(p);

    //    p->print();
}

FairParSet* BmnSiMDContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"BmnSiMDGeoPar")==0) {
    p=new BmnSiMDGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
