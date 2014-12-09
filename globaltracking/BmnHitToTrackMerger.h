/**
 * \file BmnHitToTrackMerger.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for hit-to-track merging algorithm.
 */

#ifndef BMNHITTOTRACKMERGER_H_
#define BMNHITTOTRACKMERGER_H_

#include "BmnEnums.h"
#include "TClonesArray.h"
#include "BmnTrackPropagator.h"
#include "BmnKalmanFilter.h"

/**
 * \class BmnHitToTrackMerger
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for hit-to-track merging algorithm.
 */
class BmnHitToTrackMerger {
public:
    /**
     * \brief Constructor.
     */
    BmnHitToTrackMerger();

    /**
     * \brief Destructor.
     */
    virtual ~BmnHitToTrackMerger();

    BmnStatus NearestTofHitMerge(TClonesArray* hits, TClonesArray* tracks, TClonesArray* tofTracks);
    BmnStatus AllTofHitsMerge(TClonesArray* hits, TClonesArray* tracks, TClonesArray* tofTracks);

private:
    BmnTrackPropagator* fPropagator; // Track propagation tool
    BmnKalmanFilter*    fFilter; // Track update tool
    Int_t fPDG; // PDG hypothesis
    Float_t fChiSqCut; // Chi square cut for hit to be attached to track.
};

#endif /* BMNHITTOTRACKMERGER_H_ */
