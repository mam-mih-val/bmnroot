// -------------------------------------------------------------------------
// -----                    BmnNewFieldMap source file                 -----
// -----                   Created 03/02/2015  by P. Batyuk            -----
// -------------------------------------------------------------------------
#include "BmnNewFieldMap.h"
#include "BmnFieldPar.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "TArrayF.h"
#include "TMath.h"
using namespace TMath;

BmnNewFieldMap::BmnNewFieldMap()
: BmnFieldMap()
{
    fType = 1;
}

BmnNewFieldMap::BmnNewFieldMap(const char* mapName)
: BmnFieldMap(mapName)
{
    fType = 1;
}

BmnNewFieldMap::BmnNewFieldMap(BmnFieldPar* fieldPar)
: BmnFieldMap(fieldPar)
{
    fType = 1;
}

BmnNewFieldMap::~BmnNewFieldMap() {}

Double_t BmnNewFieldMap::GetBx(Double_t x, Double_t y, Double_t z) {
    return FieldInterpolate(fBx, x, y, z);
}

Double_t BmnNewFieldMap::GetBy(Double_t x, Double_t y, Double_t z) {
    return FieldInterpolate(fBy, x, y, z);
}

Double_t BmnNewFieldMap::GetBz(Double_t x, Double_t y, Double_t z) {
    return FieldInterpolate(fBz, x, y, z);
}

Bool_t BmnNewFieldMap::IsInside(Double_t x, Double_t y, Double_t z,
                                Int_t& ix, Int_t& iy, Int_t& iz,
                                Double_t& dx, Double_t& dy, Double_t& dz)
{
    // fPosX = fPosY = fPosZ = 0
    Double_t xl = x;
    Double_t yl = y;
    Double_t zl = z;
       
    // ---  Check for being outside the map range
    if (!(xl >= fXmin && xl < fXmax && yl >= fYmin && yl < fYmax &&
            zl >= fZmin && zl < fZmax)) {
        ix = iy = iz = 0;
        dx = dy = dz = 0.;
        return kFALSE;
    }

    // --- Determine grid cell
    ix = Int_t((xl - fXmin) / fXstep);
    iy = Int_t((yl - fYmin) / fYstep);
    iz = Int_t((zl - fZmin) / fZstep);

    // Relative distance from grid point (in units of cell size)
    dx = (xl - fXmin) / fXstep - Double_t(ix);
    dy = (yl - fYmin) / fYstep - Double_t(iy);
    dz = (zl - fZmin) / fZstep - Double_t(iz);

    return kTRUE;
}

Double_t BmnNewFieldMap::FieldInterpolate(TArrayF* fcomp, Double_t x, Double_t y, Double_t z) {
    Int_t ix = 0;
    Int_t iy = 0;
    Int_t iz = 0;
    Double_t dx = 0.;
    Double_t dy = 0.;
    Double_t dz = 0.;

    if (IsInside(x, y, z, ix, iy, iz, dx, dy, dz)) {
        fHa[0][0][0] = fcomp->At(ix * fNy * fNz + iy * fNz + iz);
        fHa[1][0][0] = fcomp->At((ix + 1) * fNy * fNz + iy * fNz + iz);
        fHa[0][1][0] = fcomp->At(ix * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[1][1][0] = fcomp->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[0][0][1] = fcomp->At(ix * fNy * fNz + iy * fNz + (iz + 1));
        fHa[1][0][1] = fcomp->At((ix + 1) * fNy * fNz + iy * fNz + (iz + 1));
        fHa[0][1][1] = fcomp->At(ix * fNy * fNz + (iy + 1) * fNz + (iz + 1));
        fHa[1][1][1] = fcomp->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + (iz + 1));

        return Interpolate(dx, dy, dz);
    }
    return 0.;
}

void BmnNewFieldMap::FillParContainer()
{
    TString MapName = GetName();
    //  LOG(info) << "BmnNewFieldMap::FillParContainer() ";
    FairRun* fRun = FairRun::Instance();
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    // Bool_t kParameterMerged = kTRUE;
    BmnFieldPar* fieldPar = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
    fieldPar->SetParameters(this);
    fieldPar->setInputVersion(fRun->GetRunId(), 1);
    fieldPar->setChanged();
}

ClassImp(BmnNewFieldMap)
