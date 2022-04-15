/*************************************************************************************
 *
 *         Class BmnFHCalGeo
 *
 ************************************************************************************/

#include "BmnFHCalGeo.h"

#include <iostream>

#include "FairGeoNode.h"

using namespace std;

ClassImp(BmnFHCalGeo)

// -----   Default constructor   -------------------------------------------
BmnFHCalGeo::BmnFHCalGeo() {
    // Constructor
    fName = "FHCal";
    maxSectors = 10;
    maxModules = 70;
}
// -------------------------------------------------------------------------

const char* BmnFHCalGeo::getModule1Name(Int_t k) {
    // Returns the module name of muo number m

    sprintf(mod1Name, "FHCal0%i", k + 1);
    return mod1Name;
    cout << "MOD1NAME: " << mod1Name << endl;
}

const char* BmnFHCalGeo::getEle1Name(Int_t k) {
    // Returns the element name of muo number m

    sprintf(ele1Name, "s%i", k + 1);
    return ele1Name;
    cout << "ELE1NAME: " << ele1Name << endl;
}
