
/** CbmPVFinderKF
 *@author S.Gorbunov
 **
 **/


#ifndef CBMKFPVFINDERKF_H
#define CBMKFPVFINDERKF_H 1


#include "CbmPrimaryVertexFinder.h"


class CbmPVFinderKF : public CbmPrimaryVertexFinder
{

 public:

  /** Default constructor **/
  CbmPVFinderKF() { };


  /** Destructor **/
  ~CbmPVFinderKF() { };


  /** Execution of PV finding.
   *@param tracks   TClonesArray of CbmStsTracks
   *@param vertex   Primary vertex (output)
   **/
  Int_t FindPrimaryVertex(TClonesArray* tracks, CbmVertex* vertex);

  ClassDef(CbmPVFinderKF,1);

};

#endif
