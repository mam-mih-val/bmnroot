// -------------------------------------------------------------------------
// -----                 CbmFindPrimaryVertex source file              -----
// -----                  Created 28/11/05  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmFindPrimaryVertex.h"

#include "CbmPrimaryVertexFinder.h"
#include "CbmVertex.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmFindPrimaryVertex::CbmFindPrimaryVertex() 
  : FairTask(),
    fFinder(NULL),
    fTracks(NULL),
    fPrimVert(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmFindPrimaryVertex::CbmFindPrimaryVertex(CbmPrimaryVertexFinder* pvFinder)
  : FairTask(),
    fFinder(pvFinder),
    fTracks(NULL),
    fPrimVert(NULL)
{
}
// -------------------------------------------------------------------------



// -----  Constructor with name and title  ---------------------------------
CbmFindPrimaryVertex::CbmFindPrimaryVertex(const char* name, 
					   const char* title, 
					   CbmPrimaryVertexFinder* finder) 
  : FairTask(),
    fFinder(finder),
    fTracks(NULL),
    fPrimVert(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmFindPrimaryVertex::~CbmFindPrimaryVertex() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmFindPrimaryVertex::Init() {

  // Check for vertex finder
  if ( ! fFinder) {
    cout << "-E- CbmFindPrimaryVertex::Init : "
	 << "No vertex finder selected! " << endl;
    return kERROR;
  }

  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmFindPrimaryVertex::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get CbmStsTrack array
  fTracks = (TClonesArray*) ioman->GetObject("StsTrack");
  if ( ! fTracks) {
    cout << "-W- CbmFindPrimaryVertex::Init: No STSTrack array!"
	 << endl;
    return kERROR;
  }

  // Create and register CbmVertex object
  fPrimVert = new CbmVertex("Primary Vertex", "Global");
  ioman->Register("PrimaryVertex", "Global", fPrimVert, kTRUE);


  // Call the Init method of the vertex finder
  fFinder->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmFindPrimaryVertex::Exec(Option_t* opt) {

  // Reset primary vertex
  fPrimVert->Reset();

  // Call find method of vertex finder
  Int_t iFind = fFinder->FindPrimaryVertex(fTracks, fPrimVert);
  if (iFind) cout << "-W- CbmFindPrimaryVertex::Exec: "
		  << "Vertex finder returned " << iFind << endl;

  // Print vertex
  cout << endl;
  cout << "-------------------------------------------------------" << endl;
  cout << "-I-             Primary Vertex Finder               -I-" << endl;
  fPrimVert->Print();
  cout << "-------------------------------------------------------" << endl;

}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmFindPrimaryVertex::Finish() {
  fPrimVert->Reset();
}
// -------------------------------------------------------------------------



ClassImp(CbmFindPrimaryVertex)
