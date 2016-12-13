
/* 
 * File:   BmnMwpcDigit.cxx
 * Author: Sergey Merts
 *
 * Created on October 17, 2014, 1:18 PM
 */

#include "BmnMwpcDigit.h"

BmnMwpcDigit::BmnMwpcDigit() {
    fTime = 0.0;
    fPlane = -1;
    fWire = -1;
    fRef = -1;
    fUse = kFALSE;
}

BmnMwpcDigit::BmnMwpcDigit(UInt_t iPlane, UInt_t iWire, Float_t iTime, Int_t refId) {
    fTime = iTime;
    fPlane = iPlane;
    fWire = iWire;
    fRef = refId;
    fUse = kFALSE;
}

BmnMwpcDigit::BmnMwpcDigit(UInt_t iPlane, UInt_t iWire) {
    fTime = 0.0;
    fPlane = iPlane;
    fWire = iWire;
    fRef = -1;
    fUse = kFALSE;
}

BmnMwpcDigit::~BmnMwpcDigit() {

}

ClassImp(BmnMwpcDigit)