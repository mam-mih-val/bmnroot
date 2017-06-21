// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-06-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnParticlePair                                                            //
//                                                                            //
//  A class to consider selected pair of particles                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNPARTICLEPAIR_H
#define BMNPARTICLEPAIR_H 1

#include <TNamed.h>
#include "BmnParticlePairCuts.h"

class BmnParticlePair : public BmnParticlePairCuts {
public:

    /** Default constructor **/
    BmnParticlePair();

    /** Destructor **/
    virtual ~BmnParticlePair();
    
    void SetInvMass(Double_t val) {
        fInvMass = val;
    }

private:
    Double_t fInvMass; // Invariant mass of a considering pair

    ClassDef(BmnParticlePair, 1);

};

#endif