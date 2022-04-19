
/* 
 * File:   BmnMwpcDigit.cxx
 * Author: Sergey Merts
 *
 * Created on October 17, 2014, 1:18 PM
 */

#include "BmnMwpcDigit.h"

BmnMwpcDigit::BmnMwpcDigit() {
    fTime = 0;
    fStation = -1;
    fPlane = -1;
    fWire = -1;
    fRef = -1;
    fUse = kFALSE;
}

BmnMwpcDigit::BmnMwpcDigit(Short_t iStation, Short_t iPlane, Int_t iWire, UInt_t iTime) {
    fTime = iTime;
    fStation = iStation;
    fPlane = iPlane;
    fWire = iWire;
    fRef = -1;
    fUse = kFALSE;
}

BmnMwpcDigit::~BmnMwpcDigit() {

}

ClassImp(BmnMwpcDigit)