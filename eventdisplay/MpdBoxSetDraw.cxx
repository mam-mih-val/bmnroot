// -------------------------------------------------------------------------
// -----                        MpdBoxSetDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------
#include "MpdBoxSetDraw.h"
#include "MpdBoxSet.h"

#include "FairRunAna.h"

#include "TEveManager.h"    // for TEveManager, gEve

#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
MpdBoxSetDraw::MpdBoxSetDraw()
  : FairTask("MpdBoxSetDraw", 0),
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
    isRedraw(kFALSE),
    fStartFunctor(),
    fStopFunctor()
{
}

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
    isRedraw(kFALSE),
    fStartFunctor(),
    fStopFunctor()
{
}

// -----   Destructor   ----------------------------------------------------
MpdBoxSetDraw::~MpdBoxSetDraw()
{
}

InitStatus MpdBoxSetDraw::Init()
{
    if (fVerbose > 1) cout<<"MpdBoxSetDraw::Init()"<<endl;

    fManager = FairRootManager::Instance();

    fList = (TClonesArray*) FairRootManager::Instance()->GetObject(GetName());
    if (fList == 0)
    {
        cout<<"MpdBoxSetDraw::Init()  branch "<<GetName()<<" not found! Task will be deactivated"<<endl;
        SetActive(kFALSE);
        return kERROR;
    }
    if (fVerbose > 2) cout<<"MpdBoxSetDraw::Init() get track list "<<fList<<endl;

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > 2) cout<<"MpdBoxSetDraw::Init() get instance of MpdEventManager"<<endl;

    fq = 0;
    fStartFunctor = new StopTime();
    fStopFunctor = new StopTime();

    return kSUCCESS;
}
// -------------------------------------------------------------------------
void MpdBoxSetDraw::Exec(Option_t* /*option*/)
{
    if (!IsActive())
        return;

    Reset();

    CreateBoxSet();
    if (FairRunAna::Instance()->IsTimeStamp())
    {
        fList->Clear();
        Double_t eventTime = fManager->GetEventTime();
        if (fUseEventTime)
            fStartTime = eventTime - fTimeWindowMinus;
        cout<<"EventTime: "<<eventTime<<" TimeWindow: "<<fStartTime<<" - "<<eventTime + fTimeWindowPlus<<endl;

        fList = fManager->GetData(GetName(), fStartFunctor, fStartTime, fStopFunctor, eventTime + fTimeWindowPlus); //FairRootManager::Instance()->GetEventTime() +
    }
    if (fVerbose > 1) cout<<GetName()<<" fList: "<<fList->GetEntries()<<endl;

    for (Int_t i = 0; i < fList->GetEntriesFast(); i++)
    {
        TObject* p = fList->At(i);
        AddBoxes(fq, p, i);
    }

    gEve->AddElement(fq, fEventManager);
    if (isRedraw) gEve->Redraw3D();
}

void MpdBoxSetDraw::AddBoxes(MpdBoxSet* set, TObject* obj, Int_t i)
{
    TVector3 point = GetVector(obj);
    set->AddBox(point.X(), point.Y(), point.Z());
    set->DigitValue(GetValue(obj, i));
    if (fVerbose > 2) cout<<"MpdBoxSetDraw::Init() Add point "<<i<<": "<<point.X()<<" "<<point.Y()<<" "<<point.Z()<<" "<<endl;
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

void MpdBoxSetDraw::SetParContainers()
{
}

/** Action after each event**/
void MpdBoxSetDraw::Finish()
{
}

void MpdBoxSetDraw::Reset()
{
    if (fq != 0)
    {
        fq->Reset();
        gEve->RemoveElement(fq, fEventManager);
    }
}

ClassImp(MpdBoxSetDraw)
