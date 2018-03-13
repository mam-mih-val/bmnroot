/*
 * FairMCPointsDraw.h
 *
 *  Created on: Sep. 30, 2009
 *      Author: stockman
 */

#ifndef MPDHITPOINTSETDRAW_H
#define MPDHITPOINTSETDRAW_H

#include "MpdPointSetDraw.h"

#include "TVector3.h"

class MpdHitPointSetDraw : public MpdPointSetDraw
{
  public:
    MpdHitPointSetDraw() {}
    MpdHitPointSetDraw(const char* name, Color_t color, Style_t mstyle, Int_t iVerbose = 0)
        : MpdPointSetDraw(name, color, mstyle, iVerbose) {}
    virtual ~MpdHitPointSetDraw() {}

  protected:
    TVector3 GetVector(TObject* obj);
    void AddEveElementList();
    void RemoveEveElementList();

    ClassDef(MpdHitPointSetDraw,1);
};

#endif /* MPDHITPOINTSETDRAW_H */
