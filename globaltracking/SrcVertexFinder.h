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
#include "BmnVertex.h"
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

    void FindVertexByVirtualPlanes(vector<BmnTrack> &lTracks, vector<BmnTrack> &rTracks);
    Float_t FindVZByVirtualPlanes(Float_t z_0, Float_t range, vector<BmnTrack> tracks, Float_t& minDist);
    void CreateArmCandidates(vector<BmnTrack>& lTracks, vector<BmnTrack>& rTracks);

   private:

    Double_t CalcMeanDist(vector<Double_t> x, vector<Double_t> y);

    Int_t fEventNo; // event counter
    Int_t fPeriodId; // event counter

    //in branches
    TClonesArray* fGlobalTracksArray;
    TClonesArray* fGemHitsArray;
    TClonesArray* fTof400HitsArray;
    //out branches
    TClonesArray* fVertexArray;
    TClonesArray* fArmTracksArray;

    Double_t fTime;

    Bool_t fIsField;
    FairField* fField;
    BmnKalmanFilter* fKalman;

    ClassDef(SrcVertexFinder, 1);
};


#endif /* SRCVERTEXFINDER_H */
