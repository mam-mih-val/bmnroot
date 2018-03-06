// -------------------------------------------------------------------------
// -----                   CbmStsTrackFinder header file               -----
// -----                  Created 02/02/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsTrackFinder
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Abstract base class for concrete STS track finding algorithm.
 ** Each derived class must implement the method DoFind. This has
 ** to operate on the two TClonesArrays of pixel and strip hits
 ** and to fill the CbmStsTrackArray.
 **/

#ifndef CBMSTSTRACKFINDER
#define CBMSTSTRACKFINDER 1


#include "TObject.h"
#include "TNamed.h"

class TClonesArray;
class CbmStsDigiScheme;
class FairField;


class CbmStsTrackFinder : public TNamed
{

 public:

  /** Default constructor **/
  CbmStsTrackFinder(); 


  /** Destructor **/
  virtual ~CbmStsTrackFinder() { };


  /** Virtual method Init. If needed, to be implemented in the
   ** concrete class. Else no action.
   **/
  virtual void Init() { };


  /** Abstract method DoFind. To be implemented in the concrete class.
   ** Task: Read the hit array and fill the track array,
   ** pointers to which are private members and set by the task
   **
   *@value Number of tracks created
   **/
  virtual Int_t DoFind() = 0;

  
  /** Virtual method Finish. If needed, to be implemented in the concrete
   ** class. Executed at the end of the run.
   **/
  virtual void Finish() { };


  /** Modifiers **/
  void SetDigiScheme(CbmStsDigiScheme* scheme) { fDigiScheme = scheme; }
  void SetField(FairField* field)               { fField = field; }
  void SetMvdHitArray(TClonesArray* hits)      { fMvdHits = hits; }
  void SetStsHitArray(TClonesArray* hits)      { fStsHits = hits; }
  void SetTrackArray(TClonesArray* tracks)     { fTracks = tracks; }
  void SetVerbose(Int_t verbose)               { fVerbose = verbose; };


 protected:

  CbmStsDigiScheme* fDigiScheme;  // STS digitisation scheme
  FairField*         fField;       // Magnetic field
  TClonesArray*     fMvdHits;     // MvdHit array
  TClonesArray*     fStsHits;     // StsHit array
  TClonesArray*     fTracks;      // StsTrack array
  Int_t             fVerbose;     // Verbosity level

 private:

  CbmStsTrackFinder(const CbmStsTrackFinder&);
  CbmStsTrackFinder& operator=(const CbmStsTrackFinder&);

  ClassDef(CbmStsTrackFinder,1);

};

#endif
