
#ifndef CBM_L1_STTTRACKFINDER_H
#define CBM_L1_STTTRACKFINDER_H

#include "FairTask.h"
#include "CbmStsKFTrackFitter.h"
//#include "TLorentzVector.h"
#include "TH1.h"


class TClonesArray;

class CbmL1SttTrackFinder :public FairTask 
{
 public:

  /** Constructor **/
  CbmL1SttTrackFinder(const char *name="CbmL1SttTrackFinder", Int_t iVerbose = 1 );
  
  /** Destructor **/
  ~CbmL1SttTrackFinder();
  
  /// * FairTask methods
  
  /** Intialisation at begin of run. To be implemented in the derived class.
   *@value  Success   If not kSUCCESS, task will be set inactive.
   **/
  InitStatus Init();
  
  /** Reinitialisation.
   *@value  Success   If not kSUCCESS, task will be set inactive.
   **/
  InitStatus ReInit();

  /** Intialise parameter containers.
   **/
  void SetParContainers();

  void Exec(Option_t * option);

  /** Action after each event. **/
  void Finish();

 private:

  TClonesArray *fSttPoints; //! STT MC points
  TClonesArray *fSttHits; //! STT Hits
  TClonesArray *fMuchTracks;
  TClonesArray *fStsTracks;
  TClonesArray *fMCTracks;
  TClonesArray *fSTSTrackMatch;
  TClonesArray *fTrackCollection; //! STT tracks

  CbmVertex *fPrimVtx;  
  CbmStsKFTrackFitter fStsFitter;

  TDirectory *histodir;

  void Write();
  void writedir2current( TObject *obj );

  TH1F *fhNBranches;

 public:
  ClassDef(CbmL1SttTrackFinder,1);

};


#endif
