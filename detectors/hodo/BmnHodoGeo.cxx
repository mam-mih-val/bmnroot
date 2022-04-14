/*************************************************************************************
 *
 *         Class BmnHodoGeo
 *
 ************************************************************************************/

#include "BmnHodoGeo.h"

#include <iostream>

#include "FairGeoNode.h"

using std::cout;
using std::endl;

ClassImp(BmnHodoGeo)

    // -----   Default constructor   -------------------------------------------
    BmnHodoGeo::BmnHodoGeo() {
    // Constructor
    fName = "Hodo";
    maxSectors = 10;
    maxModules = 70;
}
// -------------------------------------------------------------------------

const char* BmnHodoGeo::getModule1Name(Int_t k) {
    // Returns the module name of muo number m

    sprintf(mod1Name, "Hodo0%i", k + 1);
    return mod1Name;
    cout << "MOD1NAME: " << mod1Name << endl;
}

const char* BmnHodoGeo::getEle1Name(Int_t k) {
    // Returns the element name of muo number m

    sprintf(ele1Name, "s%i", k + 1);
    return ele1Name;
    cout << "ELE1NAME: " << ele1Name << endl;
}
