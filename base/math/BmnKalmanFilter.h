/* 
 * File:   BmnKalmanFilter.h
 *
 * Created on July 29, 2015, 9:48 AM
 */

#ifndef BmnKalmanFilter_H
#define	BmnKalmanFilter_H

#include "BmnEnums.h"
#include "BmnGeoNavigator.h"
#include "BmnMaterialEffects.h"
#include "BmnHit.h"
#include "BmnFitNode.h"
#include "BmnMath.h"
#include "BmnMatrixMath.h"

#include "FairTrackParam.h"
#include "FairRunAna.h"
#include "FairField.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TMatrixD.h"

#include <vector>

class BmnKalmanFilter
{
  public:
    /* Constructor */
    BmnKalmanFilter();
    /* Destructor */
    virtual ~BmnKalmanFilter();
    
    void RK4Order(const vector<Double_t>& xIn, Double_t zIn, vector<Double_t>& xOut, Double_t zOut, vector<Double_t>& derivs);
    BmnStatus RK4TrackExtrapolate(FairTrackParam* par, Double_t zOut, vector<Double_t>* F);
    Double_t CalcOut(Double_t in, const Double_t k[4]);
    void TransportC(const vector<Double_t>& cIn, const vector<Double_t>& F, vector<Double_t>& cOut);
    BmnStatus Update(FairTrackParam* par, const BmnHit* hit, Double_t& chiSq);
    void UpdateF(vector<Double_t>& F, const vector<Double_t>& newF);
    
    BmnStatus Smooth(BmnFitNode* thisNode, BmnFitNode* prevNode);
    
    BmnStatus TGeoTrackPropagate(FairTrackParam* par, Double_t zOut, Int_t pdg, vector<Double_t>* F, Double_t* length, Bool_t isField);
    BmnStatus TGeoTrackPropagate(FairTrackParam* par, Double_t zOut, Int_t pdg, vector<Double_t>* F, Double_t* length);

  private:
    FairField* fField;
    BmnGeoNavigator* fNavigator;
    BmnMaterialEffects* fMaterial;  //!
};

#endif	/* BmnKalmanFilter_H */
