#include "CbmTofCell.h"

#include "CbmTofPoint.h"
#include "CbmTofDetectorId.h"

#include "FairLogger.h"

#include "TGeoManager.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTofCell::CbmTofCell() 
  : TNamed(),
    fDetectorId(0),
    fX(-666.),
    fY(-666.),
    fZ(-666.),
    fSizex(-666.),
    fSizey(-666.)
{
}

CbmTofCell::CbmTofCell(Int_t detId, Double_t x, Double_t y, Double_t z,
    Double_t sizex, Double_t sizey)
  : TNamed(),
    fDetectorId(detId),
    fX(x),
    fY(y),
    fZ(z),
    fSizex(sizex),
    fSizey(sizey)
{
}
// -------------------------------------------------------------------------
CbmTofCell::~CbmTofCell() 
{
}
/*
void CbmTofCell::GetPadInfo(CbmTrdPoint *trdPoint, Int_t &Col, 
			      Int_t &Row, Int_t &Sector)
{

  // Calculate point in the middle of the detector. This is
  // for safety reasons, because the point at exit is slightly
  // outside of the active volume. If one does not use a point
  // in the detector one will get a wrong volume from the
  // geomanager. Navigate to the correct volume to get all
  // necessary information about this volume

  Double_t x_mean = (trdPoint->GetXIn()+trdPoint->GetXOut())/2.;
  Double_t y_mean = (trdPoint->GetYIn()+trdPoint->GetYOut())/2.;
  Double_t z_mean = (trdPoint->GetZIn()+trdPoint->GetZOut())/2.;
  gGeoManager->FindNode(x_mean, y_mean, z_mean);

  // Get the local point in local MC coordinates from
  // the geomanager. This coordinate system is rotated
  // if the chamber is rotated. This is corrected in 
  // GetModuleInformation to have a
  // the same local coordinate system in all the chambers
  const Double_t *global_point = gGeoManager->GetCurrentPoint();
  Double_t local_point[3];  // global_point[3];
  gGeoManager->MasterToLocal(global_point, local_point);

  Int_t ModuleID = trdPoint->GetDetectorID();

  GetModuleInformation(ModuleID, local_point, Col, Row, Sector);

}
*/

/*
void CbmTofCell::GetModuleInformation(Int_t VolumeID, Double_t *local_point, Int_t &Col, Int_t &Row, Int_t &sector)
{

  // safety check. Up to now allways correct, so could be removed.
  if (fDetectorId != VolumeID ){
    cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
  }

  // calculate in which sector the point is
  sector = GetSector(local_point);

  Double_t posX, posY;
  Double_t fpadsizex = GetPadSizex(sector);
  Double_t fpadsizey = GetPadSizey(sector);

  TransformToLocalSector(local_point, sector, posX, posY);

  Col = 1+(Int_t)(posX/fPadSizex.At(sector));
  Row = 1+(Int_t)(posY/fPadSizey.At(sector));

}


  // ---- CalculateHitPosition ------------------------------------------
void CbmTofCell::GetPosition(const Int_t Col, const Int_t Row, 
			       const Int_t VolumeId, const Int_t sector,
			       TVector3 &posHit, TVector3 &padSize) {

  // calculate position in global coordinates from digi 
  // information(Sector, Col, Row).  
  // Returns two TVector3. One with the position and one 
  // with the padsize of the fired pad

  if (fDetectorId != VolumeId ){
    cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
  }
    
  Double_t local_point[3];
  Double_t padsizex = fPadSizex.At(sector);
  Double_t padsizey = fPadSizey.At(sector);

  // calculate position in sector coordinate system with the
  // origin in the lower right corner
  local_point[0] = (((Float_t)Col-0.5) * padsizex);
  local_point[1] = (((Float_t)Row-0.5) * padsizey); 

  // calculate position in module coordinate system
  // with origin in the lower right corner of the module
  local_point[0]+=fSectorBeginX.GetAt(sector);
  local_point[1]+=fSectorBeginY.GetAt(sector);

  // calculte position in the module coordinate system 
  // with origin in the middle of the module
  local_point[0]-=fSizex;
  local_point[1]-=fSizey;
  local_point[2]=fSizez; 
                     
  // Navigate to the correct module. (fX,fY,fZ)
  gGeoManager->FindNode(fX, fY, fZ);

  // Get the local point in local MC coordinates from
  // the geomanager. This coordinate system is rotated
  // if the chamber is rotated. This is corrected in 
  // GetModuleInformation to have a
  // the same local coordinate system in all the chambers
  Double_t global_point[3];  // global_point[3];
  gGeoManager->LocalToMaster(local_point, global_point);
  

  // calculate the position in the global coordinate system
  // with the origin in target
  Float_t posX=global_point[0];
  Float_t posY=global_point[1];
  Float_t posZ=global_point[2];

  posHit.SetXYZ(posX, posY, posZ);
  padSize.SetXYZ(padsizex,padsizey, 0.);
  
}
*/

void CbmTofCell::Print()
{
  LOG(INFO)<<"ID, X, Y, Z, sizex, sizey: "<< fDetectorId << ", "<< fX << ", "<< fY
        << ", "<< fZ << ", "<< fSizex << ", "<< fSizey << FairLogger::endl;


}

  // -------------------------------------------------------------------------
ClassImp(CbmTofCell)
