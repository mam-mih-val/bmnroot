/**
 * \file BmnLink.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2013-2014
 *
 * Base data class for storing MC information link.
 **/

#ifndef BMNLINK_H_
#define BMNLINK_H_

#include "TObject.h"
#include <string>

using std::string;

class BmnLink : public TObject
{
public:
   /**
    * \brief Constructor.
    */
   BmnLink();

   /**
    * \brief Standard constructor.
    */
   BmnLink(Float_t weight, Int_t index, Int_t entry = -1, Int_t file = -1);

   /**
    * \brief Destructor.
    */
   virtual ~BmnLink();

   /* Modifiers */
   Int_t GetFile() const { return fFile; }
   Int_t GetEntry() const { return fEntry; }
   Int_t GetIndex() const { return fIndex; }
   Float_t GetWeight() const { return fWeight; }

   /* Accessors */
   void SetFile(Int_t file) { fFile = file; }
   void SetEntry(Int_t entry) { fEntry = entry; }
   void SetIndex(Int_t index) { fIndex = index; }
   void SetWeight(Float_t weight) { fWeight = weight; }

   void AddWeight(Float_t weight) { fWeight += weight; }

   /**
    * \brief Return string representation of the object.
    * \return String representation of the object.
    **/
   virtual string ToString() const;

   friend Bool_t operator==(const BmnLink& lhs, const BmnLink& rhs) {
      return (lhs.GetFile() == rhs.GetFile() && lhs.GetEntry() == rhs.GetEntry() && lhs.GetIndex() == rhs.GetIndex());
   }

private:
   Int_t fFile; // File ID
   Int_t fEntry; // Entry number
   Int_t fIndex; // Index in array
   Float_t fWeight; // Weight

   ClassDef(BmnLink, 1)
};

#endif /* BMNLINK_H_ */
