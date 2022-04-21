// -------------------------------------------------------------------------
// -----                    BmnFieldMapSym3 source file                -----
// -----         Created 12/01/04  by M. Al/Turany (BmnField.cxx)      -----
// -----                Redesign 13/02/06  by V. Friese                -----
// -------------------------------------------------------------------------

#include "BmnFieldMapSym3.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "TArrayF.h"
#include "TMath.h"

using namespace TMath;

// -------------   Default constructor  ----------------------------------
BmnFieldMapSym3::BmnFieldMapSym3()
: BmnFieldMap(),
fHemiX(0.),
fHemiY(0.),
fHemiZ(0.)
{
    fType = 3;
}

// -------------   Standard constructor   ---------------------------------
BmnFieldMapSym3::BmnFieldMapSym3(const char* mapName)
: BmnFieldMap(mapName),
fHemiX(0.),
fHemiY(0.),
fHemiZ(0.)
{
    fType = 3;
}

// -------------   Constructor from BmnFieldPar   -------------------------
BmnFieldMapSym3::BmnFieldMapSym3(BmnFieldPar* fieldPar)
: BmnFieldMap(fieldPar),
fHemiX(0.),
fHemiY(0.),
fHemiZ(0.)
{
    fType = 3;
}

// ------------   Destructor   --------------------------------------------
BmnFieldMapSym3::~BmnFieldMapSym3() {
}

// -----------   Get x component of the field   ---------------------------
Double_t BmnFieldMapSym3::GetBx(Double_t x, Double_t y, Double_t z)
{
    Int_t ix = 0;
    Int_t iy = 0;
    Int_t iz = 0;
    Double_t dx = 0.;
    Double_t dy = 0.;
    Double_t dz = 0.;

    if (IsInside(x, y, z, ix, iy, iz, dx, dy, dz)) {

        // Get Bx field values at grid cell corners
        fHa[0][0][0] = fBx->At(ix * fNy * fNz + iy * fNz + iz);
        fHa[1][0][0] = fBx->At((ix + 1) * fNy * fNz + iy * fNz + iz);
        fHa[0][1][0] = fBx->At(ix * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[1][1][0] = fBx->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[0][0][1] = fBx->At(ix * fNy * fNz + iy * fNz + (iz + 1));
        fHa[1][0][1] = fBx->At((ix + 1) * fNy * fNz + iy * fNz + (iz + 1));
        fHa[0][1][1] = fBx->At(ix * fNy * fNz + (iy + 1) * fNz + (iz + 1));
        fHa[1][1][1] = fBx->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + (iz + 1));

        // Return interpolated field value
        return Interpolate(dx, dy, dz) * fHemiX * fHemiY;

    }

    return 0.;
}

// -----------   Get y component of the field   ---------------------------
Double_t BmnFieldMapSym3::GetBy(Double_t x, Double_t y, Double_t z)
{
    Int_t ix = 0;
    Int_t iy = 0;
    Int_t iz = 0;
    Double_t dx = 0.;
    Double_t dy = 0.;
    Double_t dz = 0.;

    if (IsInside(x, y, z, ix, iy, iz, dx, dy, dz)) {

        // Get By field values at grid cell corners
        fHa[0][0][0] = fBy->At(ix * fNy * fNz + iy * fNz + iz);
        fHa[1][0][0] = fBy->At((ix + 1) * fNy * fNz + iy * fNz + iz);
        fHa[0][1][0] = fBy->At(ix * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[1][1][0] = fBy->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[0][0][1] = fBy->At(ix * fNy * fNz + iy * fNz + (iz + 1));
        fHa[1][0][1] = fBy->At((ix + 1) * fNy * fNz + iy * fNz + (iz + 1));
        fHa[0][1][1] = fBy->At(ix * fNy * fNz + (iy + 1) * fNz + (iz + 1));
        fHa[1][1][1] = fBy->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + (iz + 1));

        // Return interpolated field value
        return Interpolate(dx, dy, dz);
    }
    return 0.;
}

// -----------   Get z component of the field   ---------------------------
Double_t BmnFieldMapSym3::GetBz(Double_t x, Double_t y, Double_t z)
{
    Int_t ix = 0;
    Int_t iy = 0;
    Int_t iz = 0;
    Double_t dx = 0.;
    Double_t dy = 0.;
    Double_t dz = 0.;

    if (IsInside(x, y, z, ix, iy, iz, dx, dy, dz)) {

        // Get Bz field values at grid cell corners
        fHa[0][0][0] = fBz->At(ix * fNy * fNz + iy * fNz + iz);
        fHa[1][0][0] = fBz->At((ix + 1) * fNy * fNz + iy * fNz + iz);
        fHa[0][1][0] = fBz->At(ix * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[1][1][0] = fBz->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + iz);
        fHa[0][0][1] = fBz->At(ix * fNy * fNz + iy * fNz + (iz + 1));
        fHa[1][0][1] = fBz->At((ix + 1) * fNy * fNz + iy * fNz + (iz + 1));
        fHa[0][1][1] = fBz->At(ix * fNy * fNz + (iy + 1) * fNz + (iz + 1));
        fHa[1][1][1] = fBz->At((ix + 1) * fNy * fNz + (iy + 1) * fNz + (iz + 1));

        // Return interpolated field value
        return Interpolate(dx, dy, dz) * fHemiY * fHemiZ;

    }

    return 0.;
}

// -----------   Check whether a point is inside the map   ----------------
Bool_t BmnFieldMapSym3::IsInside(Double_t x, Double_t y, Double_t z,
                                 Int_t& ix, Int_t& iy, Int_t& iz,
                                 Double_t& dx, Double_t& dy, Double_t& dz)
{
    // --- Transform into local coordinate system
    Double_t xl = x - fPosX;
    Double_t yl = y - fPosY;
    Double_t zl = z - fPosZ;

    // ---  Reflect w.r.t. symmetry axes
    fHemiX = fHemiY = fHemiZ = 1.;
    if (xl < 0.) {
        fHemiX = -1.;
        xl = -1. * xl;
    }
    if (yl < 0.) {
        fHemiY = -1.;
        yl = -1. * yl;
    }
    if (zl < 0.) {
        fHemiZ = -1.;
        zl = -1. * zl;
    }

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

void BmnFieldMapSym3::FillParContainer()
{
    TString MapName = GetName();
    //  LOG(info) << "BmnFieldMapSym3::FillParContainer() ";
    FairRun* fRun = FairRun::Instance();
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    // Bool_t kParameterMerged = kTRUE;
    BmnFieldPar* fieldPar = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
    fieldPar->SetParameters(this);
    fieldPar->setInputVersion(fRun->GetRunId(), 1);
    fieldPar->setChanged();
}

ClassImp(BmnFieldMapSym3)
