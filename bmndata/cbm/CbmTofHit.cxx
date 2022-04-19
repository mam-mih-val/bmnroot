/**
 * \file CbmTofHit.cxx
 * \author E. Cordier
**/

#include "CbmTofHit.h"

#include <sstream>
using std::stringstream;
using std::endl;

CbmTofHit::CbmTofHit()
  : BmnHit(),
    fTime(0.),
    fChannel(0)
{
  SetFlag(1);
  SetType(kTOFHIT_o);
}

CbmTofHit::CbmTofHit(Int_t address, TVector3 pos, TVector3 dpos, Int_t index, Double_t time, Int_t flag, Int_t channel)
  : BmnHit(address, pos, dpos, index),
    fTime(time),
    fChannel(channel)
{
  SetFlag(flag);
  SetType(kTOFHIT_o);
}

CbmTofHit::CbmTofHit(Int_t address, TVector3 pos, TVector3 dpos, Int_t index, Double_t time, Int_t flag)
  : BmnHit(address, pos, dpos, index),
    fTime(time),
    fChannel(0)
{
  SetFlag(flag);
  SetType(kTOFHIT_o);
}

CbmTofHit::CbmTofHit(Int_t address, TVector3 pos, TVector3 dpos, Int_t index, Double_t time)
  : BmnHit(address, pos, dpos, index),
    fTime(time),
    fChannel(0)
{
    SetFlag(1);
    SetType(kTOFHIT_o);
}

CbmTofHit::~CbmTofHit()
{

}

string CbmTofHit::ToString() const
{
   stringstream ss;
   ss << "CbmTofHit: detId=" << GetDetId()
       << " pos=(" << GetX() << "," << GetY() << "," << GetZ()
       << ") err=(" << GetDx() << "," << GetDy() << "," << GetDz()
       << " refId=" << GetRefId()
       << " time=" << GetTime() << " flag=" << GetFlag()
       << " channel=" << GetCh() << endl;
   return ss.str();
}

ClassImp(CbmTofHit)
