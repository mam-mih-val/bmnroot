#ifndef BMNSiBTPOINT_H
#define BMNSiBTPOINT_H

#include "FairMCPoint.h"

class BmnSiBTPoint : public FairMCPoint {

public:

    /** Constructor with arguments
    *@param trackID  Index of MCTrack
    *@param detID    Detector ID
    *@param posIn    Coordinates at entrance to active volume [cm]
    *@param posOut   Coordinates at exit of active volume [cm]
    *@param momIn    Momentum of track at entrance [GeV]
    *@param momOut   Momentum of track at exit [GeV]
    *@param tof      Time since event start [ns]
    *@param length   Track length since creation [cm]
    *@param eLoss    Energy deposit [GeV]
    **/
    BmnSiBTPoint(Int_t trackID, Int_t detID,
                TVector3 posIn, TVector3 posOut,
                TVector3 momIn, TVector3 momOut,
                Double_t tof, Double_t length, Double_t eLoss,
                Int_t isPrimary, Double_t charge, Int_t pdgId);

    BmnSiBTPoint(const BmnSiBTPoint& point) { *this = point; };

    BmnSiBTPoint();

    virtual ~BmnSiBTPoint();

    /** Accessors **/
    Double_t GetXIn()   const { return fX; }
    Double_t GetYIn()   const { return fY; }
    Double_t GetZIn()   const { return fZ; }
    Double_t GetXOut()  const { return fX_out; }
    Double_t GetYOut()  const { return fY_out; }
    Double_t GetZOut()  const { return fZ_out; }
    Double_t GetXCenter()  const { return fX_center; }
    Double_t GetYCenter()  const { return fY_center; }
    Double_t GetZCenter()  const { return fZ_center; }
    Double_t GetPxOut() const { return fPx_out; }
    Double_t GetPyOut() const { return fPy_out; }
    Double_t GetPzOut() const { return fPz_out; }
    void PositionIn(TVector3& pos)  { pos.SetXYZ(fX, fY, fZ); }
    void PositionOut(TVector3& pos) { pos.SetXYZ(fX_out, fY_out, fZ_out); }
    void PositionCenter(TVector3& pos) { pos.SetXYZ(fX_center, fY_center, fZ_center); }
    void MomentumIn(TVector3& mom) { mom.SetXYZ(fPx, fPy, fPz); }
    void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx_out, fPy_out, fPz_out); }
    Int_t GetStation() const { return fStation; }
    Int_t GetModule() const { return fModule; }

    Int_t GetIsPrimary() { return fIsPrimary; }
    Double_t GetCharge() { return fCharge; }
    Double_t GetPdgId() { return fPdgId; }

    void SetStation(Int_t station) { fStation = station; }
    void SetModule(Int_t module) { fModule = module; }

    // Output to screen
    virtual void Print(const Option_t* opt) const;

protected:

    Int_t fIsPrimary;
    Double_t fCharge;
    Int_t fPdgId;

    Double_t fX_out,  fY_out,  fZ_out;
    Double_t fX_center,  fY_center,  fZ_center; //middle pos. (hit position)
    Double_t fPx_out, fPy_out, fPz_out;

    Int_t fStation;
    Int_t fModule;

    ClassDef(BmnSiBTPoint,1)
};

#endif /* BMNSiBTPOINT_H */

