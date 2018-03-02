/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *              GNU Lesser General Public Licence (LGPL) version 3,             *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----              MpdBoxSet header file                       -----
// -----          Created 26/03/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------


/** MpdBoxSet
 * @author T. Stockmanns
 * @since 26.3.2009
 *   Base class to display 3D Points in Eve as a BoxSet
 *   One has to overwrite the method GetVector which takes a TObject and writes out a TVector3 which is then taken as an input
 *   to place the points.
 *   If one wants to change the color of the points one has to overwrite the method GetValue. This method takes a TObject and
 *   an integer and translates this into an integer as input for the EveBoxSet method DigitValue
 **
 **/

#ifndef MpdBoxSet_H
#define MpdBoxSet_H

#include "TEveBoxSet.h"                 // for TEveBoxSet

#include "MpdBoxSetDraw.h"             // for MpdBoxSetDraw

#include "Rtypes.h"                     // for Double_t, MpdBoxSet::Class, etc


class MpdBoxSet : public TEveBoxSet
{

  public:

    /** Standard constructor
       **/
    MpdBoxSet(MpdBoxSetDraw* drawer, const char* name = "MpdBoxSet", const char* t = "");

    void SetTimeWindowPlus(Double_t time) {fDraw->SetTimeWindowPlus(time);}
    void SetTimeWindowMinus(Double_t time) {fDraw->SetTimeWindowMinus(time);}

    Double_t GetTimeWindowPlus() {return fDraw->GetTimeWindowPlus();}
    Double_t GetTimeWindowMinus() {return fDraw->GetTimeWindowMinus();}


    /** Destructor **/
    virtual ~MpdBoxSet() {};



  protected:

  private:
    MpdBoxSetDraw* fDraw;

    MpdBoxSet(const MpdBoxSet&);
    MpdBoxSet operator=(const MpdBoxSet&);

    ClassDef(MpdBoxSet,1);

};


#endif
