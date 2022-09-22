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
#include <TStopwatch.h>

static Double_t workTime = 0.0;

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmFindPrimaryVertex::CbmFindPrimaryVertex() 
  : FairTask("CbmFindPrimaryVertex"),
    fFinder(NULL),
    fTracks(NULL),
    fPrimVert(NULL),
    fTrBranch("") //AZ 
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmFindPrimaryVertex::CbmFindPrimaryVertex(CbmPrimaryVertexFinder* pvFinder)
  : FairTask("CbmFindPrimaryVertex"),
    fFinder(pvFinder),
    fTracks(NULL),
    fPrimVert(NULL),
    fTrBranch("") //AZ 
{
}
// -------------------------------------------------------------------------


// -----  Constructor with name and title  ---------------------------------
CbmFindPrimaryVertex::CbmFindPrimaryVertex(const char* name, 
					   const char* title, 
					   CbmPrimaryVertexFinder* finder) 
  : FairTask(name),
    fFinder(finder),
    fTracks(NULL),
    fPrimVert(NULL),
    fTrBranch("") //AZ 
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
  //AZ fTracks = (TClonesArray*) ioman->GetObject("StsTrack");
  if (fTrBranch == "") fTracks = (TClonesArray*) ioman->GetObject("StsTrack"); //AZ
  else fTracks = (TClonesArray*) ioman->GetObject(fTrBranch); //AZ
  if ( ! fTracks) {
    cout << "-W- CbmFindPrimaryVertex::Init: No STSTrack array!"
	 << endl;
    return kERROR;
  }

  // Create and register CbmVertex object
  fPrimVert = new CbmVertex("Primary Vertex", "Global");
  ioman->Register("PrimaryVertex.", "Global", fPrimVert, kTRUE);


  // Call the Init method of the vertex finder
  fFinder->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmFindPrimaryVertex::Exec(Option_t* opt) {

  TStopwatch sw;
  sw.Start();
  
  // Reset primary vertex
  fPrimVert->Reset();

  // Call find method of vertex finder
  Int_t iFind = fFinder->FindPrimaryVertex(fTracks, fPrimVert);
  if (iFind) cout << "-W- CbmFindPrimaryVertex::Exec: "
		  << "Vertex finder returned " << iFind << endl;

  if (fVerbose > 0) {
    // Print vertex
    cout << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "-I-             Primary Vertex Finder               -I-" << endl;
    fPrimVert->Print();
    cout << "-------------------------------------------------------" << endl;
  }

  sw.Stop();
  workTime += sw.RealTime();
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmFindPrimaryVertex::Finish() {
  fPrimVert->Reset();
  printf("Work time of CbmFindPrimaryVertex: %4.2f sec.\n", workTime);
}
// -------------------------------------------------------------------------



ClassImp(CbmFindPrimaryVertex)
