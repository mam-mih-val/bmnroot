//*-- AUTHOR : Denis Bertini
//*-- Created : 20/06/2005

/////////////////////////////////////////////////////////////
//
//  CbmStsContFact
//
//  Factory for the parameter containers in libSts
//
/////////////////////////////////////////////////////////////


#include "CbmStsContFact.h"

#include "CbmStsParRootFileIo.h"
#include "CbmStsParAsciiFileIo.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDigiPar.h"

#include "FairRuntimeDb.h"
#include "CbmParTest.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"

#include "TClass.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(CbmStsContFact)

static CbmStsContFact gCbmStsContFact;

CbmStsContFact::CbmStsContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmStsContFact";
  fTitle="Factory for parameter containers in libSts";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmStsContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
    FairContainer* p1= new FairContainer("CbmStsDigiPar",
                                          "Sts Digitisation Parameters",
                                          "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");

    FairContainer* p2= new FairContainer("CbmGeoStsPar",
                                          "Sts Geometry Parameters",
                                          "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");

    containers->Add(p1);
    containers->Add(p2);
}

FairParSet* CbmStsContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"CbmStsDigiPar")==0) {
    p=new CbmStsDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmGeoStsPar")==0) {
    p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

void  CbmStsContFact::activateParIo(FairParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Sts
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    CbmStsParRootFileIo* p=new CbmStsParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    CbmStsParAsciiFileIo* p=new CbmStsParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
  }
}

