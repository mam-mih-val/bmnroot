//*-- AUTHOR : Ilse Koenig
//*-- Created : 25/10/2004

/////////////////////////////////////////////////////////////
//
//  CbmPsdContFact
//
//  Factory for the parameter containers in libPsd
//
/////////////////////////////////////////////////////////////

#include "CbmPsdContFact.h"

#include "CbmGeoPsdPar.h"

#include "FairRuntimeDb.h"

ClassImp(CbmPsdContFact)

static CbmPsdContFact gCbmPsdContFact;

CbmPsdContFact::CbmPsdContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmPsdContFact";
  fTitle="Factory for parameter containers in libPsd";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmPsdContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    FairContainer* p= new FairContainer("CbmGeoPsdPar",
                                          "Psd Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* CbmPsdContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
//  cout << " -I conatiner name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"CbmGeoPsdPar")==0) {
      p=new CbmGeoPsdPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

