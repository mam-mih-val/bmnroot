/*
 *====================================================================
 *
 *  CBM KF Track Quality
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Fit performance 
 *
 *====================================================================
 */
#include "CbmKFTrErrMCPoints.h"
#include "CbmKF.h"
#include "CbmMCTrack.h"
#include <algorithm>

ClassImp(CbmKFTrErrMCPoints)

CbmKFTrErrMCPoints::CbmKFTrErrMCPoints():
  StsArray(),
  MvdArray(),
  StsHitsArray(),
  MvdHitsArray()
{
}

int CbmKFTrErrMCPoints::GetNConsMCStations()
{
  if((GetNMvdPoints()+GetNStsPoints()) < 1 ) return 0;
  // TODO get station number of the point using methods of the point class!
  float zStation[8] = {30.,40.,50.,60.,70.,80.,90.,100.};
  vector<int> iStations;
  for(int iMvd=0; iMvd<GetNMvdPoints(); ++iMvd)
  {
    iStations.push_back( GetMvdPoint(iMvd)->GetStationNr() - 1 );
//    std::cout << GetMvdPoint(iMvd)->GetStationNr() << " " << GetMvdPoint(iMvd)->GetZ() << std::endl;
  }
  for(int iSts=0; iSts<GetNStsPoints(); ++iSts)
  {
    int stNumber = -1;
    for(int iSt=0; iSt<8; iSt++)
      if( TMath::Abs(zStation[iSt] - GetStsPoint(iSts)->GetZ()) < 2.5) stNumber = iSt;
    if(stNumber >=0)
      iStations.push_back( stNumber + CbmKF::Instance()->GetNMvdStations());
  }

  std::sort(iStations.begin(),iStations.end());

  int nMaxConsStations = 1;
  int nConsStations = 1;  
  int iPrevSt = iStations[0];
  for(unsigned int iP=1; iP<iStations.size(); iP++)
  {
    if((iStations[iP] - iPrevSt) == 1) 
    {
      nConsStations++;
      iPrevSt = iStations[iP];
    }
    else if( (iStations[iP] - iPrevSt)>1 )
    {
      if(nConsStations > nMaxConsStations)
        nMaxConsStations = nConsStations;
      nConsStations = 1;
      iPrevSt = iStations[iP];
    }
  }
  if(nConsStations > nMaxConsStations) nMaxConsStations = nConsStations;

  return nMaxConsStations;
}

int CbmKFTrErrMCPoints::GetNConsHitStations()
{
  if((GetNMvdHits()+GetNStsHits()) < 1 ) return 0;
  // TODO get station number of the point using methods of the point class!
  vector<int> iStations;
  for(int iMvd=0; iMvd<GetNMvdHits(); ++iMvd)
  {
    iStations.push_back( GetMvdHit(iMvd)->GetStationNr() - 1 );
//    std::cout << GetMvdHit(iMvd)->GetStationNr() << " " << GetMvdHit(iMvd)->GetZ() << std::endl;
  }
  for(int iSts=0; iSts<GetNStsHits(); ++iSts)
    iStations.push_back( GetStsHit(iSts)->GetStationNr() - 1 + CbmKF::Instance()->GetNMvdStations());

  std::sort(iStations.begin(),iStations.end());

  int nMaxConsStations = 1;
  int nConsStations = 1;  
  int iPrevSt = iStations[0];
  for(unsigned int iP=1; iP<iStations.size(); iP++)
  {
    if((iStations[iP] - iPrevSt) == 1) 
    {
      nConsStations++;
      iPrevSt = iStations[iP];
    }
    else if( (iStations[iP] - iPrevSt)>1 )
    {
      if(nConsStations > nMaxConsStations)
        nMaxConsStations = nConsStations;
      nConsStations = 1;
      iPrevSt = iStations[iP];
    }
  }
  if(nConsStations > nMaxConsStations) nMaxConsStations = nConsStations;

  return nMaxConsStations;
}

int CbmKFTrErrMCPoints::GetNHitStations()
{
  if((GetNMvdHits()+GetNStsHits()) < 1 ) return 0;
  // TODO get station number of the point using methods of the point class!
  vector<int> iStations;
  for(int iMvd=0; iMvd<GetNMvdHits(); ++iMvd)
  {
    iStations.push_back( GetMvdHit(iMvd)->GetStationNr() - 1 );
//    std::cout << GetMvdHit(iMvd)->GetStationNr() << " " << GetMvdHit(iMvd)->GetZ() << std::endl;
  }
  for(int iSts=0; iSts<GetNStsHits(); ++iSts)
    iStations.push_back( GetStsHit(iSts)->GetStationNr() - 1 + CbmKF::Instance()->GetNMvdStations());

  std::sort(iStations.begin(),iStations.end());

  int nStations = 1;  
  int iPrevSt = iStations[0];
  for(unsigned int iP=1; iP<iStations.size(); iP++)
  {
    if((iStations[iP] - iPrevSt) >= 1) 
    {
      nStations++;
      iPrevSt = iStations[iP];
    }
  }
    
  return nStations; 
}

int CbmKFTrErrMCPoints::GetNMaxMCPointsOnStation()
{
  if((GetNMvdPoints()+GetNStsPoints()) < 1 ) return 0;
  // TODO get station number of the point using methods of the point class!
  float zStation[8] = {30.,40.,50.,60.,70.,80.,90.,100.};
  vector<int> iStations;
  for(int iMvd=0; iMvd<GetNMvdPoints(); ++iMvd)
  {
    iStations.push_back( GetMvdPoint(iMvd)->GetStationNr() - 1 );
//    std::cout << GetMvdPoint(iMvd)->GetStationNr() << " " << GetMvdPoint(iMvd)->GetZ() << std::endl;
  }
  for(int iSts=0; iSts<GetNStsPoints(); ++iSts)
  {
    int stNumber = -1;
    for(int iSt=0; iSt<8; iSt++)
      if( TMath::Abs(zStation[iSt] - GetStsPoint(iSts)->GetZ()) < 2.5) stNumber = iSt;
    if(stNumber >=0)
      iStations.push_back( stNumber + CbmKF::Instance()->GetNMvdStations());
  }

  std::sort(iStations.begin(),iStations.end());

  int nMaxMCPointsOnStation = 1;
  int nMCPointsOnStation = 1;  
  int iPrevSt = iStations[0];
  for(unsigned int iP=1; iP<iStations.size(); iP++)
  {
    if((iStations[iP] - iPrevSt) == 0) 
    {
      nMCPointsOnStation++;
      iPrevSt = iStations[iP];
    }
    else
    {
      if(nMCPointsOnStation > nMaxMCPointsOnStation)
        nMaxMCPointsOnStation = nMCPointsOnStation;
      nMCPointsOnStation = 1;
      iPrevSt = iStations[iP];
    }
  }

  return nMaxMCPointsOnStation;
}

Bool_t CbmKFTrErrMCPoints::IsReconstructable(CbmMCTrack* mcTr, int MinNStations, int PerformanceMode, float MinRecoMom)
{
  Bool_t f = 1;
  
    // reject very slow tracks from analysis
  f &= (mcTr->GetP() > MinRecoMom);
    // detected at least in 4 stations
  if (PerformanceMode == 3) f &= (GetNConsMCStations()  >= MinNStations); // L1-MC
  if (PerformanceMode == 2) f &= (GetNHitStations()     >= MinNStations); // QA definition
  if (PerformanceMode == 1) f &= (GetNConsHitStations() >= MinNStations); // L1 definition

  // maximul 4 layers for a station.
  f &= (GetNMaxMCPointsOnStation() <= 4);

  return f;
}
