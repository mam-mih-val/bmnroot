/** @file CbmStsSenzor.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 03.05.2013
 **/


#include "TGeoMatrix.h"
#include "TGeoPhysicalNode.h"
#include "CbmStsPoint.h"
#include "CbmStsSenzor.h"
#include "setup/CbmStsAddress.h"
#include "setup/CbmStsSensorPoint.h"
#include "setup/CbmStsSensorType.h"


// -----   Constructor   ---------------------------------------------------
CbmStsSenzor::CbmStsSenzor() 
 : TNamed(),
   fAddress(0),
   fNode(NULL),
   fType(NULL),
   fModule(NULL)
{
}
// -------------------------------------------------------------------------




// -----   Process a CbmStsPoint  ------------------------------------------
void CbmStsSenzor::ProcessPoint(CbmStsPoint* point) const {

  Double_t global[3];
  Double_t local[3];

  // --- Transform entry coordinates into local C.S.
  global[0] = point->GetXIn();
  global[1] = point->GetYIn();
  global[2] = point->GetZIn();
  fNode->GetMatrix()->MasterToLocal(global, local);
  Double_t x1 = local[0];
  Double_t y1 = local[1];

  // --- Transform exit coordinates into local C.S.
  global[0] = point->GetXOut();
  global[1] = point->GetYOut();
  global[2] = point->GetZOut();
  fNode->GetMatrix()->MasterToLocal(global, local);
  Double_t x2 = local[0];
  Double_t y2 = local[1];

  // Create SensorPoint
  CbmStsSensorPoint* sPoint = new CbmStsSensorPoint(x1, y1, x2, y2,
                                                    point->GetEnergyLoss(),
                                                    point->GetTime());

  // --- Call ProcessPoint method from sensor type
  fType->ProcessPoint(sPoint, this);

}
// -------------------------------------------------------------------------










ClassImp(CbmStsSenzor)
