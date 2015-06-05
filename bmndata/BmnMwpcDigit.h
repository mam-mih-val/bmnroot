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
    BmnMwpcDigit(Short_t iPlane, Float_t iWire, Short_t iTime, Int_t refId);

    Short_t  GetPlane() const {return fPlane;}
    Float_t  GetWireNumber() const {return fWire;}
    Short_t  GetTime() const {return fTime;}
    Int_t  GetRefId() const {return fRef;}
    Int_t  IsUsed() const {return fUse;}

    void SetPlane(Short_t _tmp) {fPlane = _tmp;}
    void SetWireNumber(Float_t _tmp) {fWire = _tmp;}
    void SetTime(Short_t _tmp) {fTime = _tmp;}
    void SetRefId(Int_t _tmp) {fRef = _tmp;}
    void SetUsing(Bool_t _tmp) {fUse = _tmp;}

    /** Destructor **/
    virtual ~BmnMwpcDigit();

private:

    Short_t fPlane;
    Float_t fWire;
    Short_t fTime;
    Int_t   fRef;
    Bool_t  fUse;

    ClassDef(BmnMwpcDigit, 2);

};

#endif	/* BMNMWPCDIGIT_H */

