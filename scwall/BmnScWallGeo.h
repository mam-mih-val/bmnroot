/*************************************************************************************
 *
 *         Class BmnScWallGeo
 *         
 ************************************************************************************/

#ifndef BMNGEOSCWALL_H
#define BMNGEOSCWALL_H

#include "FairGeoSet.h"

class BmnScWallGeo : public FairGeoSet {
   protected:
    char mod1Name[100];  // name of module
    char ele1Name[100];  // substring for elements in module

   public:
    BmnScWallGeo();
    ~BmnScWallGeo() {}
    const char* getModule1Name(Int_t);
    const char* getEle1Name(Int_t);

    ClassDef(BmnScWallGeo, 0)  // Class for Hyp
};

#endif
