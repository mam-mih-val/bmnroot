/* 
 * File:   BmnKalmanFilter_tmp.h
 *
 * Created on July 29, 2015, 9:48 AM
 */

#ifndef BMNKALMANFILTER_TMP_H
#define	BMNKALMANFILTER_TMP_H

#include "BmnEnums.h"
#include "FairTrackParam.h"
#include "BmnHit.h"
#include "TMatrixD.h"
#include "BmnFitNode.h"
#include <vector>
#include "FairRunAna.h"
#include "FairField.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "BmnGemTrack.h"

class BmnKalmanFilter_tmp {
public:
    /* Constructor */
    BmnKalmanFilter_tmp();

    /* Destructor */
    virtual ~BmnKalmanFilter_tmp();

    BmnStatus Prediction(FairTrackParam* par, Float_t dZ, BmnFitNode& node);
    BmnStatus Correction(FairTrackParam* par, BmnHit* hit, Float_t &chi2, BmnFitNode& node);
    FairTrackParam Filtration(BmnGemTrack* tr, TClonesArray* hits);
    TMatrixD Transport(FairTrackParam* par, Float_t zOut, TString type); //transport matrix generation
    BmnStatus AddFitNode(BmnFitNode node);
    vector<BmnFitNode> GetFitNodes() const {return fFitNodes;};
    Bool_t IsParCorrect(const FairTrackParam* par);
    TMatrixD FillVecFromPar(const FairTrackParam* par);
    TMatrixD FillCovFromPar(const FairTrackParam* par);
    BmnStatus FillParFromVecAndCov(TMatrixD x, TMatrixD c, FairTrackParam* par);
    Int_t GetNnodes() const {return fFitNodes.size();};
    
private:
    vector<BmnFitNode> fFitNodes;
    FairField* fField;

};

#endif	/* BMNKALMANFILTER_TMP_H */

