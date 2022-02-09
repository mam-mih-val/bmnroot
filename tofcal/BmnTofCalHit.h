/* 
 * File:   BmnTofCalHit.h
 * Author: segarra
 *
 * Created on Feb 14, 2018, 10:51 AM
 */

/*
TODO: add slewing, make sure that spacing of layers is only thickness of bars 10cm, add T0

BmnTofcalHit->GetDetectorID()  			returns plane of TofCal (0 = Tof, 1-3 = Cal)
BmnTofcalHit->GetRefIndex()			returns bar index in plane (0-19)
BmnTofcalHit->GetX() / GetY() / GetZ()		returns global position of TofCal in lab frame
BmnTofcalHit->GetTimeStamp()			returns time rel to T0 with slewing corr
BmnTofcalHit->GetEnergy()				returns calibrated energy deposit for hit
BmnTofcalHit->GetDx() / GetDy() / GetDz()		returns error in position calculation

*/


#ifndef BMNTOFCALHIT_H
#define BMNTOFCALHIT_H

#include <math.h>
#include <iostream>
#include "BmnHit.h"
#include <TObject.h>
using namespace std;

// class TClonesArray;

class BmnTofCalHit : public BmnHit {
public:

    /** Default constructor **/
    BmnTofCalHit();

    /** Constructor to use **/
    BmnTofCalHit(Int_t plane, Int_t bar, TVector3 posHit, TVector3 dpos, Float_t time, Float_t energy );

    /** Destructor **/
    virtual ~BmnTofCalHit();
    
    Float_t GetEnergy() const{
	return fenergy;
    }

    void SetEnergy(Float_t energy) {
	fenergy = energy;
    }

private:

    Float_t fenergy;
    ClassDef(BmnTofCalHit, 1);
};

#endif /* BMNTOFCALHIT_H */

