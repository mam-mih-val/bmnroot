#ifndef SRCVERTEXFINDER_H
#define SRCVERTEXFINDER_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "BmnGemTrack.h"
#include "BmnGlobalTrack.h"
#include "BmnGemStripHit.h"
#include "TMath.h"
#include "TVector3.h"
#include "BmnEnums.h"
#include "FairRunAna.h"
#include "FairField.h"
#include "CbmVertex.h"
#include "BmnKalmanFilter.h"
#include <TStopwatch.h>

using namespace std;

class SrcVertexFinder : public FairTask {
public:

    // Constructors/Destructors ---------
    SrcVertexFinder() {};
    SrcVertexFinder(Int_t period, Bool_t isField);
    virtual ~SrcVertexFinder();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void SetField(Bool_t f) {
        fIsField = f;
    }

    void SetVertexApproximation(TVector3 vertex) {
        fRoughVertex3D = vertex;
    }

    void FindVertexByVirtualPlanes();
    Float_t FindVZByVirtualPlanes(Float_t z_0, Float_t range);

private:

    Double_t CalcRms2D(vector<Double_t> x, vector<Double_t> y);
    Double_t CalcMeanDist(vector<Double_t> x, vector<Double_t> y);

    // Private Data Members ------------
    TString fGlobalTracksBranchName;
    TString fVertexBranchName;

    Int_t fEventNo; // event counter
    Int_t fPeriodId; // event counter
    Int_t fNTracks; // number of reco tracks in event

    TClonesArray* fGlobalTracksArray;
    TClonesArray* fVertexArray;

    Double_t fTime;

    Bool_t fIsField;
    FairField* fField;
    BmnKalmanFilter* fKalman;
    TVector3 fRoughVertex3D;

    ClassDef(SrcVertexFinder, 1);
};


#endif /* SRCVERTEXFINDER_H */
