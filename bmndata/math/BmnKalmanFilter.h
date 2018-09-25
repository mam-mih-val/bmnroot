/* 
 * File:   BmnKalmanFilter.h
 *
 * Created on July 29, 2015, 9:48 AM
 */

#ifndef BmnKalmanFilter_H
#define	BmnKalmanFilter_H

#include "BmnEnums.h"
#include "FairTrackParam.h"
#include "BmnHit.h"
#include "TMatrixD.h"
#include "BmnFitNode.h"
#include <vector>
#include "FairRunAna.h"
#include "FairField.h"
#include "TMath.h"
#include "BmnMath.h"
#include "BmnMatrixMath.h"
#include "TClonesArray.h"
#include "BmnGemTrack.h"
#include "BmnGeoNavigator.h"
#include "BmnMaterialEffects.h"

class BmnKalmanFilter {
public:
    /* Constructor */
    BmnKalmanFilter();

    /* Destructor */
    virtual ~BmnKalmanFilter();

    BmnStatus Prediction(FairTrackParam* par, Double_t dZ, BmnFitNode& node);
    BmnStatus Correction(FairTrackParam* par, BmnHit* hit, Double_t &chi2, BmnFitNode& node);
    FairTrackParam Filtration(BmnGemTrack* tr, TClonesArray* hits);
    TMatrixD Transport(FairTrackParam* par, Double_t zOut, Bool_t isField); //transport matrix generation
    BmnStatus AddFitNode(BmnFitNode node);
    vector<BmnFitNode> GetFitNodes() const {return fFitNodes;};
    TMatrixD FillVecFromPar(const FairTrackParam* par);
    TMatrixD FillCovFromPar(const FairTrackParam* par);
    BmnStatus FillParFromVecAndCov(TMatrixD x, TMatrixD c, FairTrackParam* par);
    Int_t GetNnodes() const {return fFitNodes.size();};
    
    void RK4Order(const vector<Double_t>& xIn, Double_t zIn, vector<Double_t>& xOut, Double_t zOut, vector<Double_t>& derivs);
    BmnStatus RK4TrackExtrapolate(FairTrackParam* par, Double_t zOut, vector<Double_t>* F);
    Double_t CalcOut(Double_t in, const Double_t k[4]);
    void TransportC(const vector<Double_t>& cIn, const vector<Double_t>& F, vector<Double_t>& cOut);
    BmnStatus Update(FairTrackParam* par, const BmnHit* hit, Double_t& chiSq);
    void UpdateF(vector<Double_t>& F, const vector<Double_t>& newF);
    
    BmnStatus FitSmooth(BmnGemTrack* track, TClonesArray* hits);
    BmnStatus Smooth(BmnFitNode* thisNode, BmnFitNode* prevNode);
    
    BmnStatus TGeoTrackPropagate(FairTrackParam* par, Double_t zOut, Int_t pdg, vector<Double_t>* F, Double_t* length, Bool_t isField);
    
private:
    vector<BmnFitNode> fFitNodes;
    FairField* fField;
    BmnGeoNavigator* fNavigator;
    BmnMaterialEffects* fMaterial;

};

#endif	/* BmnKalmanFilter_H */

