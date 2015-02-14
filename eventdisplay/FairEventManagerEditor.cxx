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

// thread function for reading of MWPC files
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
             processed_events = fEventReadData->size();
             EventData* pCurEvent = (*fEventReadData)[i];
             EventData* pDrawEvent = new EventData(*pCurEvent);
             fEventDrawData->push_back(pDrawEvent);
             thread_par->semEventData->Post();
         }
     }// while (1)

     return 0;
}

void GeoDraw(vector<TVector3*>* pPointVector, FairEventManager* fEventManager, TEvePointSet** fq)
{
    if (*fq)
    {
        (*fq)->Reset();
        gEve->RemoveElement(*fq, fEventManager->EveRecoPoints);
    }

    Color_t fColor = kRed;
    Style_t fStyle = kFullDotMedium;

    Int_t npoints = pPointVector->size();
    TEvePointSet* q = new TEvePointSet("MWPC points", npoints, TEvePointSelectorConsumer::kTVT_XYZ);

    q->SetOwnIds(kTRUE);
    q->SetMarkerColor(fColor);
    q->SetMarkerSize(1);
    q->SetMarkerStyle(fStyle);

    //q->SetNextPoint(0, 0, 0);

    for (Int_t i = 0; i < npoints; i++)
    {
      TVector3* vec = (*pPointVector)[i];
      cout<<"Point "<<i<<": x="<<vec->X()<<" y="<<vec->Y()<<" z="<<vec->Z()<<endl;
      q->SetNextPoint(vec->X(), vec->Y(), vec->Z());
      q->SetPointId(new TNamed(Form("Point %d", i), ""));
      // bug in ROOT with one point
      if (npoints == 1)
      {
          TVector3* vecAdd = new TVector3(vec->X(), vec->Y(), vec->Z());
          cout<<"Point "<<i<<": x="<<vecAdd->X()<<" y="<<vecAdd->Y()<<" z="<<vecAdd->Z()<<endl;
          q->SetNextPoint(vecAdd->X(), vecAdd->Y(), vecAdd->Z());
          q->SetPointId(new TNamed(Form("Point %d", i+1), ""));
      }
    }

    if (fEventManager->EveRecoPoints == NULL)
    {
        fEventManager->EveRecoPoints = new TEveElementList("MWPC points");
        gEve->AddElement(fEventManager->EveRecoPoints, fEventManager);
        fEventManager->EveRecoPoints->SetRnrState(kTRUE);
    }

    gEve->AddElement(q, fEventManager->EveRecoPoints);

    gEve->Redraw3D(kFALSE);

    *fq = q;

    //if (npoints == 1)
        //sleep(90);

    return;
}

// thread function for event drawing
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

        //cout<<i<<" "<<fEventData->size()<<endl;

        EventData* curEvent = (*fEventDrawData)[i];
        cout<<"Event processing: "<<i<<endl;

        RawDataConverter raw_converter;
        vector<TVector3*> event_hits = raw_converter.MWPCEventToGeoVector(curEvent);

        cout<<"Point vector size: "<<event_hits.size()<<endl;

        GeoDraw(&event_hits, fEventManager, &fq);

        cout<<"Event hits were drawn for event "<<i<<endl;
        sleep(1);
        i++;
    }
}

// thread function for run tasks
void* RunTasks(void* ptr)
{
    ThreadParam_RunTask* thread_par = (ThreadParam_RunTask*) ptr;
    FairEventManager* fManager = thread_par->fEventManager;

    // get all tasks from FairRunAna
    FairRunAna* pRun = fManager->fRunAna;
    FairTask* pMainTask = pRun->GetMainTask();
    TList* taskList = pMainTask->GetListOfTasks();

    FairRootManager* fRootManager=FairRootManager::Instance();

    for (int i = 1; i < 20; i++)
    {
        fRootManager->ReadEvent(i);

        int iter = 1;
        TObjLink *lnk = taskList->FirstLink();
        while (lnk)
        {
            FairTask* pCurTask = (FairTask*) lnk->GetObject();
            pCurTask->ExecuteTask("");
            cout<<"Complete task: "<<iter++<<endl;
            lnk = lnk->Next();
        }
        gEve->Redraw3D(kFALSE);
    }
}

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

  fEventReadData = new vector<EventData*>();
  fEventDrawData = new vector<EventData*>();
}

void FairEventManagerEditor::Init()
{
  // get input file
  FairRootManager* fRootManager=FairRootManager::Instance();
  TChain* chain = fRootManager->GetInChain();
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
  TFile* file = FairRootManager::Instance()->GetInChain()->GetFile();
  Infile += file->GetName();
  TGLabel* TFName=new TGLabel(title1, Infile.Data());
  title1->AddFrame(TFName);

  // display Run ID
  UInt_t RunId = FairRunAna::Instance()->getRunId();
  TString run = "Run Id : ";
  run += RunId;
  TGLabel* TRunId=new TGLabel(title1, run.Data());
  title1->AddFrame(TRunId);

  // display event count
  TString nevent= "No of events : ";
  nevent += Entries;
  TGLabel* TEvent = new TGLabel(title1, nevent.Data());
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

  // button for high transparency to highlight event objects
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
  fShowMCPoints->SetDisabledAndSelected(kFALSE);

  // button for show|hide reconstructed points
  fShowRecoPoints = new TGCheckButton(framePointsInfo, "Reco points");
  framePointsInfo->AddFrame(fShowRecoPoints, new TGLayoutHints(kLHintsRight, 0,0,1,0));
  fShowRecoPoints->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowRecoPoints(Bool_t)");
  fShowRecoPoints->SetDisabledAndSelected(kFALSE);
  groupData->AddFrame(framePointsInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));

  TGHorizontalFrame* frameTracksInfo = new TGHorizontalFrame(groupData);
  // button for show|hide MC tracks
  fShowMCTracks = new TGCheckButton(frameTracksInfo, "MC tracks");
  frameTracksInfo->AddFrame(fShowMCTracks, new TGLayoutHints(kLHintsNormal, 0,0,0,0));
  fShowMCTracks->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowMCTracks(Bool_t)");
  fShowMCTracks->SetDisabledAndSelected(kFALSE);

  // button for show|hide reco tracks
  fShowRecoTracks = new TGCheckButton(frameTracksInfo, "Reco tracks");
  frameTracksInfo->AddFrame(fShowRecoTracks, new TGLayoutHints(kLHintsRight, 0,0,1,0));
  fShowRecoTracks->Connect("Toggled(Bool_t)", "FairEventManagerEditor", this, "ShowRecoTracks(Bool_t)");
  fShowRecoTracks->SetDisabledAndSelected(kFALSE);
  groupData->AddFrame(frameTracksInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));

  title1->AddFrame(groupData, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 3,15,1,1));

  // button for update of event visualization
  fUpdate = new TGTextButton(title1, "Update");
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
    if ((fManager->isOnline) && ((fManager->fDataSource == 1) || (iCurrentEvent != -1)))
    {
        if (fManager->fDataSource == 1)
        {
            semEventData = new TSemaphore(0);

            // run thread for data files parsing
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
        }
        else
        {
            // run thread for online presentation of simulation data
            ThreadParam_RunTask* par_run_task = new ThreadParam_RunTask();
            par_run_task->fEventManager = fManager;

            TThread* thread_run_task = new TThread(RunTasks, (void*)par_run_task);
            thread_run_task->Run();
        }
    }
    // if offline
    else
    {
        int iNewEvent = fCurrentEvent->GetIntNumber();
        // exec event visualization of selected event
        fManager->GotoEvent(iNewEvent);

        // first time checking for active buttons
        if (iCurrentEvent == -1)
        {
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

            //fUpdate->SetEnabled(kFALSE);
        }

        if (iCurrentEvent != iNewEvent)
        {
            iCurrentEvent = iNewEvent;

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
    }

    // update tab controls
    Update();

    if (!fManager->isOnline)
    {
    // update all scenes
    fManager->fRPhiView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fRhoZView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fMulti3DView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fMultiRPhiView->GetGLViewer()->UpdateScene(kTRUE);
    fManager->fMultiRhoZView->GetGLViewer()->UpdateScene(kTRUE);
    }
}

//______________________________________________________________________________
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

    // highlight ZDC modules
    if (fManager->isZDCModule)
    {
        if (is_show == true)
        {
            TGeoVolume* curVolume = gGeoManager->GetVolume("VETO");
            if (!curVolume)
            {
                cout<<"ERROR: There is no volume with given name: VETO"<<endl;
            }
            //for (int i = 0; i < curVolume->GetNdaughters(); i++)
            for (int i = 0; i < 104; i++)
            {
                if (fManager->isZDCModule[i] == false)
                {
                    TString vol_name = "";
                    if (i < 68)
                        vol_name.Form("VMDL_%d", i+1);
                    else
                        vol_name.Form("UMDL_%d", i+1-68);
                    TGeoNode* child = curVolume->FindNode(vol_name);
                    if (child == NULL)
                        continue;

                    //cout<<"Node: "<<child->GetName()<<". Number is equal "<<i<<endl;
                    child->SetVisibility(false);
                    child->VisibleDaughters(false);
                }
            }
        }
        else
        {
            TGeoVolume* curVolume = gGeoManager->GetVolume("VETO");
            if (!curVolume)
            {
                cout<<"ERROR: There is no volume with given name: VETO"<<endl;
            }
            //for (int i = 0; i < curVolume->GetNdaughters(); i++)
            for (int i = 0; i < 104; i++)
            {
                if (fManager->isZDCModule[i] == false)
                {
                    TString vol_name = "";
                    if (i < 68)
                        vol_name.Form("VMDL_%d", i+1);
                    else
                        vol_name.Form("UMDL_%d", i+1-68);
                    TGeoNode* child = curVolume->FindNode(vol_name);
                    if (child == NULL)
                        continue;

                    //cout<<"Node: "<<child->GetName()<<". Number is equal "<<i<<endl;
                    child->SetVisibility(true);
                    child->VisibleDaughters(true);
                }
            }
        }
    }

    gEve->GetGlobalScene()->SetRnrState(kFALSE);
    gEve->GetGlobalScene()->SetRnrState(kTRUE);
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
