/**
 * \file CbmLitHitData.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 **/

#include "CbmLitHitData.h"

#include "CbmLitHit.h"
#include "CbmLitStripHit.h"
#include "CbmLitPixelHit.h"

#include <sstream>
#include <iostream>
#include <cassert>
#include <cmath>

using std::endl;
using std::max;

const litfloat CbmLitHitData::EPSILON = 0.005;

CbmLitHitData::CbmLitHitData():
   fHits(),
   fMaxErrX(),
   fMaxErrY(),
   fNofStations(8)// fNofStations(0)
{
}

CbmLitHitData::~CbmLitHitData()
{
}

void CbmLitHitData::SetNofStations(
   Int_t nofStations)
{
   fNofStations = nofStations;
   fHits.resize(nofStations);
   fMaxErrX.resize(nofStations);
   fMaxErrY.resize(nofStations);
   fZPosSet.resize(nofStations);
   fZPosBins.resize(nofStations);
   for(Int_t i = 0; i < nofStations; i++) {
      fHits[i].reserve(nofStations);
      fMaxErrX[i] = 0.;
      fMaxErrY[i] = 0.;
   }
}

void CbmLitHitData::AddHit(
   CbmLitHit* hit)
{
   Int_t station = hit->GetStation() - 1;
   assert(station > -1 && station < fNofStations);
   fHits[station].push_back(hit);

   // Find different Z positions of hits
   fZPosSet[station].insert(hit->GetZ());

   // Find maximum hit error for X and Y
   if (hit->GetType() == kLITPIXELHIT) {
      CbmLitPixelHit* pixelHit = static_cast<CbmLitPixelHit*>(hit);
      fMaxErrX[station] = max(pixelHit->GetDx(), fMaxErrX[station]);
      fMaxErrY[station] = max(pixelHit->GetDy(), fMaxErrY[station]);
   }
}

const CbmLitHit* CbmLitHitData::GetHit(
   Int_t station,
   Int_t hitId) const
{
   return fHits[station][hitId];
}

const HitPtrVector& CbmLitHitData::GetHits(
   Int_t station)
{
   return fHits[station];
}

Int_t CbmLitHitData::GetNofHits(
   Int_t station) const
{
   return fHits[station].size();
}

litfloat CbmLitHitData::GetMaxErrX(
   Int_t station) const
{
   return fMaxErrX[station];
}

litfloat CbmLitHitData::GetMaxErrY(
   Int_t station) const
{
   return fMaxErrY[station];
}

//const vector<litfloat>& CbmLitHitData::GetZPos(
//   Int_t station) const
//{
//   return fZPos[station];
//}

const vector<Int_t>& CbmLitHitData::GetZPosBins(
   Int_t station) const
{
   return fZPosBins[station];
}

litfloat CbmLitHitData::GetZPosByBin(
   Int_t station,
   Int_t bin) const
{
   return GetMinZPos(station) + bin * EPSILON;
}

Int_t CbmLitHitData::GetBinByZPos(
   Int_t station,
   litfloat zPos) const
{
   return (zPos - GetMinZPos(station)) / EPSILON;
}

litfloat CbmLitHitData::GetMinZPos(
   Int_t station) const
{
   return (fZPosSet[station].empty()) ? 0. : *fZPosSet[station].begin();
}

void CbmLitHitData::Clear()
{
   for(UInt_t i = 0; i < fHits.size(); i++) {
      fHits[i].clear();
      fHits[i].reserve(1500);
      fMaxErrX[i] = 0.;
      fMaxErrY[i] = 0.;
      fZPosSet[i].clear();
      fZPosBins[i].clear();
   }
}

void CbmLitHitData::Arrange()
{
   for (Int_t iStation = 0; iStation < fNofStations; iStation++) {
      if (fZPosSet[iStation].empty()) continue;
      Double_t minZ = *fZPosSet[iStation].begin();
      Double_t maxZ = *fZPosSet[iStation].rbegin();
      set<Int_t> binSet;
      set<Double_t>::const_iterator it;
      for (it = fZPosSet[iStation].begin(); it != fZPosSet[iStation].end(); it++) {
         Double_t z = *it;
         Int_t bin = (z - minZ) / EPSILON;
         binSet.insert(bin);
      }

      set<Int_t>::const_iterator it2;
      for (it2 = binSet.begin(); it2 != binSet.end(); it2++) {
         Int_t bin = *it2;
         Double_t z = minZ + bin * EPSILON;
       //  fZPos[iStation].push_back(z);
         fZPosBins[iStation].push_back(bin);
      }
   }
}

string CbmLitHitData::ToString() const
{
   stringstream ss;
   ss << "HitData: nofStations=" << fNofStations << endl;
   for(UInt_t i = 0; i < fHits.size(); i++) {
      ss << " station " << i << ": " << GetNofHits(i) << " hits, "
            << "maxerrx=" << GetMaxErrX(i) << ", maxerry=" << GetMaxErrY(i) << ", ";
      ss << "zposset=(";
      for (set<litfloat>::const_iterator it = fZPosSet[i].begin(); it != fZPosSet[i].end(); it++) {
         ss << *it << ", ";
      }
      ss << ") ";
      ss << "zposbins=(";
      for (vector<Int_t>::const_iterator it = fZPosBins[i].begin(); it != fZPosBins[i].end(); it++) {
         ss << "|" << *it << "," << GetZPosByBin(i, *it) << "| ";
      }
      ss << ")" << endl;
   }
   return ss.str();
}
