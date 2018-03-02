/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *              GNU Lesser General Public Licence (LGPL) version 3,             *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/*
 * MpdHitDraw.cpp
 *
 *  Created on: Apr 16, 2009
 *      Author: stockman
 */

#include "MpdHitDraw.h"

#include "FairHit.h"                    // for FairHit

class TObject;

MpdHitDraw::MpdHitDraw()
{
}

MpdHitDraw::MpdHitDraw(const char* name, Int_t iVerbose):MpdBoxSetDraw(name, iVerbose)
{
}

TVector3 MpdHitDraw::GetVector(TObject* obj)
{
  FairHit* hit =static_cast<FairHit*>(obj);
  return TVector3(hit->GetX(), hit->GetY(), hit->GetZ());
}

ClassImp(MpdHitDraw)
