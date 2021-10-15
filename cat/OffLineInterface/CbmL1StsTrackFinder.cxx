/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  CbmL1StsTrackFinder source file
 *
 *====================================================================
 */
#include "CbmL1StsTrackFinder.h"

#include "L1Algo/L1Algo.h"

#include "FairHit.h"
#include "CbmStsHit.h"
#include "CbmMvdHit.h"
#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "CbmStsTrack.h"
#include "CbmKFMath.h"

#include "TClonesArray.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

ClassImp(CbmL1StsTrackFinder)

// -----   Default constructor   -------------------------------------------
  CbmL1StsTrackFinder::CbmL1StsTrackFinder() {
  fName = "STS Track Finder L1";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmL1StsTrackFinder::~CbmL1StsTrackFinder() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmL1StsTrackFinder::Init() {
}
// -------------------------------------------------------------------------



// -----   Public method DoFind   ------------------------------------------
Int_t CbmL1StsTrackFinder::DoFind() {
  /*
    if ( !mHitArray || !hHitArray || !sHitArray ) {
    cout << "-E- CbmL1StsTrackFinder::DoFind: "
    << "Hit arrays missing! " << mHitArray << " " <<hHitArray<<" "<< sHitArray << endl;
    return -1;
    }
  */
  if ( !fTracks ) {
    cout << "-E- CbmL1StsTrackFinder::DoFind: "
	 << "Track array missing! " << endl;
    return -1;
  }

  int ntracks = fTracks->GetEntries();

  CbmL1 *L1 = CbmL1::Instance();
  if( !L1 ) return 0;

  L1->Reconstruct();

  for (vector<CbmL1Track>::iterator it = L1->vRTracks.begin(); it != L1->vRTracks.end(); ++it)
    {
      CbmL1Track &T = *it;
      new((*fTracks)[ntracks]) CbmStsTrack();
      CbmStsTrack *t = L1_DYNAMIC_CAST<CbmStsTrack*>( fTracks->At(ntracks++) );
      t->SetFlag(0);
      FairTrackParam* fpar = t->GetParamFirst(), * lpar = t->GetParamLast();      
      CbmKFMath::CopyTC2TrackParam( fpar, T.T, T.C );
      CbmKFMath::CopyTC2TrackParam( lpar, T.TLast, T.CLast );
      t->SetChi2(T.chi2);
      t->SetNDF(T.NDF);
      t->SetPidHypo( T.T[4]>=0 ?211 :-211 );
      
      for (vector<int>::iterator ih = it->StsHits.begin(); ih != it->StsHits.end(); ++ih)
	{
	  CbmL1HitStore &h = L1->vHitStore[*ih];
	  double zref = L1->algo->vStations[h.iStation].z[0];
	  if( h.ExtIndex<0 ){
	    CbmMvdHit tmp;
	    tmp.SetZ(zref);
	    t->AddMvdHit( -h.ExtIndex-1, &tmp );
	  }else{
	    CbmStsHit tmp;
	    tmp.SetZ(zref);
	    t->AddStsHit( h.ExtIndex , &tmp );
	  }
	}
    }

  return ntracks;

}
// -------------------------------------------------------------------------



    

  
