/*
 * FairMCPointsDraw.h
 *
 *  Created on: Sep. 30, 2009
 *      Author: stockman
 */

#ifndef FAIRHITPOINTSETDRAW_H
#define FAIRHITPOINTSETDRAW_H

#include "FairPointSetDraw.h"
#include "TVector3.h"

class FairHitPointSetDraw : public FairPointSetDraw
{
  public:
    FairHitPointSetDraw();
    FairHitPointSetDraw(const char* name, Color_t color, Style_t mstyle, Int_t iVerbose = 1)
        : FairPointSetDraw(name, color, mstyle, iVerbose) {}
    virtual ~FairHitPointSetDraw();

  protected:
    TVector3 GetVector(TObject* obj);
    void AddEveElementList();
    void RemoveEveElementList();

    ClassDef(FairHitPointSetDraw,1);
};

#endif /* FAIRHITPOINTSETDRAW_H */
