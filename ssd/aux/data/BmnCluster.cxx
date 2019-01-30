/**
 * \file BmnCluster.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author D. Baranov
 * \date 07.12.2018
 */

#include "BmnCluster.h"

#include "BmnMatch.h"

#include <sstream>

using namespace std;

BmnCluster::BmnCluster()
 : TObject(),
   fDigis(),
   fAddress(0),
   fMatch(NULL)
{

}
BmnCluster::BmnCluster(const std::vector<Int_t>& indices, Int_t address)
 : TObject(),
   fDigis(),
   fAddress(address),
   fMatch(NULL)
{
 fDigis.assign(indices.begin(), indices.end());
}

BmnCluster::~BmnCluster()
{

}

void BmnCluster::SetMatch(BmnMatch* match) {
	 if ( fMatch ) delete fMatch;
	 fMatch = match;
}


string BmnCluster::ToString() const
{
   stringstream ss;
   ss << "BmnCluster: ";
   Int_t nofDigis = GetNofDigis();
   ss << "nofDigis=" << nofDigis << " | ";
   for (Int_t i = 0; i < nofDigis; i++) {
      ss << fDigis[i] << " ";
   }
   ss << " | address=" << fAddress << endl;
   return ss.str();
}

ClassImp(BmnCluster)
