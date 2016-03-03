/*************************************************************************************
 *
 *         Class BmnEcalContFact
 *         
 *  Adopted for BMN by:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  10-02-2016   
 *
 ************************************************************************************/

using namespace std;
#include "BmnEcalContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnEcalGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnEcalContFact)

static BmnEcalContFact gBmnEcalContFact;

BmnEcalContFact::BmnEcalContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnEcalContFact";
  fTitle="Factory for parameter containers in libEcal";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnEcalContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the ECAL library.*/

    FairContainer* p= new FairContainer("BmnEcalGeoPar",
                                          "Ecal Geometry Parameters",
                                          "EcalDefaultContext");
    p->addContext("EcalNonDefaultContext");

    containers->Add(p);

    //    p->print();
}

FairParSet* BmnEcalContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"BmnEcalGeoPar")==0) {
    p=new BmnEcalGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
