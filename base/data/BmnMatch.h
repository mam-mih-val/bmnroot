/**
 * \file BmnMatch.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2013-2014
 *
 * Base data class for storing RECO-to-MC matching information.
 **/

#ifndef BMNMATCH_H_
#define BMNMATCH_H_

#include "TObject.h"
#include "BmnLink.h"
#include <string>
#include <vector>
#include <algorithm>

using std::string;
using std::vector;
using std::pair;

class BmnMatch : public TObject
{
public:
   /**
    * \brief Default constructor.
    */
   BmnMatch();

   /**
    * \brief Destructor.
    */
   virtual ~BmnMatch();

   /* Accessors */
   const BmnLink& GetLink(Int_t i) const { return fLinks[i]; }
   const vector<BmnLink>& GetLinks() const { return fLinks; }
   const BmnLink& GetMatchedLink() const { return fLinks[fMatchedIndex]; }
   Int_t GetNofLinks() const { return fLinks.size(); }
   Double_t GetTotalWeight() const { return fTotalWeight; }
   Int_t GetMatchedIndex() const { return fMatchedIndex; }

   /* Modifiers */
   void AddLink(const BmnMatch& match);
   void AddLinks(const BmnMatch& match) { AddLink(match); }
   void AddLink(const BmnLink& newLink);
   void AddLink(Double_t weight, Int_t index, Int_t entry = -1, Int_t file = -1);
   void ClearLinks();

   /**
    * \brief Return string representation of the object.
    * \return String representation of the object.
    **/
   virtual string ToString() const;

protected:
   vector<BmnLink> fLinks; // List of links to MC
   Double_t fTotalWeight; // Sum of all reference weights
   Int_t fMatchedIndex; // Index of the matched reference in fReferences array

   ClassDef(BmnMatch, 1);
};

#endif /* BMNMATCH_H_ */
