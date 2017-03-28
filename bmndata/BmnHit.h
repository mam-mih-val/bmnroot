/* 
 * File:   BmnHit.h
 * Author: Sergey Merts
 *
 * Created on October 28, 2014, 11:26 AM
 */

#ifndef BMNHIT_H
#define	BMNHIT_H

#include "FairHit.h"
#include <TObject.h>
#include "CbmDetectorList.h"

using namespace std;

// class TClonesArray;

class BmnHit : public FairHit {
public:

    /** Default constructor **/
    BmnHit();

    /** Constructor to use **/
    BmnHit(Int_t detUID, TVector3 posHit, TVector3 posHitErr, Int_t pointIndx);

    Int_t GetXaddr() const {
        return fXaddr;
    }

    Int_t GetYaddr() const {
        return fYaddr;
    }

    Long_t GetAddr() const {
        return fAddr;
    }

    Bool_t IsUsed() const {
        return fUsing;
    }

    Bool_t GetFlag() const {
        return fFlag;
    }

    Int_t GetIndex() const {
        return fIndex;
    }

    DetectorId GetDetId() const {
        return fDetId;
    }

    Short_t GetStation() const {
        return fStation;
    }

    void SetFlag(Bool_t fl) {
        fFlag = fl;
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

    void SetAddr(Long_t addr) {
        fAddr = addr;
    }

    void SetIndex(Int_t id) {
        fIndex = id;
    }

    void SetLength(Double_t len) {
        fLength = len;
    }

    void SetDetId(DetectorId det) {
        fDetId = det;
    }

    void SetStation(Short_t st) {
        fStation = st;
    }

    void SetType(Int_t type) {
        fType = type;
    }

    Int_t GetType() {
        return fType;
    }
    
    Double_t GetLength() {
        return fLength;
    }

    /** Destructor **/
    virtual ~BmnHit();

private:

    /** Is hit used or not **/
    Bool_t fUsing;
    /** Some additional buffer addresses for seeding**/
    Int_t fXaddr;
    Int_t fYaddr;
    Long_t fAddr;
    /** Is hit good or not **/
    Bool_t fFlag;
    /** Index in hits array **/
    Int_t fIndex;
    /** Id of detector which creates hit **/
    DetectorId fDetId;
    /** station number. It essential for gem, mwpc**/
    Short_t fStation;
    Int_t fType; // 0 - fake, 1 - hit, -1 - undefined
    Double_t fLength; //length from vertex to current hit

    ClassDef(BmnHit, 1);

};
#endif	/* BMNHIT_H */

