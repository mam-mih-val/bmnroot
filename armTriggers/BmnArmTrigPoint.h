// ------------------------------------------------------------------------
// -----                     BmnArmTrigPoint header file                   -----
// ------------------------------------------------------------------------

#ifndef BMNARMTRIGPOINT_H
#define BMNARMTRIGPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"
#include "Rtypes.h"   
#include "TVirtualMC.h"
#include "TParticle.h"

using namespace std;

class BmnArmTrigPoint : public FairMCPoint {
public:

    /** Default constructor **/
    BmnArmTrigPoint();


    /** Constructor with arguments
     *@param trackID  Index of MCTrack
     *@param detID    Detector ID (at present, volume MC number)
     *@param copyNo         Number of active layer inside ArmTrig module
     *@param copyNoMother   ArmTrig module number
     *@param pos      Coordinates  [cm]
     *@param mom      Momentum of track [GeV]
     *@param tof      Time since event start [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     **/

    BmnArmTrigPoint(Int_t trackID, Int_t detID,
            Int_t copyNo,
            TVector3 posIn, TVector3 posOut,
            TVector3 momIn, TVector3 momOut,
            Double_t tof, Double_t length, Double_t eLoss,
            Int_t isPrimary, Double_t charge, Int_t pdgId,
            Double_t timeIn, Double_t timeOut, Double_t lengthtrack);
    // UInt_t EventId, 

    /** Copy constructor **/
    BmnArmTrigPoint(const BmnArmTrigPoint& point) {
        *this = point;
    };


    /** Destructor **/
    virtual ~BmnArmTrigPoint();

    /** Accessors **/
    Short_t GetCopy() const {
        return nCopy;
    };
    //  Short_t GetCopyMother()  const {return nCopyMother; };

    Int_t GetIsPrimary() {
        return fIsPrimary;
    }

    Double_t GetCharge() {
        return fCharge;
    }

    Double_t GetPdgId() {
        return fPdgId;
    }

    Double_t GetTimeIn() const {
        return fTimeIn;
    }

    Double_t GetTimeOut() const {
        return fTimeOut;
    }

    Double_t GetLengthtrack() const {
        return fLengthtrack;
    }

    Double_t GetPxOut() const {
        return fPxOut;
    }

    Double_t GetPyOut() const {
        return fPyOut;
    }

    Double_t GetPzOut() const {
        return fPzOut;
    }

    void MomentumOut(TVector3& mom) {
        mom.SetXYZ(fPxOut, fPyOut, fPzOut);
    }

    Double_t GetXOut() const {
        return fXOut;
    }

    Double_t GetYOut() const {
        return fYOut;
    }

    Double_t GetZOut() const {
        return fZOut;
    }

    void PositionOut(TVector3& pos) {
        pos.SetXYZ(fXOut, fYOut, fZOut);
    }

    // ---------------------------------------------------------

    /** Modifiers **/
    void SetCopy(Short_t i) {
        nCopy = i;
    };
    
    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;


protected:

    Double_t fXOut, fYOut, fZOut;
    Double_t fPxOut, fPyOut, fPzOut;

    Short_t nCopy; // Copy number
    //  Short_t nCopyMother;          // Copy number of mother volume


    Int_t fIsPrimary;
    Double_t fCharge;
    Int_t fPdgId;
    Double_t fTimeIn;
    Double_t fTimeOut;
    Double_t fLengthtrack;


    ClassDef(BmnArmTrigPoint, 1)
};


#endif
