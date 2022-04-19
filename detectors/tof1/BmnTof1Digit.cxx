//--------------------------------------------------------------------------------------------------------------------------------------
#include "BmnTof1Digit.h"

ClassImp(BmnTof1Digit)
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1Digit::BmnTof1Digit()
: fPlane(-1), fStrip(-1), fSide(-1), fAmplitude(-1), fTime(-1)
{

}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1Digit::BmnTof1Digit(Short_t plane, Short_t strip, Short_t side,Float_t t,Float_t a)
: fPlane(plane), fStrip(strip), fSide(side), fAmplitude(a), fTime(t)
{

}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1Digit::BmnTof1Digit(const BmnTof1Digit *ptr, Float_t t, Float_t a)
: fPlane(ptr->fPlane), fStrip(ptr->fStrip), fSide(ptr->fSide), fAmplitude(a), fTime(t)
{

}
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1Digit::~BmnTof1Digit()
{

}
//--------------------------------------------------------------------------------------------------------------------------------------
void	BmnTof1Digit::print(const char* comment, std::ostream& os)const
{
	os<<" [BmnTof1Digit] "; if(nullptr != comment) os<<comment;
	os<<"  detID: "<<fPlane<<", stripID: "<<fStrip<<", stripSide: "<<fSide<<", time: "<<fTime<<", ampl.: "<<fAmplitude<<std::endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------

