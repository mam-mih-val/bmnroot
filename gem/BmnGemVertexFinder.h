#ifndef BMNGEMVERTEXFINDER_H
#define BMNGEMVERTEXFINDER_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "BmnGemTrack.h"
#include "BmnGemStripHit.h"
#include "TMath.h"
#include "TVector3.h"
#include "BmnEnums.h"
#include "FairRunAna.h"
#include "FairField.h"
#include "CbmVertex.h"
#include "BmnKalmanFilter_tmp.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripStationSet_RunSpring2017.h"

using namespace std;

class BmnGemVertexFinder : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnGemVertexFinder();
    virtual ~BmnGemVertexFinder();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void SetField(Bool_t f) {
        fIsField = f;
    }
    
    void SetVertexApproximation(Float_t f) {
        fRoughVertex = f;
    }
    
    void SetVertexApproximation(TVector3 vertex) {
        fRoughVertex3D = vertex;
    }
    
    void FindVertexByVirtualPlanes();
    Float_t FindVZByVirtualPlanes(Float_t z_0, Float_t range);

private:

    // Private Data Members ------------
    TString fHitsBranchName;
    TString fTracksBranchName;
    TString fVertexBranchName;

    Int_t fEventNo; // event counter
    Int_t fNTracks; // number of reco tracks in event
    
    BmnGemStripStationSet* fDetector;

    TClonesArray* fGemHitsArray;
    TClonesArray* fGemTracksArray;
    TClonesArray* fVertexArray;

    Bool_t fIsField;
    FairField* fField;
    BmnKalmanFilter_tmp* fKalman;
    Float_t fRoughVertex;
    TVector3 fRoughVertex3D;

    ClassDef(BmnGemVertexFinder, 1);
};


#endif /* BMNGEMVERTEXFINDER_H */

