// -------------------------------------------------------------------------
// -----                        BmnHitDraw source file               -----
// -------------------------------------------------------------------------
#include "BmnHitDraw.h"
#include "FairHit.h"

#include "TEveManager.h"
#include "TGeoManager.h"

#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
BmnHitDraw::BmnHitDraw()
  : FairTask("BmnHitDraw", 0),
    fVerbose(0),
    fHitList(NULL),
    fEventManager(NULL),
    fq(NULL),
    fColor(0),
    fStyle(0)
{
}

// -----   Standard constructor   ------------------------------------------
BmnHitDraw::BmnHitDraw(const char* name, Color_t color ,Style_t mstyle,Int_t iVerbose)
  : FairTask(name, iVerbose),
    fVerbose(iVerbose),
    fHitList(NULL),
    fEventManager(NULL),
    fq(NULL),
    fColor(color),
    fStyle(mstyle)
{
}

// -------------------------------------------------------------------------
InitStatus BmnHitDraw::Init()
{
  if (fVerbose > 1)
      cout<<  "BmnHitDraw::Init()" << endl;

  fEventManager = FairEventManager::Instance();
  if (fVerbose > 2)
      cout<<"BmnHitDraw::Init() get instance of FairEventManager"<<endl;

  bmn_hit_tree = new TChain("cbmsim");
  bmn_hit_tree->Add(fEventManager->source_file_name);

  if (bmn_hit_tree->GetFile() == NULL)
  {
    cout<<"BmnHitDraw::Init() file with 'cbmsim' tree \""<<fEventManager->source_file_name<<"\" not found! Task will be deactivated "<<endl;
    SetActive(kFALSE);
    return kERROR;
  }

  bmn_hit_tree->SetBranchAddress(GetName(), &fHitList);
  if (!bmn_hit_tree->GetBranchStatus(GetName()))
  {
    cout<<"BmnHitDraw::Init() branch \""<<GetName()<<"\" not found in file ("<<fEventManager->source_file_name<<")! Task will be deactivated "<<endl;
    SetActive(kFALSE);
    return kERROR;
  }

  if (fVerbose > 2)
      cout<<"BmnHitDraw::Init() get hit list" <<fHitList<<endl;

  //cout<<endl<<"fEntryCount "<<fEventManager->fEntryCount<<" Event Count "<<bmn_hit_tree->GetEntries()<<endl;
  if (fEventManager->fEntryCount == 0)
      fEventManager->fEntryCount = bmn_hit_tree->GetEntries();
  else
      fEventManager->fEntryCount = TMath::Min(fEventManager->fEntryCount, bmn_hit_tree->GetEntries());

  fq = 0;

  return kSUCCESS;
}

void BmnHitDraw::Exec(Option_t* option)
{
    if (!IsActive()) return;

    Reset();

    Int_t event_number = fEventManager->GetCurrentEvent();
    bmn_hit_tree->GetEntry(event_number);

    cout<<"Event number: "<<event_number<<". "<<GetName()<<" count: "<<fHitList->GetEntries()<<". hit count: "<<fHitList->GetEntries()<<"."<<endl;

    Int_t npoints = fHitList->GetEntriesFast();
    TEvePointSet* q = new TEvePointSet(GetName(), npoints, TEvePointSelectorConsumer::kTVT_XYZ);

    q->SetOwnIds(kTRUE);
    q->SetMarkerColor(fColor);
    q->SetMarkerSize(1.5);
    q->SetMarkerStyle(fStyle);

    for (Int_t i = 0; i < npoints; i++)
    {
      TObject* p = (TObject*)fHitList->At(i);
      if(p != 0)
      {
          TVector3 vec(GetVector(p));
          q->SetNextPoint(vec.X(),vec.Y(), vec.Z());
          q->SetPointId(GetValue(p, i));
          //cout<<"VEC X: "<<vec.X()<<" Y:"<<vec.Y()<<" Z:"<<vec.Z()<<endl;
      }
    }

    if (fEventManager->EveRecoPoints == NULL)
    {
        fEventManager->EveRecoPoints = new TEveElementList("Reco points");
        gEve->AddElement(fEventManager->EveRecoPoints, fEventManager);
        fEventManager->EveRecoPoints->SetRnrState(kFALSE);
    }

    gEve->AddElement(q, fEventManager->EveRecoPoints);

    gEve->Redraw3D(kFALSE);

    fq = q;
}

TObject* BmnHitDraw::GetValue(TObject* obj,Int_t i)
{
  return new TNamed(Form("Point %d", i),"");
}

TVector3 BmnHitDraw::GetVector(TObject* obj)
{
  FairHit* p = (FairHit*)obj;
  return TVector3(p->GetX(), p->GetY(), p->GetZ());
}

// -----   Destructor   ----------------------------------------------------
BmnHitDraw::~BmnHitDraw()
{
}

// -------------------------------------------------------------------------
void BmnHitDraw::SetParContainers()
{
}

/** Action after each event**/
void BmnHitDraw::Finish()
{
}

// -------------------------------------------------------------------------
void BmnHitDraw::Reset()
{
  fHitList->Delete();

  if (fq != 0)
  {
    fq->Reset();

    gEve->RemoveElement(fq, fEventManager->EveRecoPoints);
  }
}

ClassImp(BmnHitDraw);
