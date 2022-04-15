/*************************************************************************************
 *
 *         Class BmnBCContFact
 *         
 *  Adopted for BMN by:  Nikita Lashmanov,  Andrey Dryuk
 *  e-mail:  nikita10630@mail.ru,  andredryuk@gmail.com
 *  Version:  20.12.2020  
 *
 ************************************************************************************/

using namespace std;
#include "BmnBCContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "BmnBCGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(BmnBCContFact)

static BmnBCContFact gBmnBCContFact;

BmnBCContFact::BmnBCContFact() {
  // Constructor (called when the library is loaded)
  fName="BmnBCContFact";
  fTitle="Factory for parameter containers in libBC";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BmnBCContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BD1 library.*/

    FairContainer* t= new FairContainer("BmnBCGeoPar",
                                          "BC Geometry Parameters",
                                          "BCDefaultContext");
    t->addContext("BCNonDefaultContext");

    containers->Add(t);

    //    p->print();
}

FairParSet* BmnBCContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* t=NULL;
  if (strcmp(name,"BmnBCGeoPar")==0) {
    t=new BmnBCGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return t;
}
