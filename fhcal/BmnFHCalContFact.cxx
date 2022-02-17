/*************************************************************************************
 *
 *         Class BmnFHCalContFact
 *
 ************************************************************************************/

using namespace std;
#include "BmnFHCalContFact.h"

#include <iomanip>
#include <iostream>

#include "BmnFHCalGeoPar.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

ClassImp(BmnFHCalContFact)

static BmnFHCalContFact gBmnFHCalContFact;

BmnFHCalContFact::BmnFHCalContFact() {
    // Constructor (called when the library is loaded)
    fName = "BmnFHCalContFact";
    fTitle = "Factory for parameter containers in libFHCal";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void BmnFHCalContFact::setAllContainers() {
    /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the FHCal library.*/

    FairContainer* p = new FairContainer("BmnFHCalGeoPar", "FHCal Geometry Parameters", "FHCalDefaultContext");
    p->addContext("FHCalNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnFHCalContFact::createContainer(FairContainer* c) {
    /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
    const char* name = c->GetName();
    FairParSet* p = NULL;
    if (strcmp(name, "BmnFHCalGeoPar") == 0) {
        p = new BmnFHCalGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}
