// ------------------------------------------------------------------
// -----                  CbmGlobalTrackFitterKF                -----
// -----              Created 07/03/2006 by D.Kresan            -----
// ------------------------------------------------------------------
#ifndef CBMGLOBALTRACKFITTERKF
#define CBMGLOBALTRACKFITTERKF

#include "CbmGlobalTrackFitter.h"

class TClonesArray;
class CbmKFTrack;
class CbmVertex;


class CbmGlobalTrackFitterKF : public CbmGlobalTrackFitter {

private:
    TClonesArray *fArrayStsHit;       // Array of STS hits
    TClonesArray *fArrayTrdHit;       // Array of TRD hits
    TClonesArray *fArrayTofHit;       // Array of TOF hits
    TClonesArray *fArrayStsTrack;     // Array of STS tracks
    TClonesArray *fArrayTrdTrack;     // Array of TRD tracks
    CbmVertex    *fPrimVertex;        // Primary vertex
    CbmKFTrack   *fKfTrack;           // KF track

public:
    CbmGlobalTrackFitterKF();
    virtual ~CbmGlobalTrackFitterKF();

    void Init();
    void DoFit(CbmGlobalTrack* glbTrack);

    ClassDef(CbmGlobalTrackFitterKF, 1);

 private:
  CbmGlobalTrackFitterKF(const CbmGlobalTrackFitterKF&);
  void operator=(const CbmGlobalTrackFitterKF&);
};


#endif

