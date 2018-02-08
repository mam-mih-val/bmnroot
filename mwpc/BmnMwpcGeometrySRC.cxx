// @(#)bmnroot/mwpc:$Id$
// Author: Maria Patsyuk <mpatsyuk@mit.ru> 2018-02-10

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcGeometrySRC                                                         //
//                                                                            //
// A class containing geometry params. of                                     //
// the Multi Wire Prop. Chambers of the SRC at BM@N experiment                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include "BmnMwpcGeometrySRC.h"

BmnMwpcGeometrySRC::BmnMwpcGeometrySRC() :
fDebug(kTRUE) {
    fNChambers = 4;
    fNPlanes = 6;
    fTimeBin = 8;
    fNWires = 96;
    fAngleStep = 60 * DegToRad();
    fWireStep = 0.25;
    fPlaneStep = 1.0015; // According to schemes given by M. Rumyantsev

    fPlaneHeight = 43.3;
    fPlaneWidth = fNWires * fWireStep;

    fSpaceLeft = 2.1338; // According to schemes given by M. Rumyantsev
    fSpaceRight = 2.1328; // According to schemes given by M. Rumyantsev

    fChamberWidth = fSpaceLeft + fSpaceLeft + (fNPlanes - 1) * fPlaneStep;
    //    fDistanceBetweenMWPC = 140.5;
    fGlobalZdiff = -648.4; // location of SRC Z=0 relative to BM@N Z=0
    
    /*
    Reference frame is related to left side of the magnetic pole (z = 0, zC = 124.5 cm) 
    -----> Z -----> (MWPC -> GEM -> DCH)
                                                    MWPC1                MWPC2
    
    zLeft[0] (fSpaceLeft | | | | | | fSpaceRight) zRight[0]  zLeft[1] (fSpaceLeft | | | | | | fSpaceRight) zRight[1]
                         1 2 3 4 5 6                                              1 2 3 4 5 6
    
     */

    zLeft[3] = fGlobalZdiff + 418.6; //zRight[1] - fChamberWidth;
    zRight[3] = zLeft[3] + fChamberWidth;

    zLeft[2] = fGlobalZdiff + 278.4; //zRight[0] - fChamberWidth;
    zRight[2] = zLeft[2] + fChamberWidth; //zLeft[1] - fDistanceBetweenMWPC;

    zLeft[1] = fGlobalZdiff - 117.2;
    zRight[1] = zLeft[1] + fChamberWidth;

    zLeft[0] = fGlobalZdiff - 218.6;
    zRight[0] = zLeft[0] + fChamberWidth;
    
    for (Int_t iChamber = 0; iChamber < fNChambers; iChamber++)
        for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++)
            zPlanePos[iChamber][iPlane] = zLeft[iChamber] + fSpaceLeft + iPlane * fPlaneStep;


    cout<<"it was: zRight = "<< -(186.+22.)<<", zRight1 = "<<-(186.+22.)-fChamberWidth-140.5<<endl;
    // Check built geometry
    if (fDebug)
        for (Int_t iChamber = 0; iChamber < fNChambers; iChamber++) {
            cout << "MWPC" << iChamber + 1 << endl;
            cout << "Zleft = " << zLeft[iChamber] << " Zright = " << zRight[iChamber] << endl;
            for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++)
                cout << "zPlanePos " << iPlane + 1 << " " << zPlanePos[iChamber][iPlane] << endl;
            cout << endl;
        }
}

TVector3 BmnMwpcGeometrySRC::GetChamberCenter(Int_t chamber) {
    return TVector3(0.0, 0.0, (zRight[chamber] + zLeft[chamber]) * 0.5);
}

BmnMwpcGeometrySRC::~BmnMwpcGeometrySRC() {

}
