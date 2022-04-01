// -------------------------------------------------------------------------
// -----                    BmnNewFieldMap header file                 -----
// -----                   Created 03/02/2015  by P. Batyuk            -----
// -------------------------------------------------------------------------

/** Field Map does not have any symmetries w.r.t coordinate axises **/

#ifndef BMNNEWFIELDMAP_H
#define BMNNEWFIELDMAP_H 1

#include "BmnFieldMap.h"
#include "BmnFieldPar.h"
#include "TArrayF.h"


class BmnNewFieldMap : public BmnFieldMap
{
  public:
    BmnNewFieldMap();
    BmnNewFieldMap(const char* mapName);
    BmnNewFieldMap(BmnFieldPar* fieldPar);

    virtual ~BmnNewFieldMap();

    Double_t GetBx(Double_t x, Double_t y, Double_t z);
    Double_t GetBy(Double_t x, Double_t y, Double_t z);
    Double_t GetBz(Double_t x, Double_t y, Double_t z);

    Bool_t IsInside(Double_t x, Double_t y, Double_t z,
                    Int_t& ix, Int_t& iy, Int_t& iz,
                    Double_t& dx, Double_t& dy, Double_t& dz);

    void FillParContainer();

  protected:
    Double_t FieldInterpolate(TArrayF* fcomp, Double_t x, Double_t y, Double_t z);

  ClassDef(BmnNewFieldMap, 1)
};

#endif
