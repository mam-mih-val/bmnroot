// ------------------------------------------------------------------------
// -----                     BmnHodoPoint header file                   -----
// ------------------------------------------------------------------------

#ifndef BMNHODOPOINT_H
#define BMNHODOPOINT_H

#include "FairMCPoint.h"
#include "Rtypes.h"
#include "TLorentzVector.h"
#include "TObject.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"

using namespace std;

class BmnHodoPoint : public FairMCPoint {
   public:
    /** Default constructor **/
    BmnHodoPoint();

    /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID (at present, volume MC number)
   *@param pos      Coordinates  [cm]
   *@param mom      Momentum of track [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/

    BmnHodoPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss);

    /** Copy constructor **/
    BmnHodoPoint(const BmnHodoPoint& point) { *this = point; };

    /** Destructor **/
    virtual ~BmnHodoPoint();

    Double_t GetTime() const { return fTime; }
    Double_t GetLengthtrack() const { return fLengthtrack; }

    Double_t GetPx() const { return fPx; }
    Double_t GetPy() const { return fPy; }
    Double_t GetPz() const { return fPz; }

    void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx, fPy, fPz); }

    Double_t GetX() const { return fX; }
    Double_t GetY() const { return fY; }
    Double_t GetZ() const { return fZ; }

    void PositionOut(TVector3& pos) { pos.SetXYZ(fX, fY, fZ); }

    // ---------------------------------------------------------

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

   protected:
    Double_t fX, fY, fZ;
    Double_t fPx, fPy, fPz;
    Double_t fTime;
    Double_t fLengthtrack;

    ClassDef(BmnHodoPoint, 0)
};

#endif
