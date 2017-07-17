/*
 * FairTimeStampPointDraw.cxx
 *
 *  Created on: Feb 28, 2011
 *      Author: stockman
 */

#include "FairTimeStampPointDraw.h"

#include "FairMCPoint.h"
#include "FairTimeStamp.h"


FairTimeStampPointDraw::FairTimeStampPointDraw()
{
}

FairTimeStampPointDraw::FairTimeStampPointDraw(const char* name, Int_t iVerbose)
  : FairBoxSetDraw(name, iVerbose)
{
}

FairTimeStampPointDraw::~FairTimeStampPointDraw()
{
}

TVector3 FairTimeStampPointDraw::GetVector(TObject* obj)
{
  FairMCPoint* hit = (FairMCPoint*) obj;
  return TVector3(hit->GetX(), hit->GetY(), hit->GetZ());
}

Int_t FairTimeStampPointDraw::GetValue(TObject* obj, Int_t i)
{
  FairTimeStamp* tsdata = (FairTimeStamp*)obj;
  if (tsdata > 0)
    return (Int_t) tsdata->GetTimeStamp();

  return 0;
}

ClassImp(FairTimeStampPointDraw);
