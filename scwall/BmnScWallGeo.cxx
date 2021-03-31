/*************************************************************************************
 *
 *         Class BmnScWallGeo
 *
 ************************************************************************************/

#include "BmnScWallGeo.h"

#include <iostream>

#include "FairGeoNode.h"

using std::cout;
using std::endl;

ClassImp(BmnScWallGeo)

    // -----   Default constructor   -------------------------------------------
    BmnScWallGeo::BmnScWallGeo() {
    // Constructor
    fName = "ScWall";
    maxSectors = 10;
    maxModules = 70;
}
// -------------------------------------------------------------------------

const char* BmnScWallGeo::getModule1Name(Int_t k) {
    // Returns the module name of muo number m

    sprintf(mod1Name, "ScWall0%i", k + 1);
    return mod1Name;
    cout << "MOD1NAME: " << mod1Name << endl;
}

const char* BmnScWallGeo::getEle1Name(Int_t k) {
    // Returns the element name of muo number m

    sprintf(ele1Name, "s%i", k + 1);
    return ele1Name;
    cout << "ELE1NAME: " << ele1Name << endl;
}
