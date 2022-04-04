// -------------------------------------------------------------------------
// -----                     CbmStsHit source file                     -----
// -----                Created 30/08/06  by V. Friese                 -----
// -------------------------------------------------------------------------

#include "CbmStsHit.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmStsHit::CbmStsHit() 
  : CbmHit(),
    fDigiF(0),
    fDigiB(0),
    fPosSX(0),
    fPosSY(0),
    fStatLayer(0),
    fSignalDiv(0.)
{
  //  fDigiF = fDigiB = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsHit::CbmStsHit(Int_t detId, TVector3& pos, TVector3& dpos,  
		     Double_t covXY, Int_t iDigiF, Int_t iDigiB)
  : CbmHit(detId, pos, dpos, covXY, -1),
    fDigiF(iDigiF),
    fDigiB(iDigiB),
    fPosSX(0),
    fPosSY(0),
    fStatLayer(0),
    fSignalDiv(0.)
{
  AddIndex(iDigiF);
  AddIndex(iDigiB);
//   AddLink(FairLink(kStsCluster,iDigiF));
//   AddLink(FairLink(kStsCluster,iDigiB));
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsHit::CbmStsHit(Int_t detId, TVector3& pos, TVector3& dpos,  
                     Double_t covXY, Int_t iDigiF, Int_t iDigiB,
                     Int_t iPosSX, Int_t iPosSY, Int_t iLayer)
  :CbmHit(detId, pos, dpos, covXY, -1),
   fDigiF(iDigiF),
   fDigiB(iDigiB),
   fPosSX(iPosSX),
   fPosSY(iPosSY),
   fStatLayer(iLayer),
   fSignalDiv(0.)
{
  /*
  fDigiF = iDigiF;
  fDigiB = iDigiB;
  fPosSX = iPosSX;
  fPosSY = iPosSY;
  fSignalDiv = 0.;

  fStatLayer = iLayer;
  */
  AddIndex(iDigiF);
  AddIndex(iDigiB); 
//   AddLink(FairLink(kStsCluster,iDigiF));
//   AddLink(FairLink(kStsCluster,iDigiB));
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsHit::CbmStsHit(Int_t detId, TVector3& pos, TVector3& dpos,  
		     Double_t covXY, Int_t iDigiF, Int_t iDigiB, Double_t dSignalDiv)
  : CbmHit(detId, pos, dpos, covXY, -1),
    fDigiF(iDigiF),
    fDigiB(iDigiB),
    fPosSX(0),
    fPosSY(0),
    fStatLayer(0),
    fSignalDiv(dSignalDiv)

{
  /*
  fDigiF = iDigiF;
  fDigiB = iDigiB;
  fPosSX = 0;
  fPosSY = 0;
  fSignalDiv = dSignalDiv;

  fStatLayer = 0;
  */
  AddIndex(iDigiF);
  AddIndex(iDigiB);
//   AddLink(FairLink(kStsCluster,iDigiF));
//   AddLink(FairLink(kStsCluster,iDigiB));
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmStsHit::~CbmStsHit() { }
// -------------------------------------------------------------------------



// -----   Public method GetChannel   --------------------------------------
Int_t CbmStsHit::GetDigi(Int_t iSide) const {
  if (iSide) return fDigiB;
  return fDigiF;
}
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void CbmStsHit::Print(Option_t* opt) const {
  cout.precision(5);
  cout << "StsHit at (" << fX << ", " << fY << ", " << fZ << ") cm, "
       << "Station " << GetStationNr() << ", Sector " << GetSectorNr()
       << ", Digi front " << fDigiF 
       << ", Digi back " << fDigiB << endl;
}
// -------------------------------------------------------------------------

void CbmStsHit::AddIndex(int index) 
{ 
  //AZ - durty trick to suppress warnings "FairMultiLinkedData::InsertHistory Link to wrong Class"

  if (GetPointerToLinks() == NULL) {
    AddLink(FairLink(-1, -1)); 
    SetInsertHistory(kFALSE);
    //cout << " aaaaaaa " << GetPointerToLinks()->GetLinks().size() << endl;
    ResetLinks();
    //cout << " bbbb " << GetPointerToLinks()->GetLinks().size() << endl;
  } 
  AddLink(FairLink(kStsCluster, index));
}

// ------------------------------------------------------------------------- 

ClassImp(CbmStsHit)
