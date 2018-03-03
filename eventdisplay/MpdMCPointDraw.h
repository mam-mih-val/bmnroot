/*
 * FairMCPointsDraw.h
 *
 *  Created on: Apr 17, 2009
 *      Author: stockman
 */

#ifndef MpdMCPointDraw_H
#define MpdMCPointDraw_H

#include "MpdPointSetDraw.h"

#include "TVector3.h"


class MpdMCPointDraw : public MpdPointSetDraw
{
  public:
    MpdMCPointDraw();
    MpdMCPointDraw(const char* name, Color_t color, Style_t mstyle, Int_t iVerbose = 0)
        : MpdPointSetDraw(name, color, mstyle, iVerbose) {}
    virtual ~MpdMCPointDraw();

  protected:
    TVector3 GetVector(TObject* obj);

    void AddEveElementList();
    void RemoveEveElementList();

    ClassDef(MpdMCPointDraw,1);
};

#endif /* MpdMCPointDraw_H */
