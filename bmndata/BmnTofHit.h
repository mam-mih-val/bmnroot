/* 
 * File:   BmnTofHit.h
 * Author: merz
 *
 * Created on October 24, 2014, 10:58 AM
 */

#ifndef BMNTOFHIT_H
#define BMNTOFHIT_H

#include <math.h>
#include <iostream>
#include "BmnHit.h"
#include <TObject.h>

using namespace std;

// class TClonesArray;

class BmnTofHit : public BmnHit {
public:

    /** Default constructor **/
    BmnTofHit();

    /** Constructor to use **/
    BmnTofHit(Int_t detUID, TVector3 posHit, TVector3 posHitErr, Int_t pointIndex);

    /** Destructor **/
    virtual ~BmnTofHit();

private:

    ClassDef(BmnTofHit, 1);

};

#endif /* BMNTOFHIT_H */

