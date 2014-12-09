//* $Id: 

// -------------------------------------------------------------------------
// -----                      CbmStsCluster source file                -----
// -----                  Created 25/06/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------
#include "CbmStsCluster.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmStsCluster::CbmStsCluster() 
  : FairMultiLinkedData(),
    fDetectorId(0),
    fDigis(),
    fNofDigis(0),
    fChannel(0.),
    fError(0.)
{
  //  fDetectorId = 0;

  // for now i limit the size to 99...
  for ( Int_t itemp = 100 ; itemp > 0 ; )  fDigis[--itemp] = -1;
  //  fDigis = new Int_t[100];
  /*
  fNofDigis = 0;
  fChannel  = 0.;
  fError    = 0.;
  */
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsCluster::CbmStsCluster(Double_t digiSig, Int_t iStation, Int_t iSector, Int_t iSide)
  : FairMultiLinkedData(),
    fDetectorId(0),
    fDigis(),
    fNofDigis(0),
    fChannel(0.),
    fError(0.)
{
  
  // Check range for station, sector and side
  if ( ! ( iStation >= 0 && iStation <= 255 ) ) {
    cout << "-E- CbmStsCluster: Illegal station number " << iStation << endl;
    Fatal("", "Illegal station number");
  }
  if ( ! ( iSector >= 0 && iSector <= 4095 ) ) {
    cout << "-E- CbmStsCluster: Illegal sector number " << iSector << endl;
    Fatal("", "Illegal sector number");
  }
  if ( ! ( iSide >= 0 && iSide <= 1 ) ) {
    cout << "-E- CbmStsCluster: Illegal side number " << iSide <<  endl;
    Fatal("", "Illegal side number");
  }
//   AddIndex(index, digiSig);
  // System id is set by the base class constructor on bits 0-4
  fDetectorId  =         2 << 24 ;  // station nr. on bits  5-12
  fDetectorId |= (iStation << 16);  // station nr. on bits  5-12
  fDetectorId |= (iSector  <<  4);  // sector  nr. on bits 13-27
  fDetectorId |= (iSide    <<  0);  // side        on bit  28
  // Channel number is set by the base class constructor
 
  //  fDigis = new Int_t[19];
  for ( Int_t itemp = 100 ; itemp > 0 ; )  fDigis[--itemp] = -1;
  //  fDigis = new Int_t[100];
  /*
  fNofDigis = 0;
  fChannel  = 0.;
  fError    = 0.;
  */
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmStsCluster::~CbmStsCluster() { }
// -------------------------------------------------------------------------



ClassImp(CbmStsCluster)
