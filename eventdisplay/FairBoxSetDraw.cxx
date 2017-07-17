// -------------------------------------------------------------------------
// -----                        FairBoxSetDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "FairBoxSetDraw.h"
#include "FairRunAna.h"
#include "FairBoxSet.h"
#include "FairLogger.h"

#include "Riosfwd.h"
#include "TEveBoxSet.h"
#include "TEveManager.h"

#include <iostream>
using namespace std;

FairBoxSet* fq;    //!
Double_t fX, fY, fZ;

// -----   Default constructor   -------------------------------------------
FairBoxSetDraw::FairBoxSetDraw()
  : FairTask("FairBoxSetDraw",0),
    fVerbose(0),
    fList(NULL),
    fEventManager(NULL),
    fManager(NULL),
    fq(NULL),
    fX(0.3),
    fY(0.3),
    fZ(0.3),
    fTimeWindowPlus(0.),
    fTimeWindowMinus(0.),
    fStartTime(0.),
    fUseEventTime(kTRUE),
    fStartFunctor(),
    fStopFunctor()
{
}

// -----   Standard constructor   ------------------------------------------
FairBoxSetDraw::FairBoxSetDraw(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fVerbose(iVerbose),
    fList(NULL),
    fEventManager(NULL),
    fManager(NULL),
    fq(NULL),
    fX(0.3),
    fY(0.3),
    fZ(0.3),
    fTimeWindowPlus(0.),
    fTimeWindowMinus(0.),
    fStartTime(0.),
    fUseEventTime(kTRUE),
    fStartFunctor(),
    fStopFunctor()
{
}

// -------------------------------------------------------------------------
InitStatus FairBoxSetDraw::Init()
{
  if (fVerbose > 1) cout<<"FairBoxSetDraw::Init()"<<endl;

  fManager = FairRootManager::Instance();

  fList = (TClonesArray*) fManager->GetObject(GetName());
  if (fList == 0)
  {
    LOG(ERROR)<<"FairBoxSetDraw::Init() branch "<<GetName()<<" not found! Task will be deactivated"<<FairLogger::endl;
    SetActive(kFALSE);
    return kERROR;
  }
  if (fVerbose > 2) cout<<"FairBoxSetDraw::Init() get track list"<<fList<<endl;

  fEventManager = FairEventManager::Instance();
  if (fVerbose > 2) cout<< "FairBoxSetDraw::Init() get instance of FairEventManager"<<endl;

  fq = 0;
  fStartFunctor = new StopTime();
  fStopFunctor = new StopTime();

  return kSUCCESS;
}

// -------------------------------------------------------------------------
void FairBoxSetDraw::Exec(Option_t* /*option*/)
{
  if (!IsActive()) return;
  Reset();

  CreateBoxSet();

  if (FairRunAna::Instance()->IsTimeStamp())
  {
    fList->Clear();
    Double_t eventTime = fManager->GetEventTime();
    if (fUseEventTime) { fStartTime = eventTime - fTimeWindowMinus; }
    cout<<"EventTime: "<<eventTime<<" TimeWindow: "<<fStartTime<<" - "<<eventTime + fTimeWindowPlus<<endl;

    fList = fManager->GetData(GetName(), fStartFunctor, fStartTime, fStopFunctor, eventTime + fTimeWindowPlus); //fManager->GetEventTime() +
  }
  if (fVerbose > 1) cout<<GetName()<<" fList: "<<fList->GetEntries()<<endl;

  TObject* p;
  for (Int_t i = 0; i < fList->GetEntriesFast(); i++)
  {
      p = fList->At(i);
      AddBoxes(fq, p, i);
  }

  gEve->AddElement(fq, fEventManager);

  //gEve->Redraw3D(kFALSE);
}

void FairBoxSetDraw::AddBoxes(FairBoxSet* set, TObject* obj, Int_t i)
{
  TVector3 point = GetVector(obj);

  set->AddBox(point.X(),point.Y(),point.Z());
  set->DigitValue(GetValue(obj, i));
  if (fVerbose > 2) cout<< "FairBoxSetDraw::Init() Add point "<<i<<": "<<point.X()<<" "<<point.Y()<<" "<<point.Z()<<" "<<endl;
}


Int_t FairBoxSetDraw::GetValue(TObject* /*obj*/, Int_t i)
{
  return i;
}

FairBoxSet* FairBoxSetDraw::CreateBoxSet()
{
  FairBoxSet* aBoxSet = new FairBoxSet(this, GetName());
  aBoxSet->Reset(FairBoxSet::kBT_AABoxFixedDim, kFALSE, 64);
  aBoxSet->SetDefWidth(fX);
  aBoxSet->SetDefHeight(fY);
  aBoxSet->SetDefDepth(fZ);

  fq = aBoxSet;

  return aBoxSet;
}

void FairBoxSetDraw::SetTimeWindowMinus(Double_t val)
{
  fTimeWindowMinus = val;
}

void FairBoxSetDraw::SetTimeWindowPlus(Double_t val)
{
  fTimeWindowPlus = val;
}

// -----   Destructor   ----------------------------------------------------
FairBoxSetDraw::~FairBoxSetDraw()
{
}

// -------------------------------------------------------------------------
void FairBoxSetDraw::SetParContainers()
{
}

// -------------------------------------------------------------------------
/** Action after each event**/
void FairBoxSetDraw::Finish()
{
}

// -------------------------------------------------------------------------
void FairBoxSetDraw::Reset()
{
  if (fq != 0)
  {
    fq->Reset();
    gEve->RemoveElement(fq, fEventManager);
  }
}

ClassImp(FairBoxSetDraw)
