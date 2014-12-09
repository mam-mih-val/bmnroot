/**
 * \file BmnLink.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2013-2014
 **/

#include "BmnLink.h"

#include <sstream>

using std::stringstream;

BmnLink::BmnLink() :
 TObject(),
 fFile(-1),
 fEntry(-1),
 fIndex(-1),
 fWeight(-1)
{

}

BmnLink::BmnLink(Float_t weight, Int_t index, Int_t entry, Int_t file) :
      TObject(),
      fFile(file),
      fEntry(entry),
      fIndex(index),
      fWeight(weight)
{

}

BmnLink::~BmnLink()
{

}

string BmnLink::ToString() const
{
   stringstream ss;
   ss << "BmnLink: weight=" << fWeight << " index=" << fIndex << " entry="
         << fEntry << " file=" << fFile << "\n";
   return ss.str();
}

ClassImp(BmnLink)
