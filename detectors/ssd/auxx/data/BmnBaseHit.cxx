/**
 * \file BmnBaseHit.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author D. Baranov
 * \date 2018
 **/
#include "BmnBaseHit.h"
#include "BmnMatch.h"

BmnBaseHit::BmnBaseHit()
  : BmnBaseHit(kHIT, 0., 0., -1, -1, -1., -1.)
{
}

BmnBaseHit::BmnBaseHit(HitType _type, Double_t _z, Double_t _dz,
       Int_t _refId,  Int_t _address,
       Double_t _time, Double_t _timeError)
  : TObject(),
    fType(_type),
    fZ(_z),
    fDz(_dz),
    fRefId(_refId),
    fAddress(_address),
    fTime(_time),
    fTimeError(_timeError),
    fMatch(NULL)
{
}

// Only shallow copy needed
BmnBaseHit::BmnBaseHit(const BmnBaseHit& rhs)
 : TObject(rhs),
   fType(rhs.fType),
   fZ(rhs.fZ),
   fDz(rhs.fDz),
   fRefId(rhs.fRefId),
   fAddress(rhs.fAddress),
   fTime(rhs.fTime),
   fTimeError(rhs.fTimeError),
   fMatch(NULL)
{
}

// Only shallow copy needed
BmnBaseHit& BmnBaseHit::operator=(const BmnBaseHit& rhs)
{

  if (this != &rhs) {
    TObject::operator=(rhs);
    fType = rhs.fType;
    fZ = rhs.fZ;
    fDz = rhs.fDz;
    fRefId = rhs.fRefId;
    fAddress = rhs.fAddress;
    fTime = rhs.fTime;
    fTimeError = rhs.fTimeError;
    fMatch = NULL;
  }
  return *this;
}

BmnBaseHit::~BmnBaseHit()
{
	if ( fMatch ) delete fMatch;
}

void BmnBaseHit::SetMatch(BmnMatch* match) {
	if ( fMatch ) delete fMatch;
	fMatch = match;
}

ClassImp(BmnBaseHit);
