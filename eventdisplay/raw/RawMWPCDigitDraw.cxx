// -------------------------------------------------------------------------
// -----                        RawMWPCDigitDraw source file           -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include "RawMWPCDigitDraw.h"

#include "TEveManager.h"
#include "TEveTreeTools.h"
#include "TString.h"

#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
RawMWPCDigitDraw::RawMWPCDigitDraw()
  : FairTask("RawMWPCDigitDraw", 0),
    fVerbose(0),
    fEventManager(NULL),
    fq(NULL),
    fColor(0),
    fStyle(0)
{
    pEventData = new vector<EventData*>();
}

// -----   Standard constructor   ------------------------------------------
RawMWPCDigitDraw::RawMWPCDigitDraw(const char* name, Color_t color ,Style_t mstyle,Int_t iVerbose)
  : FairTask(name, iVerbose),
    fVerbose(iVerbose),
    fEventManager(NULL),
    fq(NULL),
    fColor(color),
    fStyle(mstyle)
{
    pEventData = new vector<EventData*>();
}

// -------------------------------------------------------------------------
InitStatus RawMWPCDigitDraw::Init()
{
    if (fVerbose > 1)
        cout<<"RawMWPCDigitDraw::Init()"<<endl;

    // read source files and generate vector of EventData objects
    RawDataParser raw_parser;

    TString* mwpc_names = new TString[12];
    raw_parser.GenerateMWPCFileNames(source_file_name, &raw_parser.device_serial1, mwpc_names);
    raw_parser.GenerateMWPCFileNames(source_file_name, &raw_parser.device_serial2, &mwpc_names[6]);

    raw_parser.ParseHRBFiles(pEventData, mwpc_names);

    delete[] mwpc_names;

    fEventManager = FairEventManager::Instance();
    if (fVerbose > 2)
        cout<<"RawMWPCDigitDraw::Init() get instance of FairEventManager "<<endl;

    //cout<<endl<<"fEntryCount "<<fEventManager->fEntryCount<<" Event Count "<<pEventData->size()<<endl;
    if (fEventManager->fEntryCount == 0)
        fEventManager->fEntryCount = pEventData->size();
    else
        fEventManager->fEntryCount = TMath::Min(fEventManager->fEntryCount, (Long64_t)pEventData->size());

    return kSUCCESS;
}

void RawMWPCDigitDraw::Exec(Option_t* option)
{
    if (IsActive())
    {
        Reset();

        Int_t event_number = fEventManager->GetCurrentEvent();

        // convert EventData object to vector of coordinates
        if (event_number >= pEventData->size())
        {
            cout<<"Programming Error: event_number >= EventData vector size : "<<event_number<<" >= "<<pEventData->size()<<endl;
            return;
        }

        EventData* curEvent = (*pEventData)[event_number];
        cout<<"Event processing: "<<event_number<<endl;

        RawDataConverter raw_converter;
        vector<TVector3*> event_hits = raw_converter.MWPCEventToGeoVector(curEvent);

        Int_t npoints = event_hits.size();
        cout<<"Point vector size: "<<npoints<<endl;

        TEvePointSet* q = new TEvePointSet(GetName(), npoints, TEvePointSelectorConsumer::kTVT_XYZ);
        q->SetOwnIds(kTRUE);
        q->SetMarkerColor(fColor);
        q->SetMarkerSize(1.5);
        q->SetMarkerStyle(fStyle);

        for (Int_t i = 0; i < npoints; i++)
        {
            TVector3* vec = event_hits.at(i);
            if (vec != 0)
            {
                q->SetNextPoint(vec->X(),vec->Y(), vec->Z());
                q->SetPointId(new TNamed(Form("Point %d", i),""));
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
    }//if IsActive()
}

TObject* RawMWPCDigitDraw::GetValue(TObject* obj,Int_t i)
{
    return new TNamed(Form("Point %d", i),"");
}

// -----   Destructor   ----------------------------------------------------
RawMWPCDigitDraw::~RawMWPCDigitDraw()
{
    delete pEventData;
}

// -------------------------------------------------------------------------
void RawMWPCDigitDraw::SetParContainers()
{
}

// -------------------------------------------------------------------------
/** Action after each event**/
void RawMWPCDigitDraw::Finish()
{
}

// -------------------------------------------------------------------------
void RawMWPCDigitDraw::Reset()
{
    if (fq != 0)
    {
        fq->Reset();

        gEve->RemoveElement(fq, fEventManager->EveRecoPoints);
    }
}

ClassImp(RawMWPCDigitDraw);
