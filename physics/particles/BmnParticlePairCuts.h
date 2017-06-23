// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-06-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnParticlePairCuts                                                        //
//                                                                            //
//  A class to consider selected pair of particles                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNPARTICLEPAIRCUTS_H
#define BMNPARTICLEPAIRCUTS_H 1

#include <TNamed.h>

class BmnParticlePairCuts : public TNamed {
public:

    /** Default constructor **/
    BmnParticlePairCuts();

    /** Destructor **/
    virtual ~BmnParticlePairCuts();

    void SetV0XZ(Double_t val) {
        fV0XZ = val;
    }

    void SetV0YZ(Double_t val) {
        fV0YZ = val;
    }

    void SetVpPart1(Double_t val) {
        fVpPart1 = val;
    }

    void SetVpPart2(Double_t val) {
        fVpPart2 = val;
    }

    void SetV0Part1Part2(Double_t val) {
        fV0Part1Part2 = val;
    }

    void SetV0VpDist(Double_t val) {
        fV0VpDist = val;
    }

    Double_t GetV0Part1Part2() {
        return fV0Part1Part2;
    }
    
    Double_t GetV0VpDist() {
        return fV0VpDist;
    }
    
    Double_t GetVpPart1() {
        return fVpPart1;
    }
    
    Double_t GetVpPart2() {
        return fVpPart2;
    }

private:
    Double_t fV0XZ; // Zv of V0 obtained in XZ-plane
    Double_t fV0YZ; // Zv of V0 obtained in YZ-plane

    Double_t fVpPart1; // spatial distance between Vp and extrap. track of Part1 to Vp
    Double_t fVpPart2; // spatial distance between Vp and extrap. track of Part2 to Vp
    Double_t fV0Part1Part2; // spatial distance between the two particles at V0
    Double_t fV0VpDist; // distance between Vp and V0 along beam axis

    ClassDef(BmnParticlePairCuts, 1);

};

#endif