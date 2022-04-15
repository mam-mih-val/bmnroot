/** @file BmnSsdSensorPoint.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 20.12.2018
 **/


#include "BmnSsdSensorPoint.h"
#include <sstream>

using std::string;

// --- Default constructor   -----------------------------------------------
BmnSsdSensorPoint::BmnSsdSensorPoint() : fX1(0.), fY1(0.), fZ1(0.),
                                         fX2(0.), fY2(0.), fZ2(0.),
                                         fP(0.), fELoss(0.), fTime(0.),
                                         fBx(0.), fBy(0.), fBz(0.),
                                         fPid(0) { }
// -------------------------------------------------------------------------



// --- Destructor   --------------------------------------------------------
BmnSsdSensorPoint::~BmnSsdSensorPoint() { }
// -------------------------------------------------------------------------



// --- Standard constructor   ----------------------------------------------
BmnSsdSensorPoint::BmnSsdSensorPoint(Double_t x1, Double_t y1, Double_t z1,
                                     Double_t x2, Double_t y2, Double_t z2,
                                     Double_t p, Double_t eLoss, Double_t time,
                                     Double_t bx, Double_t by, Double_t bz,
                                     Int_t pid)
  : fX1(x1), fY1(y1), fZ1(z1), fX2(x2), fY2(y2), fZ2(z2),
    fP(p), fELoss(eLoss), fTime(time),
    fBx(bx), fBy(by), fBz(bz), fPid(pid) { }
// -------------------------------------------------------------------------



// -----   String output   -------------------------------------------------
string BmnSsdSensorPoint::ToString() const {
	std::stringstream ss;
	ss << "PID: " << fPid << ", p = " << fP << ", eLoss = " << fELoss << ", "
		 << "in : (" << fX1 << ", " << fY1 << ", " << fZ1 << "), "
		 << "out: (" << fX2 << ", " << fY2 << ", " << fZ2 << "), "
		 << "at t = " << fTime << ", field " << fBy;
	return ss.str();
}
// -------------------------------------------------------------------------


ClassImp(BmnSsdSensorPoint)
