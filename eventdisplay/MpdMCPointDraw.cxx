/*
 * FairMCPointsDraw.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: stockman
 */

#include "MpdMCPointDraw.h"
#include "MpdEventManager.h"

#include "FairMCPoint.h"

#include "TEveManager.h"


TVector3 MpdMCPointDraw::GetVector(TObject* obj)
{
    FairMCPoint* p = (FairMCPoint*) obj;
    return TVector3(p->GetX(), p->GetY(), p->GetZ());
}

void MpdMCPointDraw::AddEveElementList()
{
    fEventManager->AddEventElement(fq, MCPointList);
    return;
}

void MpdMCPointDraw::RemoveEveElementList()
{
    gEve->RemoveElement(fq, fEventManager->EveMCPoints);
    return;
}

ClassImp(MpdMCPointDraw)
