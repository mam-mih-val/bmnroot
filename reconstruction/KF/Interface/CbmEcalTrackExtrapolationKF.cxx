
#include <iostream>

#include "TClonesArray.h"

#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "FairTrackParam.h"
#include "CbmMCTrack.h"
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"
#include "CbmEcalTrackExtrapolationKF.h"

using namespace std;

// -----   Default constructor   -------------------------------------------
CbmEcalTrackExtrapolationKF::CbmEcalTrackExtrapolationKF()
  : CbmEcalTrackExtrapolation(1), fTrackParamArray(NULL), fSTSArray(NULL), iEx(-1)
{
  ;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmEcalTrackExtrapolationKF::CbmEcalTrackExtrapolationKF(Int_t verbose)
  : CbmEcalTrackExtrapolation(verbose), fTrackParamArray(NULL), fSTSArray(NULL), iEx(-1)
{
  ;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmEcalTrackExtrapolationKF::~CbmEcalTrackExtrapolationKF()
{
  ;
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmEcalTrackExtrapolationKF::Init()
{
  CbmEcalTrackExtrapolation::Init();
  //Get and check FairRootManager
  FairRootManager* ioman=FairRootManager::Instance();
  if (!ioman)
    Fatal("CbmEcalTrackExtrapolationKF::Init()", "Can't instantise Rootmanager");

  //Get STS track array
  fSTSArray=(TClonesArray*)ioman->GetObject("StsTrack");
  if (!fSTSArray)
    cout << "-W- CbmEcalTrackExtrapolationKF::Init: No sts track array!" << endl;
}
// -------------------------------------------------------------------------



// -----   Public method DoExtrapolate   ------------------------------------------
Int_t CbmEcalTrackExtrapolationKF::DoExtrapolate(TClonesArray *gTrackArray, TClonesArray *fTrackParamArray_)
{
  if (!fTrackParamArray_)
  {
    cout << "-E- CbmEcalTrackExtrapolationKF::DoExtrapolate: ";
    cout << "TrackParamArray missing! " << endl;
    return -1;
  }

  if (!gTrackArray)
  {
    cout << "-E- CbmEcalTrackExtrapolationKF::DoExtrapolate: ";
    cout << "Track Array missing! " << endl;
    return -1;
  }
  Int_t nTr=0;
  Int_t i=0;
  Int_t n=gTrackArray->GetEntriesFast();
  CbmGlobalTrack* tr;
  CbmKFTrack kfTr;
  FairTrackParam trpar;
  for(;i<n;i++)
  {
    Map()[nTr]=-1111;
    tr=(CbmGlobalTrack*)gTrackArray->At(i);
    if (!tr) continue;
    if (tr->GetStsTrackIndex()<0||tr->GetTrdTrackIndex()<0)
      continue;
    kfTr.SetTrackParam(*(const_cast<FairTrackParam*>(tr->GetParamLast())));
    kfTr.Extrapolate(Str()->GetEcalInf()->GetZPos());
    kfTr.GetTrackParam(trpar);
    Map()[nTr]=i;
    new ((*fTrackParamArray_)[nTr++]) FairTrackParam(trpar);
  }

  return nTr;
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmEcalTrackExtrapolationKF::Finish()
{
  ;
}
//-----------------------------------------------------------------------------------

ClassImp(CbmEcalTrackExtrapolationKF)
