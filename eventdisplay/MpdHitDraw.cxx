/*
 * MpdHitDraw.cpp
 *
 *  Created on: Apr 16, 2009
 *      Author: stockman
 */

#include "MpdHitDraw.h"

#include "FairHit.h"


TVector3 MpdHitDraw::GetVector(TObject* obj)
{
    FairHit* hit = static_cast<FairHit*>(obj);
    return TVector3(hit->GetX(), hit->GetY(), hit->GetZ());
}

ClassImp(MpdHitDraw)
