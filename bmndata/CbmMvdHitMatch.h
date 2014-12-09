// -------------------------------------------------------------------------
// -----                   CbmMvdHitMatch header file                  -----
// -----                 Created 07/11/06  by V. Friese                -----
// -------------------------------------------------------------------------


/** CbmMvdHitMatch header file
 *@author Michael Deveaux <m.deveaux@gsi.de>
 **
 ** Former CbmStsMapsHitInfo
 ** Data class for connecting CbmMvdHit with MC information.
 ** Data level RECO
 **/


#ifndef CBMMVDHITMATCH_H
#define CBMMVDHITMATCH_H 1


#include "TObject.h"


class CbmMvdHitMatch: public TObject 
{

 public:

  /** Default constructor **/
  CbmMvdHitMatch();


  /** Constructor with all parameters **/
  CbmMvdHitMatch(Int_t fileNumber, Int_t eventNumber, Int_t trackId,
		 Int_t pointId, Int_t nMerged);


  /** Destructor **/
  virtual ~CbmMvdHitMatch();  


  /** Accessors **/
  Int_t  GetFileNumber()  const { return fFileNumber;  }
  Int_t  GetEventNumber() const { return fEventNumber; }
  Int_t  GetTrackId()     const { return fTrackId;     } 
  Int_t  GetPointId()     const { return fPointId;     }
  Int_t  GetNMerged()     const { return fNMerged;     }
  Bool_t IsFake()         const { 
    if ( fPointId == -1 ) return kTRUE;
    else                  return kFALSE;
  }


  /** Modifiers **/
  void AddMerged()  { fNMerged++; }


  /** Reset data members **/
  void Clear();


 private:

  /** Indicates the file to which the corresponding point belongs.
   ** -1    => no file (fake hit)
   **  0    => data file
   **  1    => background file (pileup)
   **/
  Int_t fFileNumber;


  /** Event number of the corresponding MCPoint.  Important to 
   ** find it in background files due to the random access.
   ** -1 => no corresponding MCPoint (fake hit)
   **  0 => current event in data file
   **  n => event in pileup file
   **/
  Int_t fEventNumber;


  /** Index of the corresponding MCTrack in its TClonesArray.
   ** Also true for background files.
   ** -1 for fake hits
   **/
  Int_t fTrackId;


  /** Index of the corresponding MCPoint in its TClonesArray.
   ** Also true for background files.
   ** -1 for fake hits
   **/
  Int_t fPointId;


  /** Number of other hits this one is merged with
   ** -1 => Hit was created without checking for merging
   **  0 => Hit was not merged
   **  n => Hit was merged with n other hits
   **/
  Int_t fNMerged;




  ClassDef(CbmMvdHitMatch,1);

};


#endif
