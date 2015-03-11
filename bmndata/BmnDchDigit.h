#ifndef BMNDCHDIGIT_H
#define	BMNDCHDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnDchDigit : public TNamed {

public:
    
/** Default constructor **/
    BmnDchDigit();

    /** Constructor to use **/
    BmnDchDigit(Short_t iPlane, Short_t iWire, Short_t iTime, Int_t refId);

    Short_t  GetPlane() const {return fPlane;}
    Short_t  GetWireNumber() const {return fWire;}
    Short_t  GetTime() const {return fTime;}
    Int_t    GetRefId() const {return fRef;}

    void SetPlane(Short_t _tmp) {fPlane = _tmp;}
    void SetWireNumber(Short_t _tmp) {fWire = _tmp;}
    void SetTime(Short_t _tmp) {fTime = _tmp;}
    void SetRefId(Int_t _tmp) {fRef = _tmp;}

    /** Destructor **/
    virtual ~BmnDchDigit();

private:

    Short_t fPlane;
    Short_t fWire;
    Short_t fTime;
    Int_t   fRef;

    ClassDef(BmnDchDigit, 1);

};

#endif	/* BMNDCHDIGIT_H */

