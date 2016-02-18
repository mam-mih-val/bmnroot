/*************************************************************************************
 *
 *         Class BmnBdContFact
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

using namespace std;
#include "BmnBdContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnBdGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnBdContFact)

static BmnBdContFact gBmnBdContFact;

BmnBdContFact::BmnBdContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnBdContFact";
  fTitle="Factory for parameter containers in libBd";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnBdContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BD library.*/

    FairContainer* p= new FairContainer("BmnBdGeoPar",
                                          "Bd Geometry Parameters",
                                          "BdDefaultContext");
    p->addContext("BdNonDefaultContext");

    containers->Add(p);

    //    p->print();
}

FairParSet* BmnBdContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"BmnBdGeoPar")==0) {
    p=new BmnBdGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
