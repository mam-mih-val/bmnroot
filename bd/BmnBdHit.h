// -------------------------------------------------------------------------
// -----                      BmnBdHit header file                     -----
// -----                  Created 12/07/18  by A. Zinchenko            -----
// -------------------------------------------------------------------------

/** BmnBdHit.h
 *@author A.Zinchenko <Alexander.Zinchenko@jinr.ru>
 **
 ** The Barrel Detector (BD) hit  
 **/

#ifndef BMNBDHIT_H
#define BMNBDHIT_H 1

#include "FairHit.h"
#include <TObject.h>

#include <math.h>
#include <iostream>

class BmnBdHit : public FairHit 
{

 public:

    /** Default constructor **/
    BmnBdHit();

    /** Constructor to use **/
    BmnBdHit(Int_t detUID, TVector3 posHit, TVector3 posHitErr, Int_t pointIndx);

    Int_t GetStripNumber() const { return fStrip; } // strip number
    Int_t GetStripPoints() const { return GetNLinks(); } // number of points per strip

    /** Destructor **/
    virtual ~BmnBdHit();

private:
    Int_t fStrip;

    ClassDef(BmnBdHit, 1);

};

#endif
