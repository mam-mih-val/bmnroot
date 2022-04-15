// -------------------------------------------------------------------------
// -----                  CbmStsMatchTracks header file                -----
// -----                  Created 22/11/05  by V. Friese               -----
// -----                  BmnStsMatchTracks header file                -----
// -----                  Created 26/12/21  by A. Zinchenko            ----- 
// -------------------------------------------------------------------------


/** CbmStsMatchTracks.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for matching a reconstructed CbmStsTrack with a simulated
 ** CbmMCTrack. The matching criterion is a maximal number of common
 ** hits/points. The task fills the data class BmnStsTrackMatch for
 ** each CbmStsTrack.
 **/


#ifndef BMNSTSMATCHTRACKS_H
#define BMNSTSMATCHTRACKS_H 1


#include "FairTask.h"

#include "TStopwatch.h"

#include <map>

class TClonesArray;



class BmnStsMatchTracks : public FairTask
{

 public:

  /** Default constructor **/
  BmnStsMatchTracks();


  /** Standard constructor
   **
   *@param verbose  Verbosity level
   **/
  BmnStsMatchTracks(Int_t iVerbose );


  /** Constructor with task name
   **
   *@param name     Name of task
   *@param verbose  Verbosity level
   **/
  BmnStsMatchTracks(const char* name, Int_t verbose );


  /** Destructor **/
  virtual ~BmnStsMatchTracks();


  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Finishing */
  virtual void Finish();

  void SetTrackArray(TClonesArray *tracks) { fTracks = tracks; } //AZ - set track array
  void SetMatchArray(TClonesArray *matches) { fMatches = matches; } //AZ - set matches array
  void SetTrackBranch(TString trBranch = "StsTrack") { fTrBranch = trBranch; } //AZ - set track branch name 

 private:

  TClonesArray* fTracks;       // Array of CbmStsTracks
  //TClonesArray* fPoints;       // Array of CbmStsPoints
  TClonesArray* fGemPoints;    // Array of CbmStsPoints
  TClonesArray* fSilPoints;    // Array of CbmStsPoints
  TClonesArray* fHits;         // Array of CbmStsHits
  TClonesArray* fMatches;      // Array of CbmStsTrackMatch
  TStopwatch    fTimer;        // Timer
  TString       fTrBranch;     //AZ - Track branch name

  /** Map from MCTrackId to number of common hits **/
  std::map<Int_t, Int_t> fMatchMap;

  Int_t    fNEvents;        /** Number of events with success **/
  Int_t    fNEventsFailed;  /** Number of events with failure **/
  Double_t fTime;           /** Total real time used for good events **/
  Double_t fNTrackMatches;  /** Total number of matched tracks **/
  Double_t fNAllHits;       /** Total number of hits **/
  Double_t fNTrueHits;      /** Number pf correctly assigned hits **/
  
  BmnStsMatchTracks(const BmnStsMatchTracks&);
  BmnStsMatchTracks operator=(const BmnStsMatchTracks&);

  ClassDef(BmnStsMatchTracks,0);

};

#endif
