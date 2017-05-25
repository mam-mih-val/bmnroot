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
    if (fEventManager->EveMCPoints == NULL)
    {
        fEventManager->EveMCPoints = new TEveElementList("MC points");
        gEve->AddElement(fEventManager->EveMCPoints, fEventManager);
        fEventManager->EveMCPoints->SetRnrState(kFALSE);
        fEventManager->GetEventEditor()->fShowMCPoints->SetEnabled(kTRUE);
    }

    gEve->AddElement(fq, fEventManager->EveMCPoints);

    return;
}

void FairMCPointDraw::RemoveEveElementList()
{
    gEve->RemoveElement(fq, fEventManager->EveMCPoints);
    return;
}

ClassImp(FairMCPointDraw)
