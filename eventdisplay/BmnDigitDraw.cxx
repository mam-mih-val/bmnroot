﻿// -------------------------------------------------------------------------
// -----                        BmnDigitDraw source file               -----
// -------------------------------------------------------------------------
#include "BmnDigitDraw.h"
#include "BmnDchHit.h"
#include "BmnHitFinderRun1.h"
#include "RawDataConverter.h"

#include "TEveManager.h"
#include "TGeoManager.h"

#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
BmnDigitDraw::BmnDigitDraw()
  : FairTask("BmnDigitDraw", 0),
    fVerbose(0),
    fDigitList(NULL),
    fHitList(NULL),
    fEventManager(NULL),
    fq(NULL),
    fColor(0),
    fStyle(0),
    fDetectorID(0)
{
}

// -----   Standard constructor   ------------------------------------------
BmnDigitDraw::BmnDigitDraw(const char* name, Int_t det_id, Color_t color ,Style_t mstyle,Int_t iVerbose)
  : FairTask(name, iVerbose),
    fVerbose(iVerbose),
    fDigitList(NULL),
    fHitList(NULL),
    fEventManager(NULL),
    fq(NULL),
    fColor(color),
    fStyle(mstyle),
    fDetectorID(det_id)
{
}

// -------------------------------------------------------------------------
InitStatus BmnDigitDraw::Init()
{
  if (fVerbose > 1)
      cout<<  "BmnDigitDraw::Init()" << endl;

  if (fDetectorID == 0)
  {
    cout<<"BmnDigitDraw::Init() detector ID hasn't been set! Task will be deactivated"<<endl;
    SetActive(kFALSE);
    return kERROR;
  }

  fEventManager = FairEventManager::Instance();
  if (fVerbose > 2)
      cout<<"BmnDigitDraw::Init() get instance of FairEventManager"<<endl;

  bmn_digit_tree = new TChain("BMN_DIGIT");
  bmn_digit_tree->Add(fEventManager->source_file_name);

  if (bmn_digit_tree->GetFile() == NULL)
  {
    cout<<"BmnDigitDraw::Init() file with 'BMN_DIGIT' tree \""<<fEventManager->source_file_name<<"\" not found! Task will be deactivated "<<endl;
    SetActive(kFALSE);
    return kERROR;
  }

  bmn_digit_tree->SetBranchAddress(GetName(), &fDigitList);
  if (!bmn_digit_tree->GetBranchStatus(GetName()))
  {
    cout<<"BmnDigitDraw::Init() branch \""<<GetName()<<"\" not found in file ("<<fEventManager->source_file_name<<")! Task will be deactivated "<<endl;
    SetActive(kFALSE);
    return kERROR;
  }

  if (fVerbose > 2)
      cout<<"BmnDigitDraw::Init() get digit list" <<fDigitList<<endl;

  //cout<<endl<<"fEntryCount "<<fEventManager->fEntryCount<<" Event Count "<<bmn_digit_tree->GetEntries()<<endl;
  if (fEventManager->fEntryCount == 0)
      fEventManager->fEntryCount = bmn_digit_tree->GetEntries();
  else
      fEventManager->fEntryCount = TMath::Min(fEventManager->fEntryCount, bmn_digit_tree->GetEntries());

  fq = 0;
  fHitList = new TClonesArray("BmnDchHit");

  return kSUCCESS;
}

void BmnDigitDraw::Exec(Option_t* option)
{
  if (IsActive())
  {
    Reset();

    Int_t event_number = fEventManager->GetCurrentEvent();
    bmn_digit_tree->GetEntry(event_number);

    switch (fDetectorID)
    {
        // MWPC digits
        case 1:
        {
            RawDataConverter raw_converter;
            for (int i = 1; i < 4; i++)
            {
                // get MWPC position
                TGeoVolume* pVolume = gGeoManager->GetVolume("cave");
                if (pVolume != NULL)
                {
                    TString node_name = TString::Format("mwpc%d_0", i);
                    TGeoNode* pFirstNode = pVolume->FindNode(node_name);
                    if (pFirstNode != NULL)
                    {
                        TGeoMatrix* pMatrix = pFirstNode->GetMatrix();
                        //cout<<"mwpc_name: "<<node_name<<" X:"<<pMatrix->GetTranslation()[0]<<" Y:"<<pMatrix->GetTranslation()[1]<<" Z:"<<pMatrix->GetTranslation()[2]<<endl;
                        raw_converter.SetMwpcPosition(i, TVector3(pMatrix->GetTranslation()[0], pMatrix->GetTranslation()[1], pMatrix->GetTranslation()[2]));
                    }
                    else
                        cout<<"MWPC detector ("<<node_name<<") wasn't found. Default MWPC position is used for visual hits"<<endl;
                }
                else
                    cout<<"Cave volume wasn't found. Default MWPC position is used for visual hits"<<endl;
            }

            raw_converter.MwpcDigits2MwpcHits(fDigitList, fHitList);
            break;
        }
        // DCH digits
        case 2:
            ProcessDchDigits(fDigitList, fHitList);
            break;
    }

    cout<<"Event number: "<<event_number<<". "<<GetName()<<" count: "<<fDigitList->GetEntries()<<". hit count: "<<fHitList->GetEntries()<<"."<<endl;

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
}

TObject* BmnDigitDraw::GetValue(TObject* obj,Int_t i)
{
  return new TNamed(Form("Point %d", i),"");
}

TVector3 BmnDigitDraw::GetVector(TObject* obj)
{
  FairHit* p = (FairHit*)obj;
  return TVector3(p->GetX(), p->GetY(), p->GetZ());
}

// -----   Destructor   ----------------------------------------------------
BmnDigitDraw::~BmnDigitDraw()
{
}

// -------------------------------------------------------------------------
void BmnDigitDraw::SetParContainers()
{
}

/** Action after each event**/
void BmnDigitDraw::Finish()
{
}

// -------------------------------------------------------------------------
void BmnDigitDraw::Reset()
{
  fHitList->Delete();

  if (fq != 0)
  {
    fq->Reset();

    gEve->RemoveElement(fq, fEventManager->EveRecoPoints);
  }
}

ClassImp(BmnDigitDraw);
