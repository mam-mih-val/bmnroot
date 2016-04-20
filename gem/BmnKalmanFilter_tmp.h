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
#include "BmnMatrixMath.h"
#include "BmnMath.h"
#include "TClonesArray.h"
#include "BmnGemTrack.h"
#include "BmnGeoNavigator.h"
#include "BmnMaterialEffects.h"

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
    TMatrixD FillVecFromPar(const FairTrackParam* par);
    TMatrixD FillCovFromPar(const FairTrackParam* par);
    BmnStatus FillParFromVecAndCov(TMatrixD x, TMatrixD c, FairTrackParam* par);
    Int_t GetNnodes() const {return fFitNodes.size();};
    
    void RK4Order(const vector<Double_t>& xIn, Float_t zIn, vector<Double_t>& xOut, Float_t zOut, vector<Double_t>& derivs);
    BmnStatus RK4TrackExtrapolate(FairTrackParam* par, Float_t zOut, vector<Double_t>* F);
    Float_t CalcOut(Float_t in, const Float_t k[4]);
    void TransportC(const vector<Double_t>& cIn, const vector<Double_t>& F, vector<Double_t>& cOut);
    BmnStatus Update(FairTrackParam* par, const BmnHit* hit, Float_t& chiSq);
    void UpdateF(vector<Double_t>& F, const vector<Double_t>& newF);
    
    BmnStatus FitSmooth(BmnGemTrack* track, TClonesArray* hits);
    BmnStatus Smooth(BmnFitNode* thisNode, BmnFitNode* prevNode);
    
    BmnStatus TGeoTrackPropagate(FairTrackParam* par, Float_t zOut, Int_t pdg, vector<Double_t>* F, Float_t* length, TString type);
    
private:
    vector<BmnFitNode> fFitNodes;
    FairField* fField;
    BmnGeoNavigator* fNavigator;
    BmnMaterialEffects* fMaterial;

};

#endif	/* BMNKALMANFILTER_TMP_H */

