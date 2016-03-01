/**
 * \file CbmLitFitNode.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Data class for storage of fitted track parameters, transport matrix and chi-square on each detector station.
 */

#ifndef BMNFITNODE_H_
#define BMNFITNODE_H_

#include "FairTrackParam.h"
#include "TMatrixD.h"
#include <vector>
using namespace std;

/**
 * \class CbmLitFitNode
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Data class for storage of fitted track parameters, transport matrix and chi-square on each detector station.
 */
class BmnFitNode
{
public:
   /**
    * \brief Constructor.
    */
   BmnFitNode():
      fF(25, 0.),
      fPredictedParam(),
      fUpdatedParam(),
      fSmoothedParam(),
      fChiSqFiltered(0.),
      fChiSqSmoothed(0.) {
          fF_matr.ResizeTo(5, 5);
   }

   /**
    * \brief Destructor.
    */
   virtual ~BmnFitNode() {};

   /* Getters */
   const vector<Double_t>& GetF() const { return fF; }
   const TMatrixD GetF_matr() const { return fF_matr; }
   const FairTrackParam* GetPredictedParam() const { return &fPredictedParam; }
   FairTrackParam* GetUpdatedParam() { return &fUpdatedParam; }
   FairTrackParam* GetSmoothedParam() { return &fSmoothedParam; }
   Float_t GetChiSqFiltered() const { return fChiSqFiltered; }
   Float_t GetChiSqSmoothed() const { return fChiSqSmoothed; }

   /* Setters */
   void SetF(const vector<Double_t>& F) { fF.assign(F.begin(), F.end()); }
   void SetF_matr(const TMatrixD& F) { fF_matr = F; }
   void SetPredictedParam(const FairTrackParam* par) { fPredictedParam = *par;}
   void SetUpdatedParam(const FairTrackParam* par) { fUpdatedParam = *par;}
   void SetSmoothedParam(const FairTrackParam* par) { fSmoothedParam = *par;}
   void SetChiSqFiltered(Float_t chiSq) { fChiSqFiltered = chiSq; }
   void SetChiSqSmoothed(Float_t chiSq) { fChiSqSmoothed = chiSq; }

private:
   vector<Double_t> fF; // Transport matrix.
   
   TMatrixD fF_matr; // Transport matrix.

   FairTrackParam fPredictedParam; // Predicted track parameters.
   FairTrackParam fUpdatedParam; // Updated with KF track parameters.
   FairTrackParam fSmoothedParam; // Smoothed track parameters.

   Float_t fChiSqFiltered; // Contribution to chi-square of updated track parameters and hit.
   Float_t fChiSqSmoothed; // Contribution to chi-square of smoothed track parameters and hit.
};

#endif /*BMNFITNODE_H_*/
