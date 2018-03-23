/*
 * MpdHitDraw.h
 *
 *  Created on: Apr 16, 2009
 *      Author: stockman
 *
 *      Simple method to draw points derived from FairHit
 */

#ifndef MPDHITDRAW_H
#define MPDHITDRAW_H

#include "MpdBoxSetDraw.h"

#include "TVector3.h"

class MpdHitDraw: public MpdBoxSetDraw
{
  public:
    MpdHitDraw() {}
    /** Standard constructor
      *@param name        Name of task
      *@param iVerbose    Verbosity level
     **/
    MpdHitDraw(const char* name, Int_t iVerbose = 1)
        : MpdBoxSetDraw(name, iVerbose) {}

  protected:
    TVector3 GetVector(TObject* obj);

    ClassDef(MpdHitDraw,1);
};

#endif /* MPDHITDRAW_H */
