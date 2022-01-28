// MpdEventManagerEditor
//
// Specialization of TGedEditor for proper update propagation to TEveManager

#include "MpdEventManagerEditor.h"
#include "FairRootManager.h"
#include "FairRunAna.h"

#include "TChain.h"
#include "TFile.h"
#include "TGLayout.h"
#include "TGeoManager.h"
#include "TString.h"
#include "TEveManager.h"
#include "TEveElement.h"
#include "TGWindow.h"
#include <TGLViewer.h>
#include <TGLScenePad.h>
#include "TEveBrowser.h"
#include "TEveGedEditor.h"
#include "TGFileDialog.h"
#include "TThread.h"
#include "TDatabasePDG.h"
#include "TVirtualX.h"

#include <iostream>
#include <vector>
#include <regex>

using namespace std;

#define MAX_ENERGY 12

//______________________________________________________________________________
MpdEventManagerEditor::MpdEventManagerEditor(const TGWindow* p, Int_t width, Int_t height, UInt_t options, Pixel_t back)
  : TGedFrame(p, width, height, options | kVerticalFrame, back),
   fObject(0),
   fEventManager(MpdEventManager::Instance()),
   isMagnetFound(false),
   fCurrentEvent(0),
   fCurrentPDGTextEntry(0),
   fCurrentPDGString(""),
   fVizPri(0),
   fMinEnergy(0),
   fMaxEnergy(0),
   fEventTime(NULL),
   iEventCount(-1),
   isStreamSource(false),
   iThreadState(0)
{
    fEventManager->SetEventEditor(this);
    Init();
}

void MpdEventManagerEditor::Init()
{
    FairRootManager* fRootManager = FairRootManager::Instance();
    TChain* chain = fRootManager->GetInChain();

    // create tab for event visualization
    MakeTitle("EventManager Editor");
    TGVerticalFrame* fInfoFrame = CreateEditorTabSubFrame("Event Info");
    TGCompositeFrame* title1 = new TGCompositeFrame(fInfoFrame, 250, 10, kVerticalFrame | kLHintsExpandX | kFixedWidth | kOwnBackground);

    // display file name (or stream name if no input file)
    TString InSource = "File: ";
    TFile* pChainFile = chain->GetFile();
    if (pChainFile == NULL)
    {
        InSource = "Data Stream";
        isStreamSource = true;
    }
    else
        InSource += pChainFile->GetName();
    TGLabel* TFName = new TGLabel(title1, InSource.Data());
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
    // display event time
    TString time = TString::Format("%.2f ns", FairRootManager::Instance()->GetEventTime());
    fEventTime->SetText(time.Data());

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
    fCurrentEvent->Connect("ValueSet(Long_t)","MpdEventManagerEditor", this, "SelectEvent()");
    // if event count is 0 then deactivate the field for event number
    if (iEventCount < 1)
        fCurrentEvent->SetState(kFALSE);

    // button for saving the current image (EVE screenshot)
    fSave = new TGPictureButton(f, gClient->GetPicture("save.xpm"), 5);
    f->AddFrame(fSave, new TGLayoutHints(kLHintsLeft| kLHintsCenterY, 1, 2, 1, 1));
    fSave->Connect("Clicked()", "MpdEventManagerEditor", this, "SaveImage()");
    title1->AddFrame(f);

    // checkbox to display only primary particles in event
    fVizPri = new TGCheckButton(title1, "Primary Only");
    AddFrame(fVizPri, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fVizPri->Connect("Toggled(Bool_t)", "MpdEventManagerEditor", this, "DoVizPri()");
    title1->AddFrame(fVizPri);

    // textbox to display only particles with given PDG
    TGHorizontalFrame* f1 = new TGHorizontalFrame(title1);
    TGLabel* L1 = new TGLabel(f1, "Select Particles: ");
    f1->AddFrame(L1, new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 1, 2, 1, 1));
    fCurrentPDGTextEntry = new TGTextEntry(f1);
    f1->AddFrame(fCurrentPDGTextEntry, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
    fCurrentPDGTextEntry->Connect("TextChanged(char*)","MpdEventManagerEditor", this, "SelectPDG()");
    title1->AddFrame(f1);

    // textbox for min energy cutting
    fMinEnergy = new TEveGValuator(title1, "Min Energy:", 90, 0);
    fMinEnergy->SetNELength(5);
    fMinEnergy->SetLabelWidth(80);
    fMinEnergy->Build();
    fMinEnergy->SetLimits(0, MAX_ENERGY, 2001, TGNumberFormat::kNESRealOne);
    fMinEnergy->SetToolTip("Minimum energy of displayed tracks");
    fMinEnergy->SetValue(0);
    fMinEnergy->Connect("ValueSet(Double_t)", "MpdEventManagerEditor", this, "MinEnergy()");
    title1->AddFrame(fMinEnergy, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
    fEventManager->SetMinEnergy(0);

    // textbox for max energy cutting
    fMaxEnergy = new TEveGValuator(title1, "Max Energy:", 90, 0);
    fMaxEnergy->SetNELength(5);
    fMaxEnergy->SetLabelWidth(80);
    fMaxEnergy->Build();
    fMaxEnergy->SetLimits(0, MAX_ENERGY, 2001, TGNumberFormat::kNESRealOne);
    fMaxEnergy->SetToolTip("Maximum energy of displayed tracks");
    fMaxEnergy->SetValue(MAX_ENERGY);
    fMaxEnergy->Connect("ValueSet(Double_t)", "MpdEventManagerEditor", this, "MaxEnergy()");
    title1->AddFrame(fMaxEnergy, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
    fEventManager->SetMaxEnergy(MAX_ENERGY);

    fGeometryFrame = new TGHorizontalFrame(title1);
    // button: whether show detector geometry or not
    fGeometry = new TGCheckButton(fGeometryFrame, "show geometry");
    fGeometryFrame->AddFrame(fGeometry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,1,1));
    fGeometry->Connect("Toggled(Bool_t)", "MpdEventManagerEditor", this, "ShowGeometry(Bool_t)");
    fGeometry->SetOn();
    // button: whether show magnet or not
    TGeoVolume* magnet = gGeoManager->FindVolumeFast("Magnet");
    if (magnet)
    {
        isMagnetFound = true;
        ShowMagnetButton = new TGCheckButton(fGeometryFrame, "show magnet");
        fGeometryFrame->AddFrame(ShowMagnetButton, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,1,1));
        ShowMagnetButton->Connect("Toggled(Bool_t)", "MpdEventManagerEditor", this, "ShowMagnet(Bool_t)");
        ShowMagnetButton->SetOn();
    }
    title1->AddFrame(fGeometryFrame);

    // button for high transparency of detectors' geometry to highlight event objects
    TGCheckButton* fTransparency = new TGCheckButton(title1, "high transparency");
    title1->AddFrame(fTransparency, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
    fTransparency->Connect("Toggled(Bool_t)", "MpdEventManagerEditor", this, "SwitchTransparency(Bool_t)");

    // button for switching from black to white background
    TGCheckButton* backgroundButton = new TGCheckButton(title1, "light background");
    title1->AddFrame(backgroundButton, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
    backgroundButton->Connect("Toggled(Bool_t)", "MpdEventManagerEditor", this, "SwitchBackground(Bool_t)");
    if (!fEventManager->isDarkColor)
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
    fShowMCPoints->Connect("Toggled(Bool_t)", "MpdEventManagerEditor", this, "ShowMCPoints(Bool_t)");
    fShowMCPoints->SetDisabledAndSelected(kFALSE);

    // button for show|hide reconstructed points
    fShowRecoPoints = new TGCheckButton(framePointsInfo, "Reco points");
    framePointsInfo->AddFrame(fShowRecoPoints, new TGLayoutHints(kLHintsRight, 0,0,1,0));
    fShowRecoPoints->Connect("Toggled(Bool_t)", "MpdEventManagerEditor", this, "ShowRecoPoints(Bool_t)");
    fShowRecoPoints->SetDisabledAndSelected(kFALSE);
    groupData->AddFrame(framePointsInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));

    TGHorizontalFrame* frameTracksInfo = new TGHorizontalFrame(groupData);
    // button for show|hide MC tracks
    fShowMCTracks = new TGCheckButton(frameTracksInfo, "MC tracks");
    frameTracksInfo->AddFrame(fShowMCTracks, new TGLayoutHints(kLHintsNormal, 0,0,0,0));
    fShowMCTracks->Connect("Toggled(Bool_t)", "MpdEventManagerEditor", this, "ShowMCTracks(Bool_t)");
    fShowMCTracks->SetDisabledAndSelected(kFALSE);

    // button for show|hide reco tracks
    fShowRecoTracks = new TGCheckButton(frameTracksInfo, "Reco tracks");
    frameTracksInfo->AddFrame(fShowRecoTracks, new TGLayoutHints(kLHintsRight, 0,0,1,0));
    fShowRecoTracks->Connect("Toggled(Bool_t)", "MpdEventManagerEditor", this, "ShowRecoTracks(Bool_t)");
    fShowRecoTracks->SetDisabledAndSelected(kFALSE);

    groupData->AddFrame(frameTracksInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));
    title1->AddFrame(groupData, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 3,15,1,1));

    // button for update of event visualization
    if (fEventManager->isOnline)
        fUpdate = new TGTextButton(title1, "Start online display");
    else
        fUpdate = new TGTextButton(title1, "Update event");
    title1->AddFrame(fUpdate, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 3,15,1,1));
    fUpdate->Connect("Clicked()", "MpdEventManagerEditor", this, "UpdateEvent()");

    // add all frame above to "event info" tab
    fInfoFrame->AddFrame(title1, new TGLayoutHints(kLHintsTop, 0, 0, 2, 0));

    if ((iEventCount < 1) && (!isStreamSource))
    {
        fUpdate->SetEnabled(kFALSE);
        return;
    }

    // read first event in Offline mode
    if (!fEventManager->isOnline) fEventManager->GotoEvent(0);

    // update tab controls
    Update();
}

// set flag: show all particles or only primary
void MpdEventManagerEditor::DoVizPri()
{
    if (fVizPri->IsOn())
        fEventManager->SetPriOnly(kTRUE);
    else
        fEventManager->SetPriOnly(kFALSE);
}

// select displaying particle by PDG code
void MpdEventManagerEditor::SelectPDG()
{
    fCurrentPDGString = fCurrentPDGTextEntry->GetDisplayText();
}

//______________________________________________________________________________
void MpdEventManagerEditor::MinEnergy()
{
    fEventManager->SetMinEnergy(fMinEnergy->GetValue());
}

//______________________________________________________________________________
void MpdEventManagerEditor::MaxEnergy()
{
    fEventManager->SetMaxEnergy(fMaxEnergy->GetValue());
}

// show or hide detector geometry
void MpdEventManagerEditor::ShowGeometry(Bool_t is_show)
{
    // set cursor HourClock
    gVirtualX->SetCursor(gEve->GetMainWindow()->GetId(), gVirtualX->CreateCursor(kWatch));
    gVirtualX->SetCursor(gEve->GetLTEFrame()->GetListTree()->GetId(), gVirtualX->CreateCursor(kWatch));
    gVirtualX->SetCursor(gEve->GetLTEFrame()->GetEditor()->GetId(), gVirtualX->CreateCursor(kWatch));

    gEve->GetGlobalScene()->SetRnrState(is_show);
    if (!fEventManager->isOnline)
    {
        fEventManager->GetRPhiScene()->SetRnrState(is_show);
        fEventManager->GetRhoZScene()->SetRnrState(is_show);
    }

    // disable "Magnet Show" box after hiding of detector geometry
    if (isMagnetFound)
    {
        if (!is_show)
            fGeometryFrame->HideFrame(ShowMagnetButton);
        else
            fGeometryFrame->ShowFrame(ShowMagnetButton);
    }

    gEve->Redraw3D();

    // set cursor Pointer
    gSystem->ProcessEvents();
    gVirtualX->SetCursor(gEve->GetMainWindow()->GetId(), gVirtualX->CreateCursor(kPointer));
    gVirtualX->SetCursor(gEve->GetLTEFrame()->GetListTree()->GetId(), gVirtualX->CreateCursor(kPointer));
    gVirtualX->SetCursor(gEve->GetLTEFrame()->GetEditor()->GetId(), gVirtualX->CreateCursor(kPointer));
}

// show or hide magnet
void MpdEventManagerEditor::ShowMagnet(Bool_t is_show)
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
        gEve->Redraw3D();
    }
}

// switch between light and dark background
void MpdEventManagerEditor::SwitchBackground(Bool_t is_on)
{
    gEve->GetViewers()->SwitchColorSet();
}

// set transparency to high value (80%)
void MpdEventManagerEditor::SwitchTransparency(Bool_t is_on)
{
    fEventManager->SetTransparentGeometry(is_on);

    if (gEve->GetGlobalScene()->GetRnrState())
    {
        gEve->GetGlobalScene()->SetRnrState(kFALSE);
        gEve->GetGlobalScene()->SetRnrState(kTRUE);
        gEve->Redraw3D();
    }
}

// show|hide MC points
void MpdEventManagerEditor::ShowMCPoints(Bool_t is_show)
{
    /*
    TEveElement::List_t matches;
    TPRegexp* regexp = new TPRegexp("(\\w+)Point\\b");
    Int_t numFound = fEventManager->FindChildren(matches, *regexp);
    if (numFound > 0)
    {
        for (TEveElement::List_i p = matches.begin(); p != matches.end(); ++p)
            (*p)->SetRnrState(is_show);
    }
    */

    TEveElement* points = fEventManager->FindChild("MC points");
    if (points == NULL)
    {
        cout<<"There is no information about MC points"<<endl;
        fShowMCPoints->SetDisabledAndSelected(kFALSE);
        return;
    }

    points->SetRnrState(is_show);

    if (fEventManager->isZDCModule)
    {
        if (is_show)
            RedrawZDC(false);
        else
            RedrawZDC(true);
    }

    fEventManager->fgShowShowMCPoints = is_show;
    // exec event visualization of selected event
    if (fEventManager->fgRedrawRecoPointsReqired)
        fEventManager->GotoEvent(fCurrentEvent->GetIntNumber());

    // redraw points
    gEve->Redraw3D();
}

bool MpdEventManagerEditor::RedrawZDC(bool isFull, bool isRedraw)
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

            if (isFull)
            {
                if (fEventManager->isZDCModule[i] == false)
                {
                    child->SetVisibility(true);
                    child->VisibleDaughters(true);
                }
            }
            else
            {
                child->SetVisibility(fEventManager->isZDCModule[i]);
                child->VisibleDaughters(fEventManager->isZDCModule[i]);
            }
        }

        for (; i < 104; i++)
        {
            TGeoNode* child = curVolume->FindNode(Form("UMDL_%d", i+1-68));
            if (child == NULL)
                continue;
            //cout<<"Node: "<<child->GetName()<<". Number is equal "<<i<<endl;

            if (isFull)
            {
                if (fEventManager->isZDCModule[i] == false)
                {
                    child->SetVisibility(true);
                    child->VisibleDaughters(true);
                }
            }
            else
            {
                child->SetVisibility(fEventManager->isZDCModule[i]);
                child->VisibleDaughters(fEventManager->isZDCModule[i]);
            }
        }

        if ((isFull || isRedraw) && (gEve->GetGlobalScene()->GetRnrState()))
        {
            gEve->GetGlobalScene()->SetRnrState(kFALSE);
            gEve->GetGlobalScene()->SetRnrState(kTRUE);

            return true;
        }
    }// else - ZDC detector was found

    return false;
}

//______________________________________________________________________________
void MpdEventManagerEditor::ShowMCTracks(Bool_t is_show)
{
    TEveElement* tracks = fEventManager->FindChild("MC tracks");
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
void MpdEventManagerEditor::ShowRecoPoints(Bool_t is_show)
{
    TEveElement* points = fEventManager->FindChild("Reco points");
    if (points == NULL)
    {
        cout<<"There is no information about reconstructed points"<<endl;
        fShowRecoPoints->SetDisabledAndSelected(kFALSE);
        return;
    }

    fEventManager->fgShowShowRecoPoints = is_show;
    // exec event visualization of selected event
    if (fEventManager->fgRedrawRecoPointsReqired)
        fEventManager->GotoEvent(fCurrentEvent->GetIntNumber());

    points->SetRnrState(is_show);
    gEve->Redraw3D();
}

//______________________________________________________________________________
void MpdEventManagerEditor::ShowRecoTracks(Bool_t is_show)
{
    TEveElement* tracks = fEventManager->FindChild("Reco tracks");
    if (tracks == NULL)
    {
        cout<<"There is no information about reconstructed tracks"<<endl;
        fShowRecoTracks->SetDisabledAndSelected(kFALSE);
        return;
    }

    fEventManager->fgShowShowRecoTracks = is_show;
    // exec event visualization of selected event
    if (fEventManager->fgRedrawRecoPointsReqired)
        fEventManager->GotoEvent(fCurrentEvent->GetIntNumber());

    tracks->SetRnrState(is_show);
    gEve->Redraw3D();
}

void MpdEventManagerEditor::SaveImage()
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

// update event display when setting event number in textbox
void MpdEventManagerEditor::SelectEvent()
{
    // if OFFLINE mode
    if (!fEventManager->isOnline)
    {
        int iOldEvent = fEventManager->GetCurrentEvent(), iNewEvent = fCurrentEvent->GetIntNumber();
        // exec event visualization of selected event
        fEventManager->GotoEvent(iNewEvent);

        if ((fEventManager->isZDCModule) && (fShowMCPoints->IsOn()))
            RedrawZDC(false);

        if (iOldEvent != iNewEvent)
        {
            // display event time
            TString time = TString::Format("%.2f ns", FairRootManager::Instance()->GetEventTime());
            fEventTime->SetText(time.Data());

            // display new min and max energy limits given by event energy range
            fMinEnergy->SetLimits(fEventManager->GetEvtMinEnergy(), fEventManager->GetEvtMaxEnergy(), 2001);
            fMinEnergy->SetValue(fEventManager->GetEvtMinEnergy());
            fMaxEnergy->SetLimits(fEventManager->GetEvtMinEnergy(), fEventManager->GetEvtMaxEnergy(), 2001);
            fMaxEnergy->SetValue(fEventManager->GetEvtMaxEnergy());
        }

        // update tab controls
        Update();

        return;
    }

    /*// update all scenes
    fEventManager->fRPhiView->GetGLViewer()->UpdateScene(kTRUE);
    fEventManager->fRhoZView->GetGLViewer()->UpdateScene(kTRUE);
    fEventManager->fMulti3DView->GetGLViewer()->UpdateScene(kTRUE);
    fEventManager->fMultiRPhiView->GetGLViewer()->UpdateScene(kTRUE);
    fEventManager->fMultiRhoZView->GetGLViewer()->UpdateScene(kTRUE);*/
}


// clicking 'Update event' button to update event display OR 'Start online display' button to start/stop online event display
void MpdEventManagerEditor::UpdateEvent()
{
    if (iThreadState == 1)
    {
        iThreadState = 0;
        return;
    }

    fEventManager->fCurrentPDG.clear();
    istringstream iss(fCurrentPDGString);
    auto databasePDG = TDatabasePDG::Instance();
    regex rgx("[\\s,]+");
    sregex_token_iterator iter(fCurrentPDGString.begin(), fCurrentPDGString.end(), rgx, -1);
    sregex_token_iterator end;
    for ( ; iter != end; ++iter)
    {
        string s = (*iter).str();
        if (s != "")
        {   
            auto pdg = atoi(s.c_str());
            if (pdg)
                fEventManager->fCurrentPDG.insert(pdg);
            else
            {
                auto particle = databasePDG->GetParticle(s.c_str());
                if (particle)
                    fEventManager->fCurrentPDG.insert(particle->PdgCode());
            }
        }
    }

    // if OFFLINE mode
    if (!fEventManager->isOnline)
    {
        int iOldEvent = fEventManager->GetCurrentEvent(), iNewEvent = fCurrentEvent->GetIntNumber();
        // exec event visualization of selected event
        fEventManager->GotoEvent(iNewEvent);

        if ((fEventManager->isZDCModule) && (fShowMCPoints->IsOn()))
            RedrawZDC(false);

        if (iOldEvent != iNewEvent)
        {
            // display event time
            TString time = TString::Format("%.2f ns", FairRootManager::Instance()->GetEventTime());
            fEventTime->SetText(time.Data());

            // display new min and max energy limits given by event energy range
            fMinEnergy->SetLimits(fEventManager->GetEvtMinEnergy(), fEventManager->GetEvtMaxEnergy(), 2001);
            fMinEnergy->SetValue(fEventManager->GetEvtMinEnergy());
            fMaxEnergy->SetLimits(fEventManager->GetEvtMinEnergy(), fEventManager->GetEvtMaxEnergy(), 2001);
            fMaxEnergy->SetValue(fEventManager->GetEvtMaxEnergy());
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
        par_online_display->fEventManager = fEventManager;
        par_online_display->fManagerEditor = this;
        par_online_display->fRootManager = FairRootManager::Instance();
        par_online_display->isZDCRedraw = false;
        if ((fEventManager->isZDCModule) && (fShowMCPoints->IsOn()))
            par_online_display->isZDCRedraw = true;
        par_online_display->iCurrentEvent = fCurrentEvent->GetIntNumber();
        par_online_display->isStreamSource = isStreamSource;

        TThread* thread_run_online = new TThread(RunOnlineDisplay, (void*)par_online_display);
        iThreadState = 1;
        thread_run_online->Run();

        return;
    }
}

void MpdEventManagerEditor::BlockUI()
{
    //fCurrentEvent->SetState(kFALSE);
    //fUpdate->SetEnabled(kFALSE);
    fUpdate->SetText("Stop online display");
    //fGeometry->SetEnabled(kFALSE);
}

void MpdEventManagerEditor::UnblockUI()
{
    //fCurrentEvent->SetState(kTRUE);
    //fUpdate->SetEnabled(kTRUE);
    fUpdate->SetText("Start online display");
    //fGeometry->SetEnabled(kTRUE);
}

// thread function for Online Display
void* RunOnlineDisplay(void* ptr)
{
    ThreadParam_OnlineDisplay* thread_par = (ThreadParam_OnlineDisplay*) ptr;
    MpdEventManager* fEventManager = thread_par->fEventManager;
    MpdEventManagerEditor* fEditor = thread_par->fManagerEditor;
    FairRootManager* fRootManager = thread_par->fRootManager;
    int current_event = thread_par->iCurrentEvent;
    bool stream_source = thread_par->isStreamSource;
    bool isZDCRedraw = thread_par->isZDCRedraw;

    // get all tasks from FairRunAna
    FairRunAna* pRun = fEventManager->fRunAna;
    FairTask* pMainTask = pRun->GetMainTask();
    TList* taskList = pMainTask->GetListOfTasks();

    for (int i = current_event+1; ((stream_source) || (i < fEditor->iEventCount)); i++)
    {
        if (fEditor->iThreadState == 0)
            break;
        do
        {
            TThread::Sleep(0, 200000000);
        } while ((gEve->GetGlobalScene()->GetGLScene()->IsLocked()) || (gEve->GetEventScene()->GetGLScene()->IsLocked()));

        fRootManager->ReadEvent(i);
        fEventManager->SetCurrentEvent(i);

        // run all tasks
        TObjLink* lnk = taskList->FirstLink();
        while (lnk)
        {
            FairTask* pCurTask = (FairTask*) lnk->GetObject();
            pCurTask->ExecuteTask("");
            lnk = lnk->Next();
        }

        //fEditor->fCurrentEvent->SetIntNumber(i);

        // highlight ZDC modules if ZDC present
        if (isZDCRedraw)
            fEditor->RedrawZDC();

        // redraw points
        cout<<"Redrawing event #"<<i<<"..."<<endl<<endl;
        gEve->Redraw3D();
        //TThread::Sleep(1,0);
        //gSystem->ProcessEvents();
    }

    fEditor->UnblockUI();
    fEditor->iThreadState = 0;

    return NULL;
}

ClassImp(MpdEventManagerEditor);
