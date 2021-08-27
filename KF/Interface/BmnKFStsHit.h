// -------------------------------------------------------------------------
// -----                     BmnKFStsHit header file                   -----
// -----          Created 27/03/21  by A.Zinchenko, D.Zinchenko        -----
// -----                     (from CbmKFStsHit.h)                      -----
// -------------------------------------------------------------------------

#ifndef BmnKFStsHit_H
#define BmnKFStsHit_H 1 

#include "CbmKFHit.h"
//#include "CbmKFPixelMeasurement.h"
#include "CbmKFUMeasurement.h"
#include "CbmKFMaterial.h"

class CbmStsHit;
class CbmMvdHit;

class BmnKFStsHit : public CbmKFHit { 

 public:

  BmnKFStsHit():FitPoint(),tube(0){};
  ~BmnKFStsHit(){};

  //CbmKFPixelMeasurement FitPoint;
  CbmKFUMeasurement FitPoint[2];
  CbmKFTube *tube;

  void Create( CbmStsHit *h );
  //void Create( CbmMvdHit *h );
  void Create( CbmMvdHit *h ) { printf(" !!!*** BmnKFStsHit::Create - Not implemented for CbmMvdHit \n"); exit(0); }

  Int_t Filter( CbmKFTrackInterface &track, Bool_t downstream, Double_t &QP0 );

  /*
  static void FilterPDAF( CbmKFTrackInterface &track, 
			  std::vector<CbmKFStsHit*> &vpHits,
			  Bool_t downstream, Double_t *QP0,
			  double gateX, double gateY, int &best_hit_idx );
  */
  
  const BmnKFStsHit& operator=(const BmnKFStsHit& a) {
    tube = a.tube;
    for (int j = 0; j < 2; ++j) FitPoint[j] = a.FitPoint[j];
    return *this;
  };

  BmnKFStsHit(const BmnKFStsHit& a):
    tube(a.tube)    
      { for (int j = 0; j < 2; ++j) FitPoint[j] = a.FitPoint[j]; }

  
  ClassDef(BmnKFStsHit, 1);
};

#endif
