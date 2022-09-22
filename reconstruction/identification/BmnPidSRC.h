#ifndef BMNPIDSRC_H
#define BMNPIDSRC_H

#include "BmnDchTrack.h"
#include "DstEventHeader.h"
#include "BmnGlobalTrack.h"
#include "BmnEnums.h"
#include "BmnKalmanFilter.h"

#include "FairTask.h"
#include "FairRunAna.h"
#include "FairField.h"

#include "TClonesArray.h"
#include "TString.h"
#include "TMath.h"
#include "TH2F.h"
#include "TFile.h"

using namespace std;

class BmnPidSRC : public FairTask
{
  public:
    // Constructors/Destructors ---------
    BmnPidSRC();
    //BmnPidSRC(Int_t Z);
    virtual ~BmnPidSRC() {};

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void RigidityPID();
    void AzPID();
    void DrawPID();   

  private:
    // Private Data Members ------------
    TString fGlobalTracksBranchName;
    TString fDstEventHeaderBranchName;
    TString fDchTrackBranchName;
    TString fTofHitBranchName;

    Int_t fEventNo; // event counter
 //   Int_t fZ; // isotope charge
    Double_t fZin;
    Double_t fZout;

    TClonesArray* fGlobalTracksArray;   //!
    TClonesArray* fBmnDchTrack;         //!
    TClonesArray* fBmnTofHit;           //!
    DstEventHeader* fDstEventHeader;    //!
    BmnKalmanFilter* fKalman;           //!
    
    //cut parameters --------------
    // Double_t RigidityUpBorder[8][14];
    // Double_t RigidityDownBorder[8][14];
    // Double_t ZOutUpBorder[8][14];
    // Double_t ZOutDownBorder[8][14];
    
    TH2F* hPIDgem;                      //!
    TH2F* hPIDdch;                      //!
    
  ClassDef(BmnPidSRC, 1);
};

#endif /* BMNPIDSRC_H */
