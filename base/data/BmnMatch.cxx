/**
 * \file BmnMatch.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2013-2014
 **/

#include "BmnMatch.h"

#include <sstream>

using std::make_pair;
using std::stringstream;

BmnMatch::BmnMatch() :
      fLinks(),
      fTotalWeight(0.),
      fMatchedIndex(-1)
{

}

BmnMatch::~BmnMatch()
{
    fLinks.clear();
}

string BmnMatch::ToString() const
{
   stringstream ss;
   ss << "BmnMatch: ";
   Int_t nofLinks = GetNofLinks();
   ss << "nofLinks=" << nofLinks << "\n";
   for (Int_t i = 0; i < nofLinks; i++) {
      const BmnLink& link = fLinks[i];
      ss << link.ToString();
   }
   ss << " totalWeight=" << fTotalWeight << ", matchedIndex="
         << fMatchedIndex << std::endl;
   return ss.str();
}

void BmnMatch::AddLink(const BmnMatch& match)
{
   Int_t nofLinks = match.GetNofLinks();
   for (Int_t i = 0; i < nofLinks; i++) {
      AddLink(match.GetLink(i));
   }
}

void BmnMatch::AddLink(const BmnLink& newLink)
{
   Int_t addedIndex = -1;
   Int_t nofLinks = GetNofLinks();
   for (Int_t i = 0; i < nofLinks; i++) {
      BmnLink& link = fLinks[i];
      if (link == newLink) {
         link.AddWeight(newLink.GetWeight());
         addedIndex = i;
         break;
      }
   }
   if (addedIndex < 0) {
      fLinks.push_back(newLink);
      addedIndex = fLinks.size() - 1;
   }

   fTotalWeight += newLink.GetWeight();
   if (fMatchedIndex < 0) {
      fMatchedIndex = addedIndex;
   } else {
      if (fLinks[addedIndex].GetWeight() > fLinks[fMatchedIndex].GetWeight()) {
         fMatchedIndex = addedIndex;
      }
   }
}

void BmnMatch::AddLink(Double_t weight, Int_t index, Int_t entry, Int_t file) {
	BmnLink link(weight, index, entry, file);
	AddLink(link);
}


void BmnMatch::ClearLinks()
{
   fLinks.clear();
   fTotalWeight = 0.;
   fMatchedIndex = -1;
}

ClassImp(BmnMatch);
