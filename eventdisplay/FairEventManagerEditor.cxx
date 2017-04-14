/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#include "FairEventManagerEditor.h"
#include "FairRootManager.h"
#include "FairRunAna.h"

#include "TChain.h"
#include "TFile.h"
#include "TGLayout.h"
#include "TGeoManager.h"
#include "TString.h"
#include "TEveManager.h"
#include "TEveElement.h"
#include "TEvePointSet.h"
#include "TVector3.h"
#include "TObject.h"
#include "TGWindow.h"
#include <TGLViewer.h>
#include <TGLScenePad.h>
#include "TEveBrowser.h"
#include "TEveGedEditor.h"

#include <iostream>
using namespace std;

#define MAX_ENERGY 12

//______________________________________________________________________________
// FairEventManagerEditor
//
// Specialization of TGedEditor for proper update propagation to
// TEveManager.
ClassImp(FairEventManagerEditor);

//______________________________________________________________________________
FairEventManagerEditor::FairEventManagerEditor(const TGWindow* p, Int_t width, Int_t height, UInt_t options, Pixel_t back)
  : TGedFrame(p, width, height, options | kVerticalFrame, back),
   fObject(0),
   fManager(FairEventManager::Instance()),
   fCurrentEvent(0),
   fCurrentPDG(0),
   fVizPri(0),
   fMinEnergy(0),
   fMaxEnergy(0),
   iEventNumber(-1),
   iEventCount(-1),
   iThreadState(0)
{
    Init();
}

void FairEventManagerEditor::Init()
{
    TChain* chain = FairRootManager::Instance()->GetInChain();

    // create tab for event visualization
    MakeTitle("FairEventManager  Editor");
    TGVerticalFrame* fInfoFrame = CreateEditorTabSubFrame("Event Info");
    TGCompositeFrame* title1 = new TGCompositeFrame(fInfoFrame, 250, 10, kVerticalFrame | kLHintsExpandX | kFixedWidth | kOwnBackground);

    // display file name
    TString Infile = "File: ";
    Infile += chain->GetFile()->GetName();
    TGLabel* TFName = new TGLabel(title1, Infile.Data());
    title1->AddFrame(TFName);

    // textbox for Run ID and time cutting
    TGHorizontalFrame* f2 = new TGHorizontalFrame(title1);
    // Run Id
    UInt_t RunId = FairRunAna::Instance()->getRunId();
    TString run = TString::Format("Run Id: %d. ", RunId);
    TGLabel* TRunId = new TGLabel(f2, run.Data());
    // time cutting
    TGLabel* EventTimeLabel = new TGLabel(f2, " Event Time: ");
    fEventTime = new TGLabel(f2,"");
    f2->AddFrame(TRunId);
    f2->AddFrame(EventTimeLabel);
    f2->AddFrame(fEventTime);
    title1->AddFrame(f2);

    // display event count and count of geometry nodes
    iEventCount = chain->GetEntriesFast();
    Int_t nodes = gGeoManager->GetNNodes();
    TString nevent = TString::Format("No of events: %d. No. of nodes: %d", iEventCount, nodes);
    TGLabel* TEvent = new TGLabel(title1, nevent.Data());
    title1->AddFrame(TEvent);

    // setting textbox for event number
    TGHorizontalFrame* f = new TGHorizontalFrame(title1);
    TGLabel* l = new TGLabel(f, "Current Event:");
    f->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 2, 1, 1));
    fCurrentEvent = new TGNumberEntry(f, 0., 6, -1,
                        TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, iEventCount-1);
    f->AddFrame(fCurrentEvent, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
    fCurrentEvent->Connect("ValueSet(Long_t)","FairEventManagerEditor", this, "SelectEvent()");
    if (iEventCount< 1)
        fCurrentEvent->SetState(kFALSE);

    //Button for save image (EVE screenshot)
    fSave = new TGPictureButton(f, gClient->GetPicture("save.xpm"), 5);
    f->AddFrame(fSave, new TGLayoutHints(kLHintsLeft| kLHintsCenterY, 1, 2, 1, 1));
    fSave->Connect("Clicked()", "FairEventManagerEditor", this, "SaveImage()");
    title1->AddFrame(f);

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
                      TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits, 0, 1);
    f1->AddFrame(fCurrentPDG, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
    fCurrentPDG->Connect("ValueSet(Long_t)","FairEventManagerEditor", this, "SelectPDG()");
    title1->AddFrame(f1);

    // textbox for min energy cutting
    fMinEnergy = new TEveGValuator(title1, "Min Energy:", 90, 0);
    fMinEnergy->SetNELength(5);
    fMinEnergy->SetLabelWidth(80);
    fMinEnergy->Build();
    fMinEnergy->SetLimits(0, MAX_ENERGY, 2501, TGNumberFormat::kNESRealOne);
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
    fMaxEnergy->SetLimits(0, MAX_ENERGY, 2501, TGNumberFormat::kNESRealOne);
    fMaxEnergy->SetToolTip("Maximum energy of displayed tracks");
    fMaxEnergy->SetValue(MAX_ENERGY);
    fMaxEnergy->Connect("ValueSet(Double_t)", "FairEventManagerEditor", this, "MaxEnergy()");
    title1->AddFrame(fMaxEnergy, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
    fManager->SetMaxEnergy(MAX_ENERGY);

    fGeometryFrame = new TGHorizontalFrame(title1);
    // button: whether show detector geometry or not
    fGeometry = new TGCheckButton(fGeometryFrame, "show geometry");
    fGeometryFrame->AddFrame(fGeometry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,1,1));
    fGeometry->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowGeometry(Bool_t)");
    fGeometry->SetOn();
    // button: whether show magnet or not
    ShowMagnetButton = new TGCheckButton(fGeometryFrame, "show magnet");
    fGeometryFrame->AddFrame(ShowMagnetButton, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,1,1));
    ShowMagnetButton->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowMagnet(Bool_t)");
    ShowMagnetButton->SetOn();
    title1->AddFrame(fGeometryFrame);

    // button for high transparency of detectors' geometry to highlight event objects
    TGCheckButton* fTransparency = new TGCheckButton(title1, "high transparency");
    title1->AddFrame(fTransparency, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
    fTransparency->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "SwitchTransparency(Bool_t)");

    // button for switching from black to white background
    TGCheckButton* backgroundButton = new TGCheckButton(title1, "light background");
    title1->AddFrame(backgroundButton, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
    backgroundButton->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "SwitchBackground(Bool_t)");
    if (!fManager->isDarkColor)
    {
        backgroundButton->SetOn();
        gEve->GetViewers()->SwitchColorSet();
    }

    // group for displaying simulation and reconstruction data
    TGGroupFrame* groupData = new TGGroupFrame(title1, "Show MC and reco data");
    groupData->SetTitlePos(TGGroupFrame::kCenter);

    TGHorizontalFrame* framePointsInfo = new TGHorizontalFrame(groupData);
    // button for show|hide MC points
    fShowMCPoints = new TGCheckButton(framePointsInfo, "MC points");
    framePointsInfo->AddFrame(fShowMCPoints, new TGLayoutHints(kLHintsNormal, 0,0,0,0));
    fShowMCPoints->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowMCPoints(Bool_t)");
    //fShowMCPoints->SetDisabledAndSelected(kFALSE);

    // button for show|hide reconstructed points
    fShowRecoPoints = new TGCheckButton(framePointsInfo, "Reco points");
    framePointsInfo->AddFrame(fShowRecoPoints, new TGLayoutHints(kLHintsRight, 0,0,1,0));
    fShowRecoPoints->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowRecoPoints(Bool_t)");
    //fShowRecoPoints->SetDisabledAndSelected(kFALSE);
    groupData->AddFrame(framePointsInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));

    TGHorizontalFrame* frameTracksInfo = new TGHorizontalFrame(groupData);
    // button for show|hide MC tracks
    fShowMCTracks = new TGCheckButton(frameTracksInfo, "MC tracks");
    frameTracksInfo->AddFrame(fShowMCTracks, new TGLayoutHints(kLHintsNormal, 0,0,0,0));
    fShowMCTracks->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowMCTracks(Bool_t)");
    //fShowMCTracks->SetDisabledAndSelected(kFALSE);

    // button for show|hide reco tracks
    fShowRecoTracks = new TGCheckButton(frameTracksInfo, "Reco tracks");
    frameTracksInfo->AddFrame(fShowRecoTracks, new TGLayoutHints(kLHintsRight, 0,0,1,0));
    fShowRecoTracks->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowRecoTracks(Bool_t)");
    //fShowRecoTracks->SetDisabledAndSelected(kFALSE);

    groupData->AddFrame(frameTracksInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));
    title1->AddFrame(groupData, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 3,15,1,1));

    // button for update of event visualization
    if (fManager->isOnline)
        fUpdate = new TGTextButton(title1, "Start online display");
    else
        fUpdate = new TGTextButton(title1, "Update event");
    title1->AddFrame(fUpdate, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 3,15,1,1));
    fUpdate->Connect("Clicked()", "FairEventManagerEditor", this, "UpdateEvent()");

    // add all frame above to "event info" tab
    fInfoFrame->AddFrame(title1, new TGLayoutHints(kLHintsTop, 0, 0, 2, 0));

    if (iEventCount < 1)
    {
        fUpdate->SetEnabled(kFALSE);
        return;
    }

    // read first event
    iEventNumber = 0;
    fManager->GotoEvent(iEventNumber);

    // first time checking for active buttons
    if (fManager->EveMCPoints == NULL)
        fShowMCPoints->SetDisabledAndSelected(kFALSE);
    else
        fShowMCPoints->SetEnabled(kTRUE);
    if (fManager->EveMCTracks == NULL)
        fShowMCTracks->SetDisabledAndSelected(kFALSE);
    else
        fShowMCTracks->SetEnabled(kTRUE);
    if (fManager->EveRecoPoints == NULL)
        fShowRecoPoints->SetDisabledAndSelected(kFALSE);
    else
        fShowRecoPoints->SetEnabled(kTRUE);
    if (fManager->EveRecoTracks == NULL)
        fShowRecoTracks->SetDisabledAndSelected(kFALSE);
    else
        fShowRecoTracks->SetEnabled(kTRUE);

    // display event time
    TString time;
    time.Form("%.2f", FairRootManager::Instance()->GetEventTime());
    time += " ns";
    fEventTime->SetText(time.Data());

    // display and set new min and max energy limits given by event energy range
    fMinEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
    fMinEnergy->SetValue(fManager->GetEvtMinEnergy());
    MinEnergy();
    fMaxEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
    fMaxEnergy->SetValue(fManager->GetEvtMaxEnergy());
    MaxEnergy();

    // update tab controls
    Update();
}

//______________________________________________________________________________
void FairEventManagerEditor::SetModel(TObject* obj)
{
    fObject = obj;
}

// set minimum energy for particle filtering
void FairEventManagerEditor::MinEnergy()
{
    fManager->SetMinEnergy(fMinEnergy->GetValue());
}

// set maximum energy for particle filtering
void FairEventManagerEditor::MaxEnergy()
{
    fManager->SetMaxEnergy(fMaxEnergy->GetValue());
}

// set flag: show all particles or only primary
void FairEventManagerEditor::DoVizPri()
{
    if (fVizPri->IsOn())
        fManager->SetPriOnly(kTRUE);
    else
        fManager->SetPriOnly(kFALSE);
}

// select displaying particle by PDG code
void FairEventManagerEditor::SelectPDG()
{
    fManager->SelectPDG(fCurrentPDG->GetIntNumber());
}

// show or hide detector geometry
void FairEventManagerEditor::ShowGeometry(Bool_t is_show)
{
    // set cursor HourClock
    gVirtualX->SetCursor(gEve->GetMainWindow()->GetId(), gVirtualX->CreateCursor(kWatch));
    gVirtualX->SetCursor(gEve->GetLTEFrame()->GetListTree()->GetId(), gVirtualX->CreateCursor(kWatch));
    gVirtualX->SetCursor(gEve->GetLTEFrame()->GetEditor()->GetId(), gVirtualX->CreateCursor(kWatch));

    gEve->GetGlobalScene()->SetRnrState(is_show);
    if (!fManager->isOnline)
    {
        fManager->fRPhiGeomScene->SetRnrState(is_show);
        fManager->fRhoZGeomScene->SetRnrState(is_show);
    }

    // disable Magnet show choice while hiding of detector geometry
    if (!is_show)
        fGeometryFrame->HideFrame(ShowMagnetButton);
    else
        fGeometryFrame->ShowFrame(ShowMagnetButton);

    gEve->Redraw3D();

    // set cursor Pointer
    gSystem->ProcessEvents();
    gVirtualX->SetCursor(gEve->GetMainWindow()->GetId(), gVirtualX->CreateCursor(kPointer));
    gVirtualX->SetCursor(gEve->GetLTEFrame()->GetListTree()->GetId(), gVirtualX->CreateCursor(kPointer));
    gVirtualX->SetCursor(gEve->GetLTEFrame()->GetEditor()->GetId(), gVirtualX->CreateCursor(kPointer));
}

// show or hide magnet
void FairEventManagerEditor::ShowMagnet(Bool_t is_show)
{
    TGeoVolume* magnet = gGeoManager->FindVolumeFast("Magnet");
    if (!magnet)
    {
        cout<<"ERROR: There is no magnet with given name: Magnet"<<endl;
        return;
    }

    magnet->SetVisibility(is_show);
    magnet->VisibleDaughters(is_show);

    if (gEve->GetGlobalScene()->GetRnrState())
    {
        gEve->GetGlobalScene()->SetRnrState(kFALSE);
        gEve->GetGlobalScene()->SetRnrState(kTRUE);
    }

    gEve->Redraw3D();
}

// switch between light and dark background
void FairEventManagerEditor::SwitchBackground(Bool_t is_on)
{
    gEve->GetViewers()->SwitchColorSet();
}

// set transparency to high value (80%)
void FairEventManagerEditor::SwitchTransparency(Bool_t is_on)
{
    fManager->SelectedGeometryTransparent(is_on);

    if (gEve->GetGlobalScene()->GetRnrState())
    {
        gEve->GetGlobalScene()->SetRnrState(kFALSE);
        gEve->GetGlobalScene()->SetRnrState(kTRUE);
    }

    gEve->Redraw3D();
}

// show|hide MC points
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
        fShowMCPoints->SetDisabledAndSelected(kFALSE);
        return;
    }

    points->SetRnrState(is_show);

    // highlight ZDC modules if ZDC present
    if (fManager->isZDCModule)
    {
        if (is_show)
            RedrawZDC();
        else
            RestoreZDC();
    }

    // redraw points
    gEve->Redraw3D();
}

bool FairEventManagerEditor::RedrawZDC(bool isRedraw)
{
    TGeoVolume* curVolume = gGeoManager->GetVolume("VETO");
    if (!curVolume)
    {
        cout<<"ERROR: There is no volume with given name: VETO"<<endl;
        return false;
    }
    else
    {
        int i = 0;
        for (; i < 68; i++)
        {
            TGeoNode* child = curVolume->FindNode(Form("VMDL_%d", i+1));
            if (child == NULL)
                continue;
            //cout<<"Node: "<<child->GetName()<<". Number is equal "<<i<<endl;

            child->SetVisibility(fManager->isZDCModule[i]);
            child->VisibleDaughters(fManager->isZDCModule[i]);
        }

        for (; i < 104; i++)
        {
            TGeoNode* child = curVolume->FindNode(Form("UMDL_%d", i+1-68));
            if (child == NULL)
                continue;
            //cout<<"Node: "<<child->GetName()<<". Number is equal "<<i<<endl;

            child->SetVisibility(fManager->isZDCModule[i]);
            child->VisibleDaughters(fManager->isZDCModule[i]);
        }

        if ((isRedraw) && (gEve->GetGlobalScene()->GetRnrState()))
        {
            gEve->GetGlobalScene()->SetRnrState(kFALSE);
            gEve->GetGlobalScene()->SetRnrState(kTRUE);

            return true;
        }
    }// else - ZDC detector was found

    return false;
}

void FairEventManagerEditor::RestoreZDC()
{
    TGeoVolume* curVolume = gGeoManager->GetVolume("VETO");
    if (!curVolume)
    {
        cout<<"ERROR: There is no volume with given name: VETO"<<endl;
        return;
    }
    else
    {
        int i = 0;
        for (; i < 68; i++)
        {
            TGeoNode* child = curVolume->FindNode(Form("VMDL_%d", i+1));
            if (child == NULL)
                continue;
            //cout<<"Node: "<<child->GetName()<<". Number is equal "<<i<<endl;

            if (fManager->isZDCModule[i] == false)
            {
                child->SetVisibility(true);
                child->VisibleDaughters(true);
            }
        }

        for (; i < 104; i++)
        {
            TGeoNode* child = curVolume->FindNode(Form("UMDL_%d", i+1-68));
            if (child == NULL)
                continue;
            //cout<<"Node: "<<child->GetName()<<". Number is equal "<<i<<endl;

            if (fManager->isZDCModule[i] == false)
            {
                child->SetVisibility(true);
                child->VisibleDaughters(true);
            }
        }

        if (gEve->GetGlobalScene()->GetRnrState())
        {
            gEve->GetGlobalScene()->SetRnrState(kFALSE);
            gEve->GetGlobalScene()->SetRnrState(kTRUE);
        }
    }// else - ZDC detector was found
}

//______________________________________________________________________________
void FairEventManagerEditor::ShowMCTracks(Bool_t is_show)
{
    TEveElement* tracks = fManager->FindChild("MC tracks");
    if (tracks == NULL)
    {
        cout<<"There is no information about MC tracks"<<endl;
        fShowMCTracks->SetDisabledAndSelected(kFALSE);
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
        fShowRecoPoints->SetDisabledAndSelected(kFALSE);
        return;
    }

    fManager->fgShowRecoPointsIsShow = is_show;
    // exec event visualization of selected event
    if (fManager->fgRedrawRecoPointsReqired)
        fManager->GotoEvent(fCurrentEvent->GetIntNumber());

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
        fShowRecoTracks->SetDisabledAndSelected(kFALSE);
        return;
    }

    tracks->SetRnrState(is_show);
    gEve->Redraw3D();
}

void FairEventManagerEditor::BlockUI()
{
    //fCurrentEvent->SetState(kFALSE);
    //fUpdate->SetEnabled(kFALSE);
    fUpdate->SetText("Stop online display");
    //fGeometry->SetEnabled(kFALSE);
}

void FairEventManagerEditor::UnblockUI()
{
    //fCurrentEvent->SetState(kTRUE);
    //fUpdate->SetEnabled(kTRUE);
    fUpdate->SetText("Start online display");
    //fGeometry->SetEnabled(kTRUE);
}

// update event display when setting event number in textbox
void FairEventManagerEditor::SelectEvent()
{
    // if OFFLINE mode
    if (!fManager->isOnline)
    {
        int iNewEvent = fCurrentEvent->GetIntNumber();
        // exec event visualization of selected event
        fManager->GotoEvent(iNewEvent);

        if ((fManager->isZDCModule) && (fShowMCPoints->IsOn()))
            RedrawZDC();

        if (iEventNumber != iNewEvent)
        {
            iEventNumber = iNewEvent;

            // display event time
            TString time;
            time.Form("%.2f", FairRootManager::Instance()->GetEventTime());
            time += " ns";
            fEventTime->SetText(time.Data());

            // display and set new min and max energy limits given by event energy range
            fMinEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
            fMinEnergy->SetValue(fManager->GetEvtMinEnergy());
            MinEnergy();
            fMaxEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
            fMaxEnergy->SetValue(fManager->GetEvtMaxEnergy());
            MaxEnergy();
        }

        // update tab controls
        Update();

        return;
    }

    /*// update all scenes
    fManager->fRPhiView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fRhoZView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fMulti3DView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fMultiRPhiView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fMultiRhoZView->GetGLViewer()->UpdateScene(kTRUE);*/
}


// update event display when clicking Update button
void FairEventManagerEditor::UpdateEvent()
{
    if (iThreadState == 1)
    {
        iThreadState = 0;
        return;
    }

    // if OFFLINE mode
    if (!fManager->isOnline)
    {
        int iNewEvent = fCurrentEvent->GetIntNumber();
        // exec event visualization of selected event
        fManager->GotoEvent(iNewEvent);

        if ((fManager->isZDCModule) && (fShowMCPoints->IsOn()))
            RedrawZDC();

        if (iEventNumber != iNewEvent)
        {
            iEventNumber = iNewEvent;

            // display event time
            TString time;
            time.Form("%.2f", FairRootManager::Instance()->GetEventTime());
            time += " ns";
            fEventTime->SetText(time.Data());

            // display and set new min and max energy limits given by event energy range
            fMinEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
            fMinEnergy->SetValue(fManager->GetEvtMinEnergy());
            MinEnergy();
            fMaxEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
            fMaxEnergy->SetValue(fManager->GetEvtMaxEnergy());
            MaxEnergy();
        }

        // update tab controls
        Update();

        return;
    }
    // if ONLINE mode
    else
    {
        // block user interface to exclude sharing of Redraw access
        BlockUI();

        // Run separate thread for Online Display
        ThreadParam_OnlineDisplay* par_online_display = new ThreadParam_OnlineDisplay();
        par_online_display->fEventManager = fManager;
        par_online_display->fManagerEditor = this;
        par_online_display->fRootManager = FairRootManager::Instance();
        par_online_display->isZDCRedraw = false;
        if ((fManager->isZDCModule) && (fShowMCPoints->IsOn()))
            par_online_display->isZDCRedraw = true;
        par_online_display->iCurrentEvent = fCurrentEvent->GetIntNumber();

        TThread* thread_run_online = new TThread(RunOnlineDisplay, (void*)par_online_display);
        iThreadState = 1;
        thread_run_online->Run();

        return;
    }
}

void FairEventManagerEditor::SaveImage()
{
    const char* filetypes[] = {"PNG", "*.png", "JPG", "*.jpg", 0, 0};
    TGFileInfo fi;
    fi.fFileTypes = filetypes;
    fi.fIniDir    = StrDup(".");
    new TGFileDialog(gClient->GetRoot(), gEve->GetMainWindow(), kFDSave, &fi);

    printf("Saving file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
    gEve->GetDefaultGLViewer()-> SavePicture(fi.fFilename);

    return;
}

// thread function for Online Display
void* RunOnlineDisplay(void* ptr)
{
    ThreadParam_OnlineDisplay* thread_par = (ThreadParam_OnlineDisplay*) ptr;
    FairEventManager* fManager = thread_par->fEventManager;
    FairEventManagerEditor* fEditor = thread_par->fManagerEditor;
    FairRootManager* fRootManager = thread_par->fRootManager;
    int current_event = thread_par->iCurrentEvent;
    bool isZDCRedraw = thread_par->isZDCRedraw;

    // get all tasks from FairRunAna
    FairRunAna* pRun = fManager->fRunAna;
    FairTask* pMainTask = pRun->GetMainTask();
    TList* taskList = pMainTask->GetListOfTasks();

    int i;
    for (i = current_event+1; i < fEditor->iEventCount; i++)
    {
        if (fEditor->iThreadState == 0)
            break;
        do
        {
            TThread::Sleep(0, 200000000);
        } while ((gEve->GetGlobalScene()->GetGLScene()->IsLocked()) || (gEve->GetEventScene()->GetGLScene()->IsLocked()));

        fRootManager->ReadEvent(i);
        fManager->SetCurrentEvent(i);

        cout<<"Current event: "<<i<<endl;

        int iter = 1;
        TObjLink *lnk = taskList->FirstLink();
        while (lnk)
        {
            FairTask* pCurTask = (FairTask*) lnk->GetObject();
            pCurTask->ExecuteTask("");
            cout<<"Complete task: "<<iter++<<endl;
            lnk = lnk->Next();
        }

        fEditor->fCurrentEvent->SetIntNumber(i);

        // highlight ZDC modules if ZDC present
        if (isZDCRedraw)
            fEditor->RedrawZDC();

        // redraw points
        gEve->Redraw3D();
        //TThread::Sleep(1,0);
        //gSystem->ProcessEvents();
    }

    fEditor->UnblockUI();
    fEditor->iThreadState = 0;
}
