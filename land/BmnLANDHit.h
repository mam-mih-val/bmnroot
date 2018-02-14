/* 
 * File:   BmnLANDHit.h
 * Author: merz
 *
 * Created on October 24, 2014, 10:58 AM
 */

#ifndef BMNLANDHIT_H
#define BMNLANDHIT_H

#include <math.h>
#include <iostream>
#include "BmnHit.h"
#include <TObject.h>
using namespace std;

// class TClonesArray;

class BmnLANDHit : public BmnHit {
public:

    /** Default constructor **/
    BmnLANDHit();

    /** Constructor to use **/
    BmnLANDHit(Int_t plane, Int_t bar, TVector3 posHit, Float_t time, Float_t energy );

    /** Destructor **/
    virtual ~BmnLANDHit();
    Float_t GetEnergy(){return fenergy;};
    SetEnergy(float energy){fenergy=energy;};

private:

    ClassDef(BmnLANDHit, 1);
    Float_t fenergy;
};

#endif /* BMNTOFHIT_H */

