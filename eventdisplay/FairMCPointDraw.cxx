/*
 * FairMCPointsDraw.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: stockman
 */

#include "FairMCPointDraw.h"
#include "FairMCPoint.h"
#include "FairEventManager.h"
#include "FairEventManagerEditor.h"

#include "TEveManager.h"

FairMCPointDraw::FairMCPointDraw()
{
}

FairMCPointDraw::~FairMCPointDraw()
{
}

TVector3 FairMCPointDraw::GetVector(TObject* obj)
{
    FairMCPoint* p = (FairMCPoint*) obj;
    return TVector3(p->GetX(), p->GetY(), p->GetZ());
}

void FairMCPointDraw::AddEveElementList()
{
    fEventManager->AddEventElement(fq, MCPointList);
    return;
}

void FairMCPointDraw::RemoveEveElementList()
{
    gEve->RemoveElement(fq, fEventManager->EveMCPoints);
    return;
}

ClassImp(FairMCPointDraw)
