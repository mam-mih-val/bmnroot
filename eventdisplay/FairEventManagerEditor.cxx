#include "FairEventManagerEditor.h"
#include "RawDataConverter.h"
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

#include <iostream>
using namespace std;

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
   iEventNumber(-1),
   iEventCount(-1)
{
  Init();

  fEventReadData = new vector<EventData*>();
  fEventDrawData = new vector<EventData*>();
}

void FairEventManagerEditor::Init()
{
    // get input file and event count
    FairRootManager* fRootManager = FairRootManager::Instance();
    TChain* chain = fRootManager->GetInChain();

    if (fManager->source_file_name)
        iEventCount = fManager->fEntryCount;
    else
        iEventCount = chain->GetEntriesFast();

    // create tab for event visualization
    MakeTitle("FairEventManager  Editor");
    TGVerticalFrame* fInfoFrame = CreateEditorTabSubFrame("Event Info");
    title1 = new TGCompositeFrame(fInfoFrame, 250, 10,
        kVerticalFrame | kLHintsExpandX | kFixedWidth | kOwnBackground);

    // display file name
    TString Infile= "File : ";
    if (fManager->source_file_name)
        Infile += fManager->source_file_name;
    else
        Infile += chain->GetFile()->GetName();
    TGLabel* TFName = new TGLabel(title1, Infile.Data());
    title1->AddFrame(TFName);

    // display Run ID
    UInt_t RunId = FairRunAna::Instance()->getRunId();
    TString run = "Run Id : ";
    run += RunId;
    TGLabel* TRunId = new TGLabel(title1, run.Data());
    title1->AddFrame(TRunId);

    // display event count
    TString nevent = "No of events : ";
    nevent += iEventCount;
    TGLabel* TEvent = new TGLabel(title1, nevent.Data());
    title1->AddFrame(TEvent);

    // count of geometry nodes
    Int_t nodes = gGeoManager->GetNNodes();
    TString NNodes = "No. of Nodes : ";
    NNodes += nodes;
    TGLabel* NoNode = new TGLabel(title1, NNodes.Data());
    title1->AddFrame(NoNode);

    // setting textbox for event number
    TGHorizontalFrame* f = new TGHorizontalFrame(title1);
    TGLabel* l = new TGLabel(f, "Current Event:");
    f->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 2, 1, 1));
    fCurrentEvent = new TGNumberEntry(f, 0., 6, -1,
                        TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, iEventCount-1);
    f->AddFrame(fCurrentEvent, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
    fCurrentEvent->Connect("ValueSet(Long_t)","FairEventManagerEditor", this, "SelectEvent()");
    title1->AddFrame(f);
    if (iEventCount< 1)
        fCurrentEvent->SetState(kFALSE);

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
                      TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits, 0, 1);
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
    fGeometry = new TGCheckButton(title1, "show geometry");
    title1->AddFrame(fGeometry, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
    fGeometry->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowGeometry(Bool_t)");
    fGeometry->SetOn();

    // button for switching from black to white background
    TGCheckButton* fBackground = new TGCheckButton(title1, "light background");
    title1->AddFrame(fBackground, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
    fBackground->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "SwitchBackground(Bool_t)");
    if (!fManager->isDarkColor)
    {
        fBackground->SetOn();
        gEve->GetViewers()->SwitchColorSet();
    }

    // button for high transparency of detectors' geometry to highlight event objects
    TGCheckButton* fTransparency = new TGCheckButton(title1, "high transparency");
    title1->AddFrame(fTransparency, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
    fTransparency->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "SwitchTransparency(Bool_t)");

    // group for displaying simulation and reconstruction data
    groupData = new TGGroupFrame(title1, "Show MC and reco data");
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
    fUpdate->Connect("Clicked()", "FairEventManagerEditor", this, "SelectEvent()");

    // add all frame above to "event info" tab
    fInfoFrame->AddFrame(title1, new TGLayoutHints(kLHintsTop, 0, 0, 2, 0));

    // if raw data online stream - return because of separate read and draw functions
    if ((fManager->isOnline) && (fManager->fDataSource == 1))
        return;

    if (iEventCount < 1)
    {
        fUpdate->SetEnabled(kFALSE);
        return;
    }

    // read first event
    fManager->GotoEvent(0);

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

    iEventNumber = 0;

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
    gEve->GetGlobalScene()->SetRnrState(is_show);
    fManager->fRPhiGeomScene->SetRnrState(is_show);
    fManager->fRhoZGeomScene->SetRnrState(is_show);

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

    gEve->GetGlobalScene()->SetRnrState(kFALSE);
    gEve->GetGlobalScene()->SetRnrState(kTRUE);
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

void FairEventManagerEditor::RedrawZDC()
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

        gEve->GetGlobalScene()->SetRnrState(kFALSE);
        gEve->GetGlobalScene()->SetRnrState(kTRUE);
    }// else - ZDC detector was found
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

        gEve->GetGlobalScene()->SetRnrState(kFALSE);
        gEve->GetGlobalScene()->SetRnrState(kTRUE);
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
    fCurrentEvent->SetState(kFALSE);
    fUpdate->SetEnabled(kFALSE);
    fGeometry->SetEnabled(kFALSE);
}

void FairEventManagerEditor::UnblockUI()
{
    fCurrentEvent->SetState(kTRUE);
    fUpdate->SetEnabled(kTRUE);
    fGeometry->SetEnabled(kTRUE);
}

// update event display when setting event number in textbox or clicking Update button
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
    // if ONLINE mode
    else
    {
        // block user interface to exclude sharing of Redraw access
        BlockUI();

        // if raw data online stream - separate read and draw threads
        if (fManager->fDataSource == 1)
        {
            semEventData = new TSemaphore(0);

            // run thread for data files reading
            ThreadParam_ReadFile* par_read_file = new ThreadParam_ReadFile();
            par_read_file->fEventReadData = fEventReadData;
            par_read_file->fEventDrawData = fEventDrawData;
            par_read_file->raw_file_name_begin = fManager->source_file_name;
            par_read_file->semEventData = semEventData;

            TThread* thread_read_file = new TThread(ReadMWPCFiles, (void*)par_read_file);
            thread_read_file->Run();

            // run thread to draw hit arrays if ready
            ThreadParam_Draw* par_draw = new ThreadParam_Draw();
            par_draw->fEventDrawData = fEventDrawData;
            par_draw->fEventManager = fManager;
            par_draw->semEventData = semEventData;

            TThread* thread_draw = new TThread(DrawEvent, (void*)par_draw);
            thread_draw->Run();

            return;
        }
        else
        {
            // run thread for online display of root files
            ThreadParam_RunTask* par_run_task = new ThreadParam_RunTask();
            par_run_task->fEventManager = fManager;
            par_run_task->fManagerEditor = this;
            par_run_task->isZDCRedraw = false;
            if ((fManager->isZDCModule) && (fShowMCPoints->IsOn()))
                par_run_task->isZDCRedraw = true;
            par_run_task->isRootManagerReadEvent = (fManager->source_file_name == NULL);

            TThread* thread_run_task = new TThread(RunTasks, (void*)par_run_task);
            thread_run_task->Run();

            return;
        }
    }

    /*// update all scenes
    fManager->fRPhiView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fRhoZView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fMulti3DView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fMultiRPhiView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fMultiRhoZView->GetGLViewer()->UpdateScene(kTRUE);*/
}

// thread function for reading of raw MWPC files
void* ReadMWPCFiles(void* ptr)
{
     ThreadParam_ReadFile* thread_par = (ThreadParam_ReadFile*) ptr;
     char* raw_file_name_begin = thread_par->raw_file_name_begin;
     vector<EventData*>* fEventReadData = thread_par->fEventReadData;
     vector<EventData*>* fEventDrawData = thread_par->fEventDrawData;

     // read source files and generate vector of EventData objects
     RawDataParser raw_parser;

     TString* mwpc_names = new TString[12];
     raw_parser.GenerateMWPCFileNames(raw_file_name_begin, &raw_parser.device_serial1, mwpc_names);
     raw_parser.GenerateMWPCFileNames(raw_file_name_begin, &raw_parser.device_serial2, &mwpc_names[6]);

     long lStart[12] = { 0 };
     int processed_events = 0;
     while (1)
     {
         raw_parser.ParseHRBFiles(fEventReadData, mwpc_names, lStart);

         // copy complete events to another draw vector to exclude time-consuming sharing
         for (int i = processed_events; i < fEventReadData->size(); i++)
         {
             EventData* pCurEvent = (*fEventReadData)[i];
             EventData* pDrawEvent = new EventData(*pCurEvent);
             fEventDrawData->push_back(pDrawEvent);
             thread_par->semEventData->Post();
         }
         processed_events = fEventReadData->size();
     }// while (1)

     delete [] mwpc_names;
     return 0;
}

// thread function for event drawing with raw data
void* DrawEvent(void* ptr)
{
    ThreadParam_Draw* thread_par = (ThreadParam_Draw*) ptr;
    vector<EventData*>* fEventDrawData = thread_par->fEventDrawData;
    FairEventManager* fEventManager = thread_par->fEventManager;
    TSemaphore* semEventData = thread_par->semEventData;

    int i = 0;
    TEvePointSet* fq = NULL;
    while (1)
    {
        semEventData->Wait();

        if (i >= fEventDrawData->size())
        {
            cout<<"Programming Error: index >= size : "<<i<<" >= "<<fEventDrawData->size()<<endl;
            continue;
        }

        EventData* curEvent = (*fEventDrawData)[i];

        RawDataConverter raw_converter;
        vector<TVector3*> event_hits = raw_converter.MWPCEventToGeoVector(curEvent);

        cout<<"Event processing: "<<i<<". Point vector size: "<<event_hits.size()<<endl;

        if (fq)
        {
            fq->Reset();
            gEve->RemoveElement(fq, fEventManager->EveRecoPoints);
        }

        TEvePointSet* q = RawDataConverter::Vector2EvePoints(&event_hits, "MWPC points", kRed, kFullDotMedium, 1, true);

        if (fEventManager->EveRecoPoints == NULL)
        {
            fEventManager->EveRecoPoints = new TEveElementList("MWPC points");
            gEve->AddElement(fEventManager->EveRecoPoints, fEventManager);
            fEventManager->EveRecoPoints->SetRnrState(kTRUE);
        }

        gEve->AddElement(q, fEventManager->EveRecoPoints);

        gEve->Redraw3D(kFALSE);

        fq = q;

        /*if (q->Size() == 2)
        {
            cout<<"sleep 20..."<<endl;
            sleep(20);
        }*/

        cout<<"Event hits were drawn for event "<<i<<endl;
        sleep(1);
        i++;
    }
}

// thread function for execution of FairRunAna tasks
void* RunTasks(void* ptr)
{
    ThreadParam_RunTask* thread_par = (ThreadParam_RunTask*) ptr;
    FairEventManager* fManager = thread_par->fEventManager;
    FairEventManagerEditor* fEditor = thread_par->fManagerEditor;
    bool isZDCRedraw = thread_par->isZDCRedraw;
    bool isRootManagerReadEvent = thread_par->isRootManagerReadEvent;

    // get all tasks from FairRunAna
    FairRunAna* pRun = fManager->fRunAna;
    FairTask* pMainTask = pRun->GetMainTask();
    TList* taskList = pMainTask->GetListOfTasks();

    FairRootManager* fRootManager = FairRootManager::Instance();

    for (int i = 1; i < fEditor->iEventCount; i++)
    {
        if (isRootManagerReadEvent)
            fRootManager->ReadEvent(i);
        else
        {
            sleep(1);
            fEditor->fCurrentEvent->SetIntNumber(i);
            fManager->SetCurrentEvent(i);
        }

        int iter = 1;
        TObjLink *lnk = taskList->FirstLink();
        while (lnk)
        {
            FairTask* pCurTask = (FairTask*) lnk->GetObject();
            pCurTask->ExecuteTask("");
            cout<<"Complete task: "<<iter++<<endl;
            lnk = lnk->Next();
        }

        // highlight ZDC modules if ZDC present
        if (isZDCRedraw)
            fEditor->RedrawZDC();

        // redraw points
        gEve->Redraw3D();
    }

    fEditor->UnblockUI();
}
