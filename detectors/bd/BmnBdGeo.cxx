#include "BmnBdGeo.h"
#include "FairGeoNode.h"
#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
BmnBdGeo::BmnBdGeo()
{
    fName="BD";
    maxSectors = 10;
    maxModules = 100;
}

// Return the module name for number q
const char* BmnBdGeo::getModule2Name(Int_t q)
{
    sprintf(mod2Name, "BD0%i", q+1);
    //cout << "MOD2NAME: " << mod2Name << endl;
    return mod2Name;
}

// Returns the element name for number q
const char* BmnBdGeo::getEle2Name(Int_t q)
{
    sprintf(ele2Name, "s%i", q+1);
    //cout << "ELE2NAME: " << ele2Name << endl;
    return ele2Name;
}


ClassImp(BmnBdGeo)
