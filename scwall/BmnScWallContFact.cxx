/*************************************************************************************
 *
 *         Class BmnScWallContFact
 *         
 *  Author:   Sergei Merts
 *  e-mail:   merts@jinr.ru
 *  Version:  28-03-2021
 *
 ************************************************************************************/

using namespace std;
#include "BmnScWallContFact.h"

#include <iomanip>
#include <iostream>

#include "BmnScWallGeoPar.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

ClassImp(BmnScWallContFact)

    static BmnScWallContFact gBmnScWallContFact;

BmnScWallContFact::BmnScWallContFact() {
    // Constructor (called when the library is loaded)
    fName = "BmnScWallContFact";
    fTitle = "Factory for parameter containers in libScWall";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void BmnScWallContFact::setAllContainers() {
    /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the ScWall library.*/

    FairContainer* p = new FairContainer("BmnScWallGeoPar", "ScWall Geometry Parameters", "ScWallDefaultContext");
    p->addContext("ScWallNonDefaultContext");

    containers->Add(p);
}

FairParSet* BmnScWallContFact::createContainer(FairContainer* c) {
    /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
    const char* name = c->GetName();
    FairParSet* p = NULL;
    if (strcmp(name, "BmnScWallGeoPar") == 0) {
        p = new BmnScWallGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}
