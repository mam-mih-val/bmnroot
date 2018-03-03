/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *              GNU Lesser General Public Licence (LGPL) version 3,             *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                        MpdBoxSetDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------
#include "MpdBoxSetDraw.h"

#include "MpdBoxSet.h"                 // for MpdBoxSet
#include "MpdEventManager.h"           // for MpdEventManager
#include "FairRootManager.h"            // for FairRootManager
#include "FairRunAna.h"                 // for FairRunAna
#include "FairTSBufferFunctional.h"     // for StopTime
#include "FairTimeStamp.h"              // for FairTimeStamp

#include <iosfwd>                       // for ostream
#include "TClonesArray.h"               // for TClonesArray
#include "TEveBoxSet.h"
#include "TEveManager.h"                // for TEveManager, gEve
#include "TVector3.h"                   // for TVector3

#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream, etc

using std::cout;
using std::endl;

MpdBoxSet* fq;    //!
Double_t fX, fY, fZ;

// -----   Default constructor   -------------------------------------------
MpdBoxSetDraw::MpdBoxSetDraw()
  : FairTask("MpdBoxSetDraw",0),
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
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
MpdBoxSetDraw::MpdBoxSetDraw(const char* name, Int_t iVerbose)
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
InitStatus MpdBoxSetDraw::Init()
{
  if(fVerbose>1) {
    cout<<  "MpdBoxSetDraw::Init()" << endl;
  }
  fManager = FairRootManager::Instance();

  fList = static_cast<TClonesArray*>(FairRootManager::Instance()->GetObject(GetName()));
  //std::cout << fList << std::endl;
  if (fList==0) {
    cout << "MpdBoxSetDraw::Init()  branch " << GetName() << " Not found! Task will be deactivated "<< endl;
    SetActive(kFALSE);
    return kERROR;
  }
  if(fVerbose>2) {
    cout<<  "MpdBoxSetDraw::Init() get track list" <<  fList<< endl;
  }
  fEventManager =MpdEventManager::Instance();
  if(fVerbose>2) {
    cout<<  "MpdBoxSetDraw::Init() get instance of MpdEventManager " << endl;
  }
  fq=0;

  fStartFunctor = new StopTime();
  fStopFunctor = new StopTime();

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void MpdBoxSetDraw::Exec(Option_t* /*option*/)
{
  if(IsActive()) {
    TObject* p;
    Reset();
    //  cout<<  "MpdBoxSetDraw::Init() Exec! " << fList->GetEntriesFast() << endl;
    CreateBoxSet();
    if (FairRunAna::Instance()->IsTimeStamp()) {
      fList->Clear();
      Double_t eventTime = FairRootManager::Instance()->GetEventTime();
      if (fUseEventTime) { fStartTime = eventTime - fTimeWindowMinus; }
      cout << "EventTime: " << eventTime << " TimeWindow: " << fStartTime << " - " << eventTime + fTimeWindowPlus << std::endl;

      fList = FairRootManager::Instance()->GetData(GetName(), fStartFunctor, fStartTime, fStopFunctor, eventTime + fTimeWindowPlus); //FairRootManager::Instance()->GetEventTime() +

    }

    //fList = (TClonesArray *)fManager->GetObject(GetName());
    if (fVerbose > 1) {
      std::cout << GetName() << " fList: " << fList->GetEntries() << std::endl;
    }
    for (Int_t i=0; i<fList->GetEntriesFast(); ++i) {
      p=fList->At(i);
      if (fVerbose > 2) {
//        FairTimeStamp* data = (FairTimeStamp*)p;
//        cout << "TimeStamp: " <<  data->GetTimeStamp() << std::endl;
      }
      AddBoxes(fq, p, i);
    }
    gEve->AddElement(fq, fEventManager );
    gEve->Redraw3D(kFALSE);
  }
}

void MpdBoxSetDraw::AddBoxes(MpdBoxSet* set, TObject* obj, Int_t i)
{
  TVector3 point = GetVector(obj);
  set->AddBox(point.X(),point.Y(),point.Z());
  set->DigitValue(GetValue(obj, i));
  if(fVerbose>2) {
    cout<<  "MpdBoxSetDraw::Init() Add point " << i << ": " <<point.X()<< " "<<point.Y()<< " "<< point.Z()<< " " << endl;
  }
}


Int_t MpdBoxSetDraw::GetValue(TObject* /*obj*/, Int_t i)
{
  return i;
}

MpdBoxSet* MpdBoxSetDraw::CreateBoxSet()
{
  MpdBoxSet* aBoxSet = new MpdBoxSet(this, GetName());
  aBoxSet->Reset(MpdBoxSet::kBT_AABoxFixedDim, kFALSE, 64);
  aBoxSet->SetDefWidth(fX);
  aBoxSet->SetDefHeight(fY);
  aBoxSet->SetDefDepth(fZ);
  fq = aBoxSet;

  return aBoxSet;
}

void MpdBoxSetDraw::SetTimeWindowMinus(Double_t val)
{
  fTimeWindowMinus = val;
}

void MpdBoxSetDraw::SetTimeWindowPlus(Double_t val)
{
  fTimeWindowPlus = val;
}

// -----   Destructor   ----------------------------------------------------
MpdBoxSetDraw::~MpdBoxSetDraw()
{
}
// -------------------------------------------------------------------------
void MpdBoxSetDraw::SetParContainers()
{

}
// -------------------------------------------------------------------------
/** Action after each event**/
void MpdBoxSetDraw::Finish()
{
}
// -------------------------------------------------------------------------
void MpdBoxSetDraw::Reset()
{
  if(fq!=0) {
    fq->Reset();
    gEve->RemoveElement(fq, fEventManager );
  }
}


ClassImp(MpdBoxSetDraw)


