/**
 * \file BmnTrackFitter.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for track fitter algorithm.
 */

#ifndef BMNTRACKFITTER_H_
#define BMNTRACKFITTER_H_

#include "BmnEnums.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "BmnTrackPropagator.h"
#include "BmnKalmanFilter.h"
#include "BmnFitNode.h"
#include "TClonesArray.h"

/**
 * \class BmnTrackFitter
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Interface for track fitter algorithm.
 */
class BmnTrackFitter {
public:
    /**
     * \brief Constructor.
     */
    BmnTrackFitter();
    BmnTrackFitter(BmnTrackPropagator* prp, BmnKalmanFilter* upd, Int_t nItr = 2, Float_t chi = 15, Int_t nHits = 3);

    /**
     * \brief Destructor.
     */
    virtual ~BmnTrackFitter();

    BmnStatus FitImp(CbmGlobalTrack* glTrack, TClonesArray* stsTracks, TClonesArray* stsHits, TClonesArray* tofHits, Bool_t downstream = kTRUE);
    BmnStatus FitIter(CbmGlobalTrack* glTrack, Bool_t downstream = kTRUE);
    BmnStatus FitSmooth(CbmGlobalTrack* track, Bool_t downstream = kTRUE);
    void Smooth(BmnFitNode* thisNode, const BmnFitNode* prevNode);

private:
    /* Track propagation tool */
    BmnTrackPropagator* fPropagator;
    /* Track update tool */
    BmnKalmanFilter* fUpdate;
    /* Number of iterations */
    Int_t fNofIterations;
    /* Cut on chi square for single hit */
    Float_t fChiSqCut;
    /* Minimum number of hits in track */
    Int_t fMinNofHits;


};

#endif
