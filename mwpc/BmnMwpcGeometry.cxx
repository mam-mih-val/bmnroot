// @(#)bmnroot/mwpc:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-02-10

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcGeometry                                                            //
//                                                                            //
// A class containing geometry params. of                                     //
// the Multi Wire Prop. Chambers of the BM@N experiment                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include "BmnMwpcGeometry.h"

BmnMwpcGeometry::BmnMwpcGeometry() {

    fNChambers = 2;
    fNPlanes = 6;
    fTimeBin = 8; 
    fNWires = 96; 
    fAngleStep = 60 * DegToRad();
    fWireStep = 0.25;
    fPlaneStep = 1.0015;// ?
    
    fPlaneHeight = 43.3; 
    fPlaneWidth = fNWires * fWireStep; 
    
    zPlanePos[0][0];
    zPlanePos[0][1];
    zPlanePos[0][2];
    zPlanePos[0][3];
    zPlanePos[0][4];
    zPlanePos[0][5];
    
    zPlanePos[1][0];
    zPlanePos[1][1];
    zPlanePos[1][2];
    zPlanePos[1][3];
    zPlanePos[1][4];
    zPlanePos[1][5];
   
    
    
    
    
}

BmnMwpcGeometry::~BmnMwpcGeometry() {

}