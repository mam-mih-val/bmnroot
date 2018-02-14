/* 
 * File:   BmnLANDHit.h
 * Author: segarra
 *
 * Created on Feb 14, 2018, 10:51 AM
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
    BmnLANDHit(Int_t plane, Int_t bar, TVector3 posHit, TVector3 dpos, Float_t time, Float_t energy );

    /** Destructor **/
    virtual ~BmnLANDHit();
    
    Float_t GetEnergy() const{
	return fenergy;
    }

    void SetEnergy(Float_t energy) {
	fenergy = energy;
    }

private:

    Float_t fenergy;
    ClassDef(BmnLANDHit, 1);
};

#endif /* BMNLANDHIT_H */

