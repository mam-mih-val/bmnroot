/*************************************************************************************
 *
 *         Class BmnHodoContFact
 *         
 *  Author:   Sergei Merts
 *  e-mail:   merts@jinr.ru
 *  Version:  28-03-2021
 *
 ************************************************************************************/

using namespace std;
#include "BmnHodoContFact.h"

#include <iomanip>
#include <iostream>

#include "BmnHodoGeoPar.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

ClassImp(BmnHodoContFact)

    static BmnHodoContFact gBmnHodoContFact;

BmnHodoContFact::BmnHodoContFact() {
    // Constructor (called when the library is loaded)
    fName = "BmnHodoContFact";
    fTitle = "Factory for parameter containers in libHodo";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void BmnHodoContFact::setAllContainers() {
    /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Hodo library.*/

    FairContainer* p = new FairContainer("BmnHodoGeoPar", "Hodo Geometry Parameters", "HodoDefaultContext");
    p->addContext("HodoNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnHodoContFact::createContainer(FairContainer* c) {
    /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
    const char* name = c->GetName();
    FairParSet* p = NULL;
    if (strcmp(name, "BmnHodoGeoPar") == 0) {
        p = new BmnHodoGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}
