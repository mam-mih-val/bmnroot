#ifndef BMNREGION_H
#define BMNREGION_H 1
#include "TObject.h"

class BmnRegion : public TObject {
public:

    /** Default constructor **/
    BmnRegion(Double_t Zmin, Double_t Zmax);
    /** Destructor **/
    virtual ~BmnRegion();
    Bool_t IsInside(Double_t Z);
    ClassDef(BmnRegion, 1)

protected:
    Double_t fZmin;
    Double_t fZmax;
};

#endif
