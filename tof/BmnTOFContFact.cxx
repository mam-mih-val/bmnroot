//--------------------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <iomanip>

#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"

#include "BmnTOFGeoPar.h"
#include "BmnTOFContFact.h"

ClassImp(BmnTOFContFact)

static BmnTOFContFact gBmnTOFContFact;

using namespace std;
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOFContFact::BmnTOFContFact() 
{
	// Constructor (called when the library is loaded)
	fName = "BmnTOFContFact";
	fTitle = "Factory for parameter containers in libTOF";

	setAllContainers();
	FairRuntimeDb::instance()->addContFactory(this);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOFContFact::setAllContainers() 
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the TOF library.*/

	FairContainer* ptr = new FairContainer("BmnTOFGeoPar", "TOF Geometry Parameters", "TOFDefaultContext");
	ptr->addContext("TOFNonDefaultContext");

	containers->Add(ptr);

    //    ptp->print();
}
//--------------------------------------------------------------------------------------------------------------------------------------
FairParSet* BmnTOFContFact::createContainer(FairContainer* c) 
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */

	const char* name = c->GetName();
	FairParSet* ptr = NULL;

	if(strcmp(name, "BmnTOFGeoPar") == 0) ptr = new BmnTOFGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());

return ptr;
}
//--------------------------------------------------------------------------------------------------------------------------------------

