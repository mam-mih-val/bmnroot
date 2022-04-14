/*************************************************************************************
 *
 *         Class BmnFHCalGeo
 *
 ************************************************************************************/

#ifndef BMNGEOFHCAL_H
#define BMNGEOFHCAL_H

#include "FairGeoSet.h"

class BmnFHCalGeo : public FairGeoSet {
protected:
    char mod1Name[100];  // name of module
    char ele1Name[100];  // substring for elements in module

public:
    BmnFHCalGeo();
    ~BmnFHCalGeo() {}
    const char* getModule1Name(Int_t);
    const char* getEle1Name(Int_t);

    ClassDef(BmnFHCalGeo, 0)  // Class for Hyp
};

#endif
