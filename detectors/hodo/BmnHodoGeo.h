/*************************************************************************************
 *
 *         Class BmnHodoGeo
 *         
 ************************************************************************************/

#ifndef BMNGEOHODO_H
#define BMNGEOHODO_H

#include "FairGeoSet.h"

class BmnHodoGeo : public FairGeoSet {
   protected:
    char mod1Name[100];  // name of module
    char ele1Name[100];  // substring for elements in module

   public:
    BmnHodoGeo();
    ~BmnHodoGeo() {}
    const char* getModule1Name(Int_t);
    const char* getEle1Name(Int_t);

    ClassDef(BmnHodoGeo, 0)  // Class for Hyp
};

#endif
