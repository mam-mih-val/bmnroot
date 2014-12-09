/**
 * \file BmnTrackMatchNew.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2013-2014
 *
 * Base data class for storing RECO-to-MCTrack matching information.
 **/

#ifndef BMNTRACKMATCHNEW_H_
#define BMNTRACKMATCHNEW_H_

#include "BmnMatch.h"
#include <string>

using std::string;

class BmnTrackMatch : public BmnMatch
{
public:
   /**
    * \brief Default constructor.
    */
   BmnTrackMatch();

   /**
    * \brief Destructor.
    */
   virtual ~BmnTrackMatch();

   /* Accessors */
   Int_t GetNofTrueHits() const { return fNofTrueHits; }
   Int_t GetNofWrongHits() const { return fNofWrongHits; }
   Int_t GetNofHits() const { return fNofTrueHits + fNofWrongHits; }
   Double_t GetTrueOverAllHitsRatio() const {
      Double_t all = GetNofHits();
      return (all == 0) ? 0. : (fNofTrueHits / all);
   }
   Double_t GetWrongOverAllHitsRatio() const {
      Double_t all = GetNofHits();
      return (all == 0) ? 0. : (fNofWrongHits / all);
   }

   /* Modifiers */
   void SetNofTrueHits(Int_t nofTrueHits) { fNofTrueHits = nofTrueHits; }
   void SetNofWrongHits(Int_t nofWrongHits) { fNofWrongHits = nofWrongHits; }

   /**
    * \brief Return string representation of the object.
    * \return String representation of the object.
    **/
   virtual string ToString() const;

private:
   Int_t fNofTrueHits;  // Number of true hits in reconstructed track
   Int_t fNofWrongHits; // Number of wrong hits in reconstructed track

   ClassDef(BmnTrackMatch, 1);
};

#endif /* BMNTRACKMATCHNEW_H_ */
