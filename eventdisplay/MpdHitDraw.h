/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *              GNU Lesser General Public Licence (LGPL) version 3,             *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/*
 * MpdHitDraw.h
 *
 *  Created on: Apr 16, 2009
 *      Author: stockman
 *
 *      Simple method to draw points derived from FairHit
 */

#ifndef MpdHitDraw_H_
#define MpdHitDraw_H_

#include "MpdBoxSetDraw.h"             // for MpdBoxSetDraw

#include "Rtypes.h"                     // for MpdHitDraw::Class, etc
#include "TVector3.h"                   // for TVector3

class TObject;

class MpdHitDraw: public MpdBoxSetDraw
{
  public:
    MpdHitDraw();

    /** Standard constructor
      *@param name        Name of task
      *@param iVerbose    Verbosity level
      **/
    MpdHitDraw(const char* name, Int_t iVerbose = 1);

  protected:

    TVector3 GetVector(TObject* obj);

    ClassDef(MpdHitDraw,1);
};

#endif /* MpdHitDraw_H_ */
