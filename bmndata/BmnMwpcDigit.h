/* 
 * File:   BmnMwpcDigit.h
 * Author: Sergey Merts
 *
 * Created on October 17, 2014, 1:18 PM
 */

#ifndef BMNMWPCDIGIT_H
#define	BMNMWPCDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnMwpcDigit : public TNamed {

public:
    
/** Default constructor **/
    BmnMwpcDigit();

    /** Constructor to use **/
    BmnMwpcDigit(UInt_t iPlane, UInt_t iWire, UInt_t iTime, Int_t refId);
    BmnMwpcDigit(UInt_t iPlane, UInt_t iWire, UInt_t iTime);

    UInt_t  GetPlane() const {return fPlane;}
    UInt_t  GetWireNumber() const {return fWire;}
    UInt_t  GetTime() const {return fTime;}
    Int_t  GetRefId() const {return fRef;}
    Int_t  IsUsed() const {return fUse;}

    void SetPlane(UInt_t _tmp) {fPlane = _tmp;}
    void SetWireNumber(UInt_t _tmp) {fWire = _tmp;}
    void SetTime(UInt_t _tmp) {fTime = _tmp;}
    void SetRefId(Int_t _tmp) {fRef = _tmp;}
    void SetUsing(Bool_t _tmp) {fUse = _tmp;}

    /** Destructor **/
    virtual ~BmnMwpcDigit();

private:

    UInt_t fPlane;
    UInt_t fWire;
    UInt_t fTime; //ns
    Int_t   fRef;
    Bool_t  fUse;

    ClassDef(BmnMwpcDigit, 2);

};

#endif	/* BMNMWPCDIGIT_H */

