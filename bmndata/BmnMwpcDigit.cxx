
/* 
 * File:   BmnMwpcDigit.cxx
 * Author: Sergey Merts
 *
 * Created on October 17, 2014, 1:18 PM
 */

#include "BmnMwpcDigit.h"

BmnMwpcDigit::BmnMwpcDigit() {
    fTime = -1;
    fPlane = -1;
    fWire = -1;
    fRef = -1;
}

BmnMwpcDigit::BmnMwpcDigit(Short_t iPlane, Short_t iWire, Short_t iTime, Int_t refId) {
    fTime = iTime;
    fPlane = iPlane;
    fWire = iWire;
    fRef = refId;
}

BmnMwpcDigit::~BmnMwpcDigit() {

}

ClassImp(BmnMwpcDigit)