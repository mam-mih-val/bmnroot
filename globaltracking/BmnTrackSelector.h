
#ifndef BMNTRACKSELECTOR_H_
#define BMNTRACKSELECTOR_H_

#include "BmnEnums.h"
#include "TClonesArray.h"

/**
 * \class BmnTrackSelector
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for track selection algorithm.
 */
class BmnTrackSelector
{
public:
   /**
    * \brief Constructor.
    */
   BmnTrackSelector();
   
      /* Constructor */
   BmnTrackSelector(
      Float_t maxChiSq,
      Int_t minLastPlaneId,
      Int_t minNofHits,
      Float_t minMomentum);

   /**
    * \brief Destructor.
    */
   virtual ~BmnTrackSelector();

   /* Return chi square cut */
   Float_t GetMaxChiSq() const {return fMaxChiSq;}

   /* Returns last plane index cut */
   Int_t GetMinLastPlaneId() const {return fMinLastPlaneId;}

   /* Returns number of hits cut */
   Int_t GetMinNofHits() const {return fMinNofHits;}

   /* Returns momentum cut */
   Float_t GetMinMomentum() const {return fMinMomentum;}

   /* Sets chi square cut */
   void SetMaxChiSq(Float_t maxChiSq) {fMaxChiSq = maxChiSq;}

   /* Sets last plane index cut */
   void SetMinLastPlaneId(Int_t minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}

   /* Sets number of hits cut */
   void SetMinNofHits(Int_t minNofHits) {fMinNofHits = minNofHits;}

   /* Sets momentum cut */
   void SetMinMomentum(Float_t minMomentum) {fMinMomentum = minMomentum;}
   
   
   
   BmnStatus TrackSelectCuts(TClonesArray* tracks);
   
private:
   /* Chi square cut */
   Float_t fMaxChiSq;
   /* Last plane index cut */
   Int_t fMinLastPlaneId;
   /* Number of hits cut */
   Int_t fMinNofHits;
   /* Momentum cut in [GeV/c] */
   Float_t fMinMomentum;
   
};

#endif /*BMNTRACKSELECTOR_H_*/
