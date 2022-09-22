// -------------------------------------------------------------------------
// -----                        FairMCModuleDraw source file           -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include "FairMCModuleDraw.h"
#include "FairRootManager.h"
#include "BmnZdcPoint.h"
#include "CbmMCTrack.h"
#include "FairLogger.h"

#include "TEvePointSet.h"
#include "TString.h"

#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
FairMCModuleDraw::FairMCModuleDraw()
  : FairTask("FairMCModuleDraw", 0),
    fVerbose(0),
    fPointList(NULL),
    fEventManager(NULL),
    fColor(0),
    fStyle(0)
{}

// -----   Standard constructor   ------------------------------------------
FairMCModuleDraw::FairMCModuleDraw(const char* name, Color_t color ,Style_t mstyle,Int_t iVerbose)
  : FairTask(name, iVerbose),
    fVerbose(iVerbose),
    fPointList(NULL),
    fEventManager(NULL),
    fColor(color),
    fStyle(mstyle)
{
}

// -------------------------------------------------------------------------
InitStatus FairMCModuleDraw::Init()
{
  if (fVerbose > 1) cout<<"FairMCModuleDraw::Init()"<<endl;

  FairRootManager* fManager = FairRootManager::Instance();

  fPointList = (TClonesArray*) fManager->GetObject(GetName());
  if(fPointList == 0)
  {
    LOG(error)<<"FairMCModuleDraw::Init() branch "<< GetName()<<" not found! Task will be deactivated";
    SetActive(kFALSE);
    return kERROR;
  }
  if (fVerbose > 2) cout<<"FairMCModuleDraw::Init() get point list"<<fPointList<<endl;

  fMCTracks = (TClonesArray*) fManager->GetObject("MCTrack");

  fEventManager = MpdEventManager::Instance();
  if (fVerbose > 2) cout<< "FairMCModuleDraw::Init() get instance of MpdEventManager " << endl;

  // create array for ZDC modules' visibility
  fEventManager->isZDCModule = new bool[104];

  return kSUCCESS;
}

void FairMCModuleDraw::Exec(Option_t* option)
{
  if (!IsActive())
    return
  Reset();

  bool* pZDCModuleNumber = fEventManager->isZDCModule;
  Int_t npoints = fPointList->GetEntriesFast();
  for (int i = 0; i < npoints; i++)
  {
      BmnZdcPoint* p = (BmnZdcPoint*) fPointList->At(i);

      CbmMCTrack* pMCTrack = (CbmMCTrack*) fMCTracks->At(p->GetTrackID());
      if (!pMCTrack)
      {
          cout<<"MCTrack is NULL for selected index"<<endl;
          continue;
      }

      // filter only forward points
      Double32_t start_z = pMCTrack->GetStartZ();
      //cout<<"StartZ: "<<start_z<<cout<<endl;
      if (start_z < 1000.0)
          pZDCModuleNumber[p->GetCopyMother()] = true;
  }
}

TVector3 FairMCModuleDraw::GetVector(TObject* obj)
{
  FairMCPoint* p = (FairMCPoint*)obj;
  return TVector3(p->GetX(), p->GetY(), p->GetZ());
}

TObject* FairMCModuleDraw::GetValue(TObject* obj,Int_t i)
{
  return new TNamed(Form("Point %d", i),"");
}

// -----   Destructor   ----------------------------------------------------
FairMCModuleDraw::~FairMCModuleDraw()
{
    delete[] fEventManager->isZDCModule;
}

// -------------------------------------------------------------------------
void FairMCModuleDraw::SetParContainers()
{
}

/** Action after each event**/
void FairMCModuleDraw::Finish()
{
}

// -------------------------------------------------------------------------
void FairMCModuleDraw::Reset()
{
    bool* pZDCModuleNumber = fEventManager->isZDCModule;
    for (int i = 0; i < 104; i++)
        pZDCModuleNumber[i] = false;
}

ClassImp(FairMCModuleDraw);
