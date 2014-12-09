#include "FairEventManagerEditor.h"

#include "FairEventManager.h"           // for FairEventManager
#include "FairRootManager.h"            // for FairRootManager
#include "FairRunAna.h"                 // for FairRunAna

#include "TChain.h"                     // for TChain
#include "TEveGValuators.h"             // for TEveGValuator
#include "TFile.h"                      // for TFile
#include "TGButton.h"                   // for TGCheckButton, TGTextButton
#include "TGLabel.h"                    // for TGLabel
#include "TGLayout.h"                   // for TGLayoutHints, etc
#include "TGNumberEntry.h"              // for TGNumberEntry, etc
#include "TGeoManager.h"                // for TGeoManager, gGeoManager
#include "TString.h"                    // for TString
#include "TEveManager.h"                // for TEveManager, gEve
#include "TEveElement.h"                // for TEveElement
#include "TPRegexp.h"

#include <stddef.h>                     // for NULL

#include <iostream>
using namespace std;

class TGWindow;
class TObject;

#define MAXE 12

//______________________________________________________________________________
// FairEventManagerEditor
//
// Specialization of TGedEditor for proper update propagation to
// TEveManager.

ClassImp(FairEventManagerEditor)


//______________________________________________________________________________
FairEventManagerEditor::FairEventManagerEditor(const TGWindow* p, Int_t width, Int_t height,
    UInt_t options, Pixel_t back)
  :TGedFrame(p, width, height, options | kVerticalFrame, back),
   fObject(0),
   fManager(FairEventManager::Instance()),
   fCurrentEvent(0),
   fCurrentPDG(0),
   fVizPri(0),
   fMinEnergy(0),
   fMaxEnergy(0),
   iCurrentEvent(-1)
{
  Init();
}

void FairEventManagerEditor::Init()
{
  // get input file
  FairRootManager* fRootManager=FairRootManager::Instance();
  TChain* chain =fRootManager->GetInChain();
  Int_t Entries= chain->GetEntriesFast();

  // create tab for event visualization
  MakeTitle("FairEventManager  Editor");
  TGVerticalFrame*      fInfoFrame= CreateEditorTabSubFrame("Event Info");
  title1 = new TGCompositeFrame(fInfoFrame, 250, 10,
      kVerticalFrame | kLHintsExpandX |
      kFixedWidth    | kOwnBackground);

  // display file name
  TString Infile= "file : ";
//  TFile* file =FairRunAna::Instance()->GetInputFile();
  TFile* file =FairRootManager::Instance()->GetInChain()->GetFile();
  Infile+=file->GetName();
  TGLabel* TFName=new TGLabel(title1, Infile.Data());
  title1->AddFrame(TFName);

  // display Run ID
  UInt_t RunId= FairRunAna::Instance()->getRunId();
  TString run= "Run Id : ";
  run += RunId;
  TGLabel* TRunId=new TGLabel(title1, run.Data());
  title1->AddFrame(TRunId);

  // display event count
  TString nevent= "No of events : ";
  nevent +=Entries ;
  TGLabel* TEvent=new TGLabel(title1, nevent.Data());
  title1->AddFrame(TEvent);

  // count of geometry nodes
  Int_t nodes= gGeoManager->GetNNodes();
  TString NNodes= "No. of Nodes : ";
  NNodes += nodes;
  TGLabel* NoNode=new TGLabel(title1, NNodes.Data());
  title1->AddFrame(NoNode);

  // setting textbox for event number
  TGHorizontalFrame* f = new TGHorizontalFrame(title1);
  TGLabel* l = new TGLabel(f, "Current Event:");
  f->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 2, 1, 1));
  fCurrentEvent = new TGNumberEntry(f, 0., 6, -1,
                                    TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative,
                                    TGNumberFormat::kNELLimitMinMax, 0, Entries-1);
  f->AddFrame(fCurrentEvent, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
  fCurrentEvent->Connect("ValueSet(Long_t)","FairEventManagerEditor", this, "SelectEvent()");
  title1->AddFrame(f);

  // textbox for time cutting
  TGHorizontalFrame* f2 = new TGHorizontalFrame(title1);
  TGLabel* EventTimeLabel = new TGLabel(f2, "Event Time: ");
  fEventTime = new TGLabel(f2,"");
  f2->AddFrame(EventTimeLabel);
  f2->AddFrame(fEventTime);
  title1->AddFrame(f2);

  // checkbox to display only primary particles in event
  fVizPri = new TGCheckButton(title1, "Primary Only");
  AddFrame(fVizPri, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
  fVizPri->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "DoVizPri()");
  title1->AddFrame(fVizPri);

  // textbox to display only particles with given PDG
  TGHorizontalFrame* f1 = new TGHorizontalFrame(title1);
  TGLabel* L1 = new TGLabel(f1, "Select PDG :");
  f1->AddFrame(L1, new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 1, 2, 1, 1));
  fCurrentPDG = new TGNumberEntry(f1, 0., 12, -1,
                                  TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber,
                                  TGNumberFormat::kNELNoLimits, 0, 1);
  f1->AddFrame(fCurrentPDG, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
  fCurrentPDG->Connect("ValueSet(Long_t)","FairEventManagerEditor", this, "SelectPDG()");
  title1->AddFrame(f1);


  // textbox for min energy cutting
  fMinEnergy = new TEveGValuator(title1, "Min Energy:", 90, 0);
  fMinEnergy->SetNELength(5);
  fMinEnergy->SetLabelWidth(80);
  fMinEnergy->Build();
  fMinEnergy->SetLimits(0, MAXE, 2501, TGNumberFormat::kNESRealOne);
  fMinEnergy->SetToolTip("Minimum energy of displayed tracks");
  fMinEnergy->SetValue(0);
  fMinEnergy->Connect("ValueSet(Double_t)", "FairEventManagerEditor", this, "MinEnergy()");
  title1->AddFrame(fMinEnergy, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
  fManager->SetMinEnergy(0);

  // textbox for max energy cutting
  fMaxEnergy = new TEveGValuator(title1, "Max Energy:", 90, 0);
  fMaxEnergy->SetNELength(5);
  fMaxEnergy->SetLabelWidth(80);
  fMaxEnergy->Build();
  fMaxEnergy->SetLimits(0, MAXE, 2501, TGNumberFormat::kNESRealOne);
  fMaxEnergy->SetToolTip("Maximum energy of displayed tracks");
  fMaxEnergy->SetValue(MAXE);
  fMaxEnergy->Connect("ValueSet(Double_t)", "FairEventManagerEditor", this, "MaxEnergy()");
  title1->AddFrame(fMaxEnergy, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
  fManager->SetMaxEnergy(MAXE);

  // button: whether show detector geometry or not
  TGCheckButton* fGeometry = new TGCheckButton(title1, "show geometry");
  title1->AddFrame(fGeometry, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
  fGeometry->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowGeometry(Bool_t)");
  fGeometry->SetOn();

  // button for switching from black to white background
  TGCheckButton* fBackground = new TGCheckButton(title1, "light background");
  title1->AddFrame(fBackground, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
  fBackground->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "SwitchBackground(Bool_t)");

  // group for displaying simulation and reconstruction data
  groupData = new TGGroupFrame(title1, "Show MC and reco data");
  groupData->SetTitlePos(TGGroupFrame::kCenter);

  TGHorizontalFrame* framePointsInfo = new TGHorizontalFrame(groupData);
  // button for show|hide MC points
  fShowMCPoints = new TGCheckButton(framePointsInfo, "MC points");
  framePointsInfo->AddFrame(fShowMCPoints, new TGLayoutHints(kLHintsNormal, 0,0,0,0));
  fShowMCPoints->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowMCPoints(Bool_t)");

  // button for show|hide reconstructed points
  fShowRecoPoints = new TGCheckButton(framePointsInfo, "Reco points");
  framePointsInfo->AddFrame(fShowRecoPoints, new TGLayoutHints(kLHintsRight, 0,0,1,0));
  fShowRecoPoints->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowRecoPoints(Bool_t)");
  groupData->AddFrame(framePointsInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));

  TGHorizontalFrame* frameTracksInfo = new TGHorizontalFrame(groupData);
  // button for show|hide MC tracks
  fShowMCTracks = new TGCheckButton(frameTracksInfo, "MC tracks");
  frameTracksInfo->AddFrame(fShowMCTracks, new TGLayoutHints(kLHintsNormal, 0,0,0,0));
  fShowMCTracks->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowMCTracks(Bool_t)");

  // button for show|hide reco tracks
  fShowRecoTracks = new TGCheckButton(frameTracksInfo, "Reco tracks");
  frameTracksInfo->AddFrame(fShowRecoTracks, new TGLayoutHints(kLHintsRight, 0,0,1,0));
  fShowRecoTracks->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowRecoTracks(Bool_t)");
  groupData->AddFrame(frameTracksInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));

  title1->AddFrame(groupData, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 3,15,1,1));

  // button for update of event visualization
  TGTextButton* fUpdate = new TGTextButton(title1, "Update");
  title1->AddFrame(fUpdate, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 3,15,1,1));
  fUpdate->Connect("Clicked()", "FairEventManagerEditor", this, "SelectEvent()");

  // add all frame above to "event info" tab
  fInfoFrame->AddFrame(title1, new TGLayoutHints(kLHintsTop, 0, 0, 2, 0));
}

//______________________________________________________________________________
void FairEventManagerEditor::MaxEnergy()
{
    fManager->SetMaxEnergy(fMaxEnergy->GetValue());
}
//______________________________________________________________________________
void FairEventManagerEditor::MinEnergy()
{
    fManager->SetMinEnergy(fMinEnergy->GetValue());
}

//______________________________________________________________________________
void FairEventManagerEditor::DoVizPri()
{
    if (fVizPri->IsOn())
        fManager->SetPriOnly(kTRUE);
    else
        fManager->SetPriOnly(kFALSE);
}
//______________________________________________________________________________
void FairEventManagerEditor::SelectPDG()
{
    fManager->SelectPDG(fCurrentPDG->GetIntNumber());
}

//______________________________________________________________________________
void FairEventManagerEditor::SetModel(TObject* obj)
{
    fObject = obj;
}

//______________________________________________________________________________
void FairEventManagerEditor::SelectEvent()
{
  int iNewEvent = fCurrentEvent->GetIntNumber();
  // exec event visualization of selected event
  fManager->GotoEvent(iNewEvent);

  if (iCurrentEvent == -1)
  {
      if (fManager->EveMCPoints == NULL)
          fShowMCPoints->SetDisabledAndSelected(kFALSE);
      if (fManager->EveMCTracks == NULL)
          fShowMCTracks->SetDisabledAndSelected(kFALSE);
      if (fManager->EveRecoPoints == NULL)
          fShowRecoPoints->SetDisabledAndSelected(kFALSE);
      if (fManager->EveRecoTracks == NULL)
          fShowRecoTracks->SetDisabledAndSelected(kFALSE);
  }

  if (iCurrentEvent != iNewEvent)
  {
      iCurrentEvent = iNewEvent;

      // display event time
      TString time;
      time.Form("%.2f", FairRootManager::Instance()->GetEventTime());
      time += " ns";
      fEventTime->SetText(time.Data());

      // new min and max energy limits given by event energy range
      fMinEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
      fMinEnergy->SetValue(fManager->GetEvtMinEnergy());
      MinEnergy();
      fMaxEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
      fMaxEnergy->SetValue(fManager->GetEvtMaxEnergy());
      MaxEnergy();
  }

  // update tab controls
  Update();

  // update all scenes
  fManager->fRPhiView->GetGLViewer()->UpdateScene(kTRUE);
  fManager->fRhoZView->GetGLViewer()->UpdateScene(kTRUE);
  fManager->fMulti3DView->GetGLViewer()->UpdateScene(kTRUE);
  fManager->fMultiRPhiView->GetGLViewer()->UpdateScene(kTRUE);
  fManager->fMultiRhoZView->GetGLViewer()->UpdateScene(kTRUE);
}

//______________________________________________________________________________
void FairEventManagerEditor::SwitchBackground(Bool_t is_on)
{
    gEve->GetViewers()->SwitchColorSet();
}

//______________________________________________________________________________
void FairEventManagerEditor::ShowGeometry(Bool_t is_show)
{
    gEve->GetGlobalScene()->SetRnrState(is_show);
    fManager->fRPhiGeomScene->SetRnrState(is_show);
    fManager->fRhoZGeomScene->SetRnrState(is_show);

    gEve->Redraw3D();
}

//______________________________________________________________________________
void FairEventManagerEditor::ShowMCPoints(Bool_t is_show)
{
    /*
    TEveElement::List_t matches;
    TPRegexp* regexp = new TPRegexp("(\\w+)Point\\b");
    Int_t numFound = fManager->FindChildren(matches, *regexp);
    if (numFound > 0)
    {
        for (TEveElement::List_i p = matches.begin(); p != matches.end(); ++p)
            (*p)->SetRnrState(is_show);
    }
    */

    TEveElement* points = fManager->FindChild("MC points");
    if (points == NULL)
    {
        cout<<"There is no information about MC points"<<endl;
        fShowMCPoints->SetOn(kFALSE);
        return;
    }

    points->SetRnrState(is_show);
    gEve->Redraw3D();
}

//______________________________________________________________________________
void FairEventManagerEditor::ShowMCTracks(Bool_t is_show)
{
    TEveElement* tracks = fManager->FindChild("MC tracks");
    if (tracks == NULL)
    {
        cout<<"There is no information about MC tracks"<<endl;
        fShowMCTracks->SetOn(kFALSE);
        return;
    }

    tracks->SetRnrState(is_show);
    gEve->Redraw3D();
}

//______________________________________________________________________________
void FairEventManagerEditor::ShowRecoPoints(Bool_t is_show)
{
    TEveElement* points = fManager->FindChild("Reco points");
    if (points == NULL)
    {
        cout<<"There is no information about reconstructed points"<<endl;
        fShowRecoPoints->SetOn(kFALSE);
        return;
    }

    points->SetRnrState(is_show);
    gEve->Redraw3D();
}

//______________________________________________________________________________
void FairEventManagerEditor::ShowRecoTracks(Bool_t is_show)
{
    TEveElement* tracks = fManager->FindChild("Reco tracks");
    if (tracks == NULL)
    {
        cout<<"There is no information about reconstructed tracks"<<endl;
        fShowRecoTracks->SetOn(kFALSE);
        return;
    }

    tracks->SetRnrState(is_show);
    gEve->Redraw3D();
}
