/*
 * FairMCPointsDraw.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: stockman
 */

#include "FairHitPointSetDraw.h"
#include "FairHit.h"
#include "FairEventManager.h"
#include "FairEventManagerEditor.h"

#include "TEveManager.h"

#include <iostream>
using namespace std;

FairHitPointSetDraw::FairHitPointSetDraw()
{
}

FairHitPointSetDraw::~FairHitPointSetDraw()
{
}

TVector3 FairHitPointSetDraw::GetVector(TObject* obj)
{
    FairHit* hit = (FairHit*) obj;
    if (fVerbose > 2) cout<<"FairHitPointSetDraw::GetVector(): "<<hit->GetX()<<" "<<hit->GetY()<<" "<<hit->GetZ()<<endl;

    return TVector3(hit->GetX(), hit->GetY(), hit->GetZ());
}

void FairHitPointSetDraw::AddEveElementList()
{
    fEventManager->AddEventElement(fq, RecoPointList);
    return;
}

void FairHitPointSetDraw::RemoveEveElementList()
{
    gEve->RemoveElement(fq, fEventManager->EveRecoPoints);
    return;
}

ClassImp(FairHitPointSetDraw)
