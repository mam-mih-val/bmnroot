#ifndef CBM_L1_STTTRACK_h
#define CBM_L1_STTTRACK_h

#include "CbmL1SttHit.h"

#include "CbmKFTrackInterface.h"

#include <vector>

class CbmStsTrack;
class CbmMuchTrack;

class CbmL1SttTrack : public CbmKFTrackInterface
{
 public:

  CbmL1SttTrack(){ }
  ~CbmL1SttTrack(){ }

  double *GetTrack(){ return T; }
  double *GetCovMatrix(){ return C; }
  double &GetRefChi2(){ return chi2; }
  int    &GetRefNDF(){ return NDF; }
  double GetMass(){ return 0.1057; }
  bool   IsElectron(){ return 0; }
  int    GetNOfHits(){ return vHits.size(); }
  CbmKFHit *GetHit( int i ){ return vHits[i]; }

  void SetStsTrack( CbmStsTrack* track);
  void SetMuchTrack( CbmMuchTrack* track);

  double T[6], C[15], chi2 ;
  int NDF ;
  std::vector<CbmL1SttHit* > vHits;
  int NHits, NMissed, NMissedStations;
  bool ok;
  bool stopped;
  int StsID;

  static bool Compare( const CbmL1SttTrack *p1, const CbmL1SttTrack *p2 ){
    return (p1->NHits>p2->NHits) || (p1->NHits==p2->NHits)&&(p1->chi2 <p2->chi2) ;
  }
  
 public:
  ClassDef(CbmL1SttTrack, 1);

};

#endif
