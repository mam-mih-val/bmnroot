//--------------------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <iomanip>

#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"

#include "BmnTOF1GeoPar.h"
#include "BmnTOF1ContFact.h"

ClassImp(BmnTOF1ContFact)

static BmnTOF1ContFact gBmnTOF1ContFact;

using namespace std;
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTOF1ContFact::BmnTOF1ContFact() 
{
	// Constructor (called when the library is loaded)
	fName = "BmnTOF1ContFact";
	fTitle = "Factory for parameter containers in libTOF1";

	setAllContainers();
	FairRuntimeDb::instance()->addContFactory(this);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void BmnTOF1ContFact::setAllContainers() 
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the TOF1 library.*/

	FairContainer* ptr = new FairContainer("BmnTOF1GeoPar", "TOF1 Geometry Parameters", "TOF1DefaultContext");
	ptr->addContext("TOF1NonDefaultContext");

	containers->Add(ptr);

    //    ptp->print();
}
//--------------------------------------------------------------------------------------------------------------------------------------
FairParSet* BmnTOF1ContFact::createContainer(FairContainer* c) 
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */

	const char* name = c->GetName();
	FairParSet* ptr = NULL;

	if(strcmp(name, "BmnTOF1GeoPar") == 0) ptr = new BmnTOF1GeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());

return ptr;
}
//--------------------------------------------------------------------------------------------------------------------------------------

