// @(#)bmnroot/land:$Id$
// Author: Maria Patsyuk <mpatsyuk@mit.edu> 2018-02-10

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnLANDGeometry                                                            //
//                                                                            //
// A class containing geometry params. of the LAND                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNLANDGEOMETRY_H
#define BMNLANDGEOMETRY_H

#include <iostream>
#include <TNamed.h>
#include <TMath.h>
#include <TVector3.h>

using namespace std;
using namespace TMath;

class BmnLANDGeometry : public TNamed {
public:

    /** Default constructor **/
    BmnLANDGeometry();

    /** Destructor **/
    virtual ~BmnLANDGeometry();
    
    Double_t GetGlobalX() {
        return fGlobalX;
    }
    
    Double_t GetGlobalZ() {
        return fGlobalZ;
    }
    
    Double_t GetGlobalAngle() {
        return fGlobalAngle;
    }
    
private:

    Double_t fGlobalAngle;
    Double_t fGlobalX;
    Double_t fGlobalZ;
    
    Bool_t fDebug;
    
    ClassDef(BmnLANDGeometry, 1);
};

#endif
