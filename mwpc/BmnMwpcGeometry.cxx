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

BmnMwpcGeometry::BmnMwpcGeometry() :
fDebug(kFALSE) {
    fNChambers = 2;
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
    fDistanceBetweenMWPC = 140.5;

    /*
    Reference frame is related to left side of the magnetic pole (z = 0, zC = 124.5 cm) 
    -----> Z -----> (MWPC -> GEM -> DCH)
                                                    MWPC1                MWPC2
    
    zLeft[0] (fSpaceLeft | | | | | | fSpaceRight) zRight[0]  zLeft[1] (fSpaceLeft | | | | | | fSpaceRight) zRight[1]
                         1 2 3 4 5 6                                              1 2 3 4 5 6
    
     */

    zRight[1] = -(186. + 22.); // http://bmnshift.jinr.ru/wiki/lib/exe/fetch.php?media=geom_bmn_pc_dch_run5.pdf
    zLeft[1] = zRight[1] - fChamberWidth;

    zRight[0] = zLeft[1] - fDistanceBetweenMWPC;
    zLeft[0] = zRight[0] - fChamberWidth;

    for (Int_t iChamber = 0; iChamber < fNChambers; iChamber++)
        for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++)
            zPlanePos[iChamber][iPlane] = zLeft[iChamber] + fSpaceLeft + iPlane * fPlaneStep;

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

TVector3 BmnMwpcGeometry::GetChamberCenter(Int_t chamber) {
    return TVector3(0.0, 0.0, (zRight[chamber] + zLeft[chamber]) * 0.5);
}

BmnMwpcGeometry::~BmnMwpcGeometry() {

}
