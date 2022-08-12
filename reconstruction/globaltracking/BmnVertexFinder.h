#ifndef BMNVERTEXFINDER_H
#define BMNVERTEXFINDER_H

#include "BmnEnums.h"
#include "BmnVertex.h"
#include "BmnKalmanFilter.h"
#include "BmnGemTrack.h"
#include "BmnGlobalTrack.h"

#include "FairTask.h"
#include "FairRunAna.h"
#include "FairField.h"

#include "TClonesArray.h"
#include "TString.h"
#include "TMath.h"
#include "TVector3.h"
#include <TStopwatch.h>

using namespace std;

class BmnVertexFinder : public FairTask
{
  public:
    // Constructors/Destructors ---------
    BmnVertexFinder() {}
    BmnVertexFinder(Int_t period);
    virtual ~BmnVertexFinder();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    BmnVertex FindPrimaryVertex(TClonesArray* tracks);
    BmnVertex FindSecondaryVertex(TClonesArray* tracks);
    Float_t FindVZByVirtualPlanes(Float_t z_0, Float_t range, TClonesArray* tracks, Int_t flag);

  private:
    Double_t CalcRms2D(vector<Double_t> x, vector<Double_t> y);
    Double_t CalcMeanDist(vector<Double_t> x, vector<Double_t> y);

    // Private Data Members ------------
    TString fGlobalTracksBranchName;
    TString fVertexBranchName;

    Int_t fEventNo; // event counter
    Int_t fPeriodId; // event counter

    TClonesArray* fGlobalTracksArray;   //!
    TClonesArray* fVertexArray;         //!

    Double_t fTime;

    Bool_t fRobustRefit;
    FairField* fField;                  //!
    BmnKalmanFilter* fKalman;           //!

  ClassDef(BmnVertexFinder, 1);
};

#endif /* BMNVERTEXFINDER_H */
