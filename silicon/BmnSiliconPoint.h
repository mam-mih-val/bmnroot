#ifndef BMNSILICONPOINT_H
#define BMNSILICONPOINT_H

#include "FairMCPoint.h"

class BmnSiliconPoint : public FairMCPoint {

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
    BmnSiliconPoint(Int_t trackID, Int_t detID,
                    TVector3 posIn, TVector3 posOut,
                    TVector3 momIn, TVector3 momOut,
                    Double_t tof, Double_t length, Double_t eLoss,
                    Int_t isPrimary, Double_t charge, Int_t pdgId);

    BmnSiliconPoint(const BmnSiliconPoint& point) { *this = point; };

    BmnSiliconPoint();

    virtual ~BmnSiliconPoint();

    /** Accessors **/
    Double_t GetXIn()   const { return fX; }
    Double_t GetYIn()   const { return fY; }
    Double_t GetZIn()   const { return fZ; }
    Double_t GetXOut()  const { return fX_out; }
    Double_t GetYOut()  const { return fY_out; }
    Double_t GetZOut()  const { return fZ_out; }
    Double_t GetPxOut() const { return fPx_out; }
    Double_t GetPyOut() const { return fPy_out; }
    Double_t GetPzOut() const { return fPz_out; }
    void PositionIn(TVector3& pos)  { pos.SetXYZ(fX, fY, fZ); }
    void PositionOut(TVector3& pos) { pos.SetXYZ(fX_out,fY_out,fZ_out); }
    void MomentumIn(TVector3& mom) { mom.SetXYZ(fPx,fPy,fPz); }
    void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx_out,fPy_out,fPz_out); }

    Int_t GetIsPrimary() { return fIsPrimary; }
    Double_t GetCharge() { return fCharge; }
    Double_t GetPdgId() { return fPdgId; }

    Int_t GetStation() { return fStation; }
    Int_t GetModule() { return fModule; }

    void SetStation(Int_t station_num) { fStation = station_num; }
    void SetModule(Int_t module_num) { fModule = module_num; }

    // Output to screen
    virtual void Print(const Option_t* opt) const;

protected:

    Int_t fIsPrimary;
    Double_t fCharge;
    Int_t fPdgId;

    Double_t fX_out,  fY_out,  fZ_out;
    Double_t fPx_out, fPy_out, fPz_out;

    Int_t fStation;
    Int_t fModule;

    ClassDef(BmnSiliconPoint,1)
};

#endif /* BMNSILICONPOINT_H */

