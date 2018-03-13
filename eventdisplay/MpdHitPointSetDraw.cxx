/*
 * FairMCPointsDraw.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: stockman
 */

#include "MpdHitPointSetDraw.h"
#include "MpdEventManager.h"

#include "FairHit.h"

#include "TEveManager.h"

#include <iostream>
using namespace std;


TVector3 MpdHitPointSetDraw::GetVector(TObject* obj)
{
    FairHit* hit = (FairHit*) obj;
    if (fVerbose > 2) cout<<"MpdHitPointSetDraw::GetVector(): "<<hit->GetX()<<" "<<hit->GetY()<<" "<<hit->GetZ()<<endl;

    return TVector3(hit->GetX(), hit->GetY(), hit->GetZ());
}

void MpdHitPointSetDraw::AddEveElementList()
{
    fEventManager->AddEventElement(fq, RecoPointList);
    return;
}

void MpdHitPointSetDraw::RemoveEveElementList()
{
    gEve->RemoveElement(fq, fEventManager->EveRecoPoints);
    return;
}

ClassImp(MpdHitPointSetDraw)
