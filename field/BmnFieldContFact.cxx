// -------------------------------------------------------------------------
// -----                    BmnFieldContFact source file               -----
// -----                   Created 20/02/06  by V. Friese              -----
// -------------------------------------------------------------------------
#include "BmnFieldContFact.h"

#include "BmnFieldPar.h"

#include "FairParSet.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include <iostream>

using std::cout;
using std::endl;


static BmnFieldContFact gBmnFieldContFact;


// -----   Constructor   ---------------------------------------------------
BmnFieldContFact::BmnFieldContFact() {
  fName = "BmnFieldContFact";
  fTitle = "Factory for field parameter containers";
  SetAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
BmnFieldContFact::~BmnFieldContFact() { }
// -------------------------------------------------------------------------



// -----   Create containers   ---------------------------------------------
FairParSet* BmnFieldContFact::createContainer(FairContainer* container) {


  const char* name = container->GetName();
  LOG(info) << "create BmnFieldPar container " << name;
  FairParSet* set = NULL;
  if ( strcmp(name, "BmnFieldPar") == 0 ) 
    set = new BmnFieldPar( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  return set;
}
// -------------------------------------------------------------------------



// -----   Set all containers (private)   ----------------------------------
void BmnFieldContFact::SetAllContainers() {
  FairContainer* container = new FairContainer("BmnFieldPar",
					     "Field parameter container",
					     "Default field");
  containers->Add(container);
}
// -------------------------------------------------------------------------


ClassImp(BmnFieldContFact)
