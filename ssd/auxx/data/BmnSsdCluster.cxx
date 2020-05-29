/**
 ** \file BmnSsdCluster.cxx
 ** \author V.Friese <v.friese@gsi.de>
 ** \author D. Baranov
 ** \since 07.12.2018
 **
 **/

#include "BmnSsdCluster.h"

#include <sstream>

using namespace std;


// --- Constructor
BmnSsdCluster::BmnSsdCluster()
    : BmnCluster()
    , fCharge(0.)
    , fSize(0)
    , fPosition(0.)
    , fPositionError(0.)
    , fTime(0.)
    , fTimeError(0.)
    , fIndex(-1)
{
}


// --- Destructor
BmnSsdCluster::~BmnSsdCluster()
{
}


// --- String output
string BmnSsdCluster::ToString() const {
   stringstream ss;
   ss << "SsdCluster: address " << GetAddress() << " | digis "
  		<< GetNofDigis() << " | charge " << fCharge << " | time "
  		<< fTime << " +- " << fTimeError
		<< " | position " << GetPosition() << " | error "
  		<< GetPositionError();
   return ss.str();
}



ClassImp(BmnSsdCluster)
