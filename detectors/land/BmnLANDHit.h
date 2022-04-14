/* 
 * File:   BmnLANDHit.h
 * Author: segarra
 *
 * Created on Feb 14, 2018, 10:51 AM
 */

/*
TODO: add slewing, make sure that spacing of layers is only thickness of bars 10cm, add T0

BmnLandHit->GetDetectorID()  			returns plane of LAND (5 = veto, 0-4 = LAND)
BmnLandHit->GetRefIndex()			returns bar index in plane (0-20)
BmnLandHit->GetX() / GetY() / GetZ()		returns global position of LAND in lab frame
BmnLandHit->GetTimeStamp()			returns time rel to T0 with slewing corr
BmnLandHit->GetEnergy()				returns calibrated energy deposit for hit
BmnLandHit->GetDx() / GetDy() / GetDz()		returns error in position calculation

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

