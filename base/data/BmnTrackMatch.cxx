/**
 * \file BmnTrackMatchNew.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2013-2014
 *
 **/

#include "BmnTrackMatch.h"

#include <sstream>

using std::make_pair;
using std::stringstream;

BmnTrackMatch::BmnTrackMatch() :
      BmnMatch(),
      fNofTrueHits(0),
      fNofWrongHits(0)
{
}

BmnTrackMatch::~BmnTrackMatch() {}

string BmnTrackMatch::ToString() const {
   stringstream ss;
   ss << "BmnMatch: ";
   Int_t nofLinks = GetNofLinks();
   ss << "nofLinks=" << nofLinks << "\n";
   for (Int_t i = 0; i < nofLinks; i++) {
      const BmnLink& link = fLinks[i];
      ss << link.ToString();
   }
   ss << "| totalWeight=" << fTotalWeight << ", matchedIndex="
         << fMatchedIndex << " nofTrueHits=" << fNofTrueHits
         << " nofWrongHits=" << fNofWrongHits << std::endl;
   return ss.str();
}

ClassImp(BmnTrackMatch);
