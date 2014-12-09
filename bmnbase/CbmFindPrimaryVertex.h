// -------------------------------------------------------------------------
// -----                 CbmFindPrimaryVertex header file              -----
// -----                  Created 28/11/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmFindPrimaryVertex
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for PV finding.
 ** Input: TClonesArray of CbmStsTracks (later CbmGlobalTracks)
 ** Output: CbmVertex
 **
 ** Uses as vertex finding algorithm classes derived from 
 ** CbmPrimaryVertexFinder.
 **/


#ifndef CBMFINDPRIMARYVERTEX_H
#define CBMFINDPRIMARYVERTEX_H 1


#include "FairTask.h"

class TClonesArray;
class CbmPrimaryVertexFinder;
class CbmVertex;



class CbmFindPrimaryVertex : public FairTask
{

 public:

  /** Default constructor **/
  CbmFindPrimaryVertex();


  /** Standard constructor
   *@param pvFinder  Pointer to concrete vertex finder
   **/
  CbmFindPrimaryVertex(CbmPrimaryVertexFinder* pvFinder);
  

  /** Constructor with name and title
   **
   *@param name      Name of task
   *@param title     Title of task
   *@param pvFinder  Pointer to vertex finder concrete object
   **/
  CbmFindPrimaryVertex(const char* name, const char* title,
		       CbmPrimaryVertexFinder* pvFinder);


  /** Destructor **/
  virtual ~CbmFindPrimaryVertex();


  /** Initialisation **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish **/
  virtual void Finish();


 private:

  CbmPrimaryVertexFinder* fFinder;
  TClonesArray*           fTracks;
  CbmVertex*              fPrimVert;


  CbmFindPrimaryVertex(const CbmFindPrimaryVertex&);
  CbmFindPrimaryVertex& operator=(const CbmFindPrimaryVertex&);

  ClassDef(CbmFindPrimaryVertex,1);

};

#endif


