// ---------------------------------------------------------------------------------
// -----                    BmnFieldMap header file                    -------------
// -----                Created 03/02/2015  by P. Batyuk               -------------
// ---------------------------------------------------------------------------------
#ifndef BMNFIELDMAP_H
#define BMNFIELDMAP_H 1

#include "BmnFieldMapData.h"
#include "BmnFieldPar.h"
#include "FairField.h"
#include "TArrayF.h"

typedef struct {
    Int_t N;
    Float_t min;
    Float_t max;
    Float_t step;
} coordinate_info_t;

class BmnFieldMap : public FairField
{
  public:
    BmnFieldMap();
    BmnFieldMap(const char* mapName);
    BmnFieldMap(BmnFieldPar* fieldPar);
    virtual ~BmnFieldMap();

    virtual void Init();

    virtual Double_t GetBx(Double_t x, Double_t y, Double_t z) = 0;
    virtual Double_t GetBy(Double_t x, Double_t y, Double_t z) = 0;
    virtual Double_t GetBz(Double_t x, Double_t y, Double_t z) = 0;

    virtual Bool_t IsInside(Double_t x, Double_t y, Double_t z,
                            Int_t& ix, Int_t& iy, Int_t& iz,
                            Double_t& dx, Double_t& dy, Double_t& dz) = 0;

    /** Write the field map to an ASCII file **/
    void WriteAsciiFile(const char* fileName);

    /** Write field map data to a ROOT file **/
    void WriteRootFile(const char* fileName, const char* mapName);

    /** Set the position of the field centre **/
    void SetPosition(Double_t x, Double_t y, Double_t z);

    /** Set a global field scaling factor **/
    void SetScale(Double_t factor) { fScale = factor; fIsOff = (factor == 0 ? kTRUE : kFALSE); }

    /** Turn magnetic field off **/
    void SetFieldOff(Bool_t is_off = kTRUE) { fIsOff = is_off; }

    /** Set field map file path **/
    void SetFileName(const char* file_name) { fFileName = file_name; }

    /** Accessors to field parameters in local coordinate system **/
    Double_t GetXmin() const { return fXmin; }
    Double_t GetYmin() const { return fYmin; }
    Double_t GetZmin() const { return fZmin; }

    Double_t GetXmax() const { return fXmax; }
    Double_t GetYmax() const { return fYmax; }
    Double_t GetZmax() const { return fZmax; }

    Double_t GetXstep() const { return fXstep; }
    Double_t GetYstep() const { return fYstep; }
    Double_t GetZstep() const { return fZstep; }

    Int_t GetNx() const { return fNx; }
    Int_t GetNy() const { return fNy; }
    Int_t GetNz() const { return fNz; }

    /** Accessor to field centre position in global system **/
    Double_t GetPositionX() const { return fPosX; }
    Double_t GetPositionY() const { return fPosY; }
    Double_t GetPositionZ() const { return fPosZ; }

    /** Accessor to global scaling factor  **/
    Double_t GetScale() const { return fScale; }

    /** Whether magnetic field is off **/
    Bool_t IsFieldOff() const { return fIsOff; }

    /** Accessors to the field value arrays **/
    TArrayF* GetBx() const { return fBx; }
    TArrayF* GetBy() const { return fBy; }
    TArrayF* GetBz() const { return fBz; }

    /** Accessor to field map file **/
    const char* GetFileName() { return fFileName.Data(); }

    /** Screen output **/
    void Print(Option_t*);

    virtual void FillParContainer() = 0;

    /** Reset the field parameters and data **/
    void Reset();

  protected:
    /** Reset the field parameters and data **/
    //void Reset();

    /** Read the field map from an ASCII file **/
    void ReadAsciiFile(const char* fileName);

    /** Read field map from a ROOT file **/
    void ReadRootFile(const char* fileName);

    /** Set field parameters and data **/
    void SetField(const BmnFieldMapData* data);

    /** Get field values by interpolation of the grid.
     ** @param dx,dy,dz  Relative distance from grid point [cell units]
     **/
    Double_t Interpolate(Double_t dx, Double_t dy, Double_t dz);

    /** Map file name **/
    TString fFileName;

    /** Global scaling factor (w.r.t. map on file) **/
    Double_t fScale;

    /** Field centre position in global coordinates  **/
    Double_t fPosX, fPosY, fPosZ;

    /** Field limits in local coordinate system **/
    Double_t fXmin, fXmax, fXstep;
    Double_t fYmin, fYmax, fYstep;
    Double_t fZmin, fZmax, fZstep;

    /** Number of grid points  **/
    Int_t fNx, fNy, fNz;

    /** Arrays with the field values  **/
    TArrayF* fBx;
    TArrayF* fBy;
    TArrayF* fBz;

    /** Variables for temporary storage 
     ** Used in the very frequently called method GetFieldValue  **/
    Double_t fHa[2][2][2]; //! Field at corners of a grid cell
    Double_t fHb[2][2]; //! Interpolated field (2-dim)
    Double_t fHc[2]; //! Interpolated field (1-dim)

    Bool_t fDebugInfo;
    /** Whether magnetic field is off **/
    Bool_t fIsOff;

private:
    BmnFieldMap(const BmnFieldMap&) = delete;
    BmnFieldMap& operator=(const BmnFieldMap&) = delete;

  ClassDef(BmnFieldMap, 2)
};

#endif
