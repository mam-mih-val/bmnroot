// -------------------------------------------------------------------------
// -----                        FairPointSetDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------
#include "FairPointSetDraw.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TEveManager.h"
#include "TEveTreeTools.h"  // for TEvePointSelectorConsumer
#include "TString.h"        // for Form

#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
FairPointSetDraw::FairPointSetDraw()
  : FairTask("FairPointSetDraw", 0),
    fVerbose(0),
    fPointList(NULL),
    fEventManager(NULL),
    fq(NULL),
    fColor(0),
    fStyle(0)
{
}

// -----   Standard constructor   ------------------------------------------
FairPointSetDraw::FairPointSetDraw(const char* name, Color_t color, Style_t mstyle, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fVerbose(iVerbose),
    fPointList(NULL),
    fEventManager(NULL),
    fq(NULL),
    fColor(color),
    fStyle(mstyle)
{
}

// -------------------------------------------------------------------------
InitStatus FairPointSetDraw::Init()
{
  if (fVerbose > 0) cout<<"FairPointSetDraw::Init()"<<endl;

  FairRootManager* fManager = FairRootManager::Instance();

  fPointList = (TClonesArray*) fManager->GetObject(GetName());
  if (fPointList == 0)
  {
    LOG(ERROR)<<"FairPointSetDraw::Init() branch "<<GetName()<<" not found! Task will be deactivated"<<FairLogger::endl;
    SetActive(kFALSE);
  }
  if (fVerbose > 1) cout<<"FairPointSetDraw::Init() get point list "<<fPointList<<endl;

  fEventManager = FairEventManager::Instance();
  if (fVerbose > 1) cout<<"FairPointSetDraw::Init() get instance of FairEventManager"<<endl;

  fq = 0;

  return kSUCCESS;
}

void FairPointSetDraw::Exec(Option_t* /*option*/)
{
  if (fVerbose > 0) cout<<"FairPointSetDraw::Exec()"<<endl;
  if (!IsActive())
    return;

  Reset();

  Int_t npoints = fPointList->GetEntriesFast();
  if (fVerbose > 0) cout<<"FairPointSetDraw::Exec() the number of points is "<<fPointList->GetEntries()<<endl;

  TEvePointSet* q = new TEvePointSet(GetName(), npoints, TEvePointSelectorConsumer::kTVT_XYZ);
  q->SetOwnIds(kTRUE);
  q->SetMarkerColor(fColor);
  q->SetMarkerSize(1.5);
  q->SetMarkerStyle(fStyle);

  for (Int_t i = 0; i < npoints; i++)
  {
    TObject* p = (TObject*) fPointList->At(i);
    if (p != 0)
    {
        TVector3 vec(GetVector(p));
        q->SetNextPoint(vec.X(), vec.Y(), vec.Z());
        q->SetPointId(GetValue(p, i));
        if (fVerbose > 2) cout<<"FairPointSetDraw::Exec() add point to EVE set: "<<i<<endl;
    }
    else cout<<"CRITICAL ERROR: FairPointSetDraw::Exec() point is not TObject"<<endl;
  }

  fq = q;

  AddEveElementList();

  //gEve->Redraw3D(kFALSE);
}

TObject* FairPointSetDraw::GetValue(TObject* /*obj*/, Int_t i)
{
  return new TNamed(Form("Point %d", i),"");
}

// -----   Destructor   ----------------------------------------------------
FairPointSetDraw::~FairPointSetDraw()
{
}

// -------------------------------------------------------------------------
void FairPointSetDraw::SetParContainers()
{
}

/** Action after each event**/
void FairPointSetDraw::Finish()
{
}

// -------------------------------------------------------------------------
void FairPointSetDraw::Reset()
{
  if (fq != 0)
  {
    fq->Reset();
    RemoveEveElementList();
  }
}

ClassImp(FairPointSetDraw);
