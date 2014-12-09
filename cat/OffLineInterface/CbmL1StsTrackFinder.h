/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  CbmL1StsTrackFinder header
 *
 *====================================================================
 */

#ifndef CBML1STSTRACKFINDER
#define CBML1STSTRACKFINDER 1


#include "CbmL1.h"

#include "CbmStsTrackFinder.h"

class TClonesArray;


class CbmL1StsTrackFinder : public CbmStsTrackFinder
{

 public:

  /** Default constructor **/
  CbmL1StsTrackFinder();


  /** Destructor **/
  virtual ~CbmL1StsTrackFinder();


  /** Initialisation **/
  virtual void Init();


  /** Track finding algorithm
   **/
  virtual Int_t DoFind();

 private:

  ClassDef(CbmL1StsTrackFinder,1);

};


#endif
