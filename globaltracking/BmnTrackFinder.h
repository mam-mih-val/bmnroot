/**
 * \brief CbmLitTrackFinder.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for track finding algorithm.
 */

#ifndef BMNTRACKFINDER_H_
#define BMNTRACKFINDER_H_

#include "TClonesArray.h"
#include "BmnEnums.h"

/**
 * \class CbmLitTrackFinder
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for track finding algorithm.
 */
class BmnTrackFinder
{
public:
   /**
    * \brief Constructor.
    */
   BmnTrackFinder();

   /**
    * \brief Destructor.
    */
   virtual ~BmnTrackFinder();

   /**
    * \brief Main function to be implemented for concrete track finder algorithm.
    * \param[in] hits Input vector of hits.
    * \param[in] trackSeeds Input vector of track seeds.
    * \param[out] tracks Output vector of found tracks.
    * \return Status code.
    */
//   virtual BmnStatus DoFind( TClonesArray* hits, TClonesArray* trackSeeds, TClonesArray* tracks) = 0;
   
   BmnStatus BranchTrackFind(TClonesArray* hits, TClonesArray* trackSeeds, TClonesArray* tracks);
//   void CopyToOutput(TrackPtrIterator itBegin, TrackPtrIterator itEnd, TrackPtrVector& tracks);
//   void RemoveHits(TrackPtrIterator itBegin, TrackPtrIterator itEnd);
//   void FollowTracks(TrackPtrIterator itBegin, TrackPtrIterator itEnd);
//   void InitTrackSeeds(TrackPtrIterator itBegin, TrackPtrIterator itEnd);
//   void ArrangeHits(HitPtrIterator itBegin, HitPtrIterator itEnd);
   
   BmnStatus NNTrackFind(TClonesArray* hits, TClonesArray* trackSeeds, TClonesArray* tracks);
};

#endif /*CBMLITTRACKFINDER_H_*/
