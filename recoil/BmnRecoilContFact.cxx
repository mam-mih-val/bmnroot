/*************************************************************************************
 *
 *         Class BmnRecoilContFact
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  24-03-2014   
 *
 ************************************************************************************/

using namespace std;
#include "BmnRecoilContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnRecoilGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnRecoilContFact)

static BmnRecoilContFact gBmnRecoilContFact;

BmnRecoilContFact::BmnRecoilContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnRecoilContFact";
  fTitle="Factory for parameter containers in libRecoil";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnRecoilContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the RECOIL library.*/

    FairContainer* p= new FairContainer("BmnRecoilGeoPar",
                                          "Recoil Geometry Parameters",
                                          "RecoilDefaultContext");
    p->addContext("RecoilNonDefaultContext");

    containers->Add(p);

    //    p->print();
}

FairParSet* BmnRecoilContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"BmnRecoilGeoPar")==0) {
    p=new BmnRecoilGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
