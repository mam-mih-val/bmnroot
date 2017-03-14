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
#include "FairHit.h"
#include <TObject.h>

using namespace std;

// class TClonesArray;

class BmnTofHit : public FairHit {
public:

    /** Default constructor **/
    BmnTofHit();

    /** Constructor to use **/
    BmnTofHit(Int_t detUID, TVector3 posHit, TVector3 posHitErr, Int_t pointIndex);

    void SetLength(Double_t Length);
    Double_t GetLength();
    
    Bool_t IsUsed() const;
    void SetUsing(Bool_t use);

    /** Destructor **/
    virtual ~BmnTofHit();

private:

    Bool_t fUsing;
    Double_t fLength;

    ClassDef(BmnTofHit, 1);

};

#endif /* BMNTOFHIT_H */

