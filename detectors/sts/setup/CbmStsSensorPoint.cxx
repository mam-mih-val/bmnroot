/** @file CbmStsSensorPoint.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 08.05.2013
 **/


#include "CbmStsSensorPoint.h"


// --- Default constructor   -----------------------------------------------
CbmStsSensorPoint::CbmStsSensorPoint() : fX1(0.), fY1(0.),
                                         fX2(0.), fY2(0.),
                                         fELoss(0.), fTime(0.) { }
// -------------------------------------------------------------------------



// --- Destructor   --------------------------------------------------------
CbmStsSensorPoint::~CbmStsSensorPoint() { }
// -------------------------------------------------------------------------



// --- Standard constructor   ----------------------------------------------
CbmStsSensorPoint::CbmStsSensorPoint(Double_t x1, Double_t y1,
                                     Double_t x2, Double_t y2,
                                     Double_t eLoss, Double_t time)
  : fX1(x1), fY1(y1), fX2(x2), fY2(y2), fELoss(eLoss), fTime(time) { }
// -------------------------------------------------------------------------


ClassImp(CbmStsSensorPoint)
