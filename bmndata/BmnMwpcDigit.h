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
    BmnMwpcDigit(Short_t iPlane, Short_t iWire, Short_t iTime, Int_t refId);

    Short_t  GetPlane() const {return fPlane;}
    Short_t  GetWireNumber() const {return fWire;}
    Short_t  GetTime() const {return fTime;}
    Int_t  GetRefId() const {return fRef;}

    void SetPlane(Short_t _tmp) {fPlane = _tmp;}
    void SetWireNumber(Short_t _tmp) {fWire = _tmp;}
    void SetTime(Short_t _tmp) {fTime = _tmp;}
    void SetRefId(Int_t _tmp) {fRef = _tmp;}

    /** Destructor **/
    virtual ~BmnMwpcDigit();

private:

    Short_t fPlane;
    Short_t fWire;
    Short_t fTime;
    Int_t   fRef;

    ClassDef(BmnMwpcDigit, 1);

};

#endif	/* BMNMWPCDIGIT_H */

