// BmnMwpcHit class

#ifndef BmnMwpcHit_H
#define BmnMwpcHit_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "BmnHit.h"
#include <TObject.h>

using namespace std;

// class TClonesArray;

class BmnMwpcHit : public BmnHit {
public:

    /** Default constructor **/
    BmnMwpcHit();

    /** Constructor to use **/
    BmnMwpcHit(Int_t detUID, TVector3 posHit, TVector3 posHitErr, Int_t pointIndx);

    Int_t GetXaddr() const {
        return fXaddr;
    }

    Int_t GetYaddr() const {
        return fYaddr;
    }

    ULong_t GetAddr() const {
        return fAddr;
    }

    Short_t GetMwpcId() const {
        return fMwpcId;
    }

    Int_t GetHitId() const {
        return fID;
    }

    Bool_t IsUsed() const {
        return fUsing;
    }

    void SetUsing(Bool_t use) {
        fUsing = use;
    }

    void SetXaddr(Int_t addr) {
        fXaddr = addr;
    }

    void SetYaddr(Int_t addr) {
        fYaddr = addr;
    }

    void SetAddr(ULong_t addr) {
        fAddr = addr;
    }

    void SetMwpcId(Short_t id) {
        fMwpcId = id;
    }

    void SetHitId(Int_t idx) {
        fID = idx;
    }
    
    Int_t GetWireNumber() { return fWireNumber; }
    Int_t GetDetectorID() const { return  fDetectorID; } //
    Int_t GetWireTime() { return  fWireTime; } //
    Int_t GetPlaneId() { return  fPlaneId; } //
    Int_t GetPdgId() { return  fPdgId; } //
    void SetWireNumber(Int_t wire_num) { fWireNumber = wire_num; }
    void SetWireTime(Int_t time_wire) { fWireTime = time_wire; }
    void SetPlaneId(Int_t plane_id) { fPlaneId = plane_id; }
    void SetPdgId(Int_t pdg_id) { fPdgId = pdg_id; }

    /** Destructor **/
    virtual ~BmnMwpcHit();

private:

    Int_t   fID; // identifier of hit in hits array
    Bool_t  fUsing;
    Int_t   fXaddr;
    Int_t   fYaddr;
    ULong_t fAddr;
    Short_t fMwpcId; // 1, 2, 3
    
protected:
    Int_t fWireNumber;  // Nearest wire to a MC-track
    Int_t fDetectorID;  // Detector ID
    Int_t fWireTime;
    Int_t fPlaneId;
    Int_t fPdgId;
    
    ClassDef(BmnMwpcHit, 1);

};

#endif
