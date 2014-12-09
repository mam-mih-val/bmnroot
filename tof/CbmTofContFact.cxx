//*-- AUTHOR : Ilse Koenig
//*-- Created : 25/10/2004

/////////////////////////////////////////////////////////////
//
//  CbmTofContFact
//
//  Factory for the parameter containers in libTof
//
/////////////////////////////////////////////////////////////

#include "CbmTofContFact.h"

#include "CbmGeoTofPar.h"
#include "CbmTofDigiPar.h"

#include "FairRuntimeDb.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(CbmTofContFact)

static CbmTofContFact gCbmTofContFact;

CbmTofContFact::CbmTofContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmTofContFact";
  fTitle="Factory for parameter containers in libTof";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmTofContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Tof library.*/

    FairContainer* p= new FairContainer("CbmGeoTofPar",
                                          "Tof Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);

    FairContainer* p1= new FairContainer("CbmTofDigiPar",
                                          "TOF Digitization parameters",
                                          "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");

    containers->Add(p1);
}

FairParSet* CbmTofContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=NULL;
  if (strcmp(name,"CbmGeoTofPar")==0) {
    p=new CbmGeoTofPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmTofDigiPar")==0) {
    p=new CbmTofDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

