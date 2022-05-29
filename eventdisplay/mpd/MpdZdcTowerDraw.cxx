// -------------------------------------------------------------------------
// -----                        ZdcTowerDraw source file               -----
// -------------------------------------------------------------------------

//#define DEBUG_ZDC_TOWERS
#include "MpdZdcTowerDraw.h"
#include "MpdZdcDigi.h"

#include "MpdEventManagerEditor.h"
#include "FairLogger.h"

#include "TEveManager.h"    // for gEve
#include "TEveTreeTools.h"  // for TEvePointSelectorConsumer
#include "TGeoManager.h"    // for gGeoManager
#include "TEveCaloData.h"
#include "TEveCalo.h"
#include "TEveViewer.h"
#include "TGeoBBox.h"
#include "TH2F.h"
#include "TRandom.h"

#include <iostream>
using namespace std;

#define ZDC_PARENT "/cave_1/ZDC_common_0"

// -----   Default constructor   -------------------------------------------
MpdZdcTowerDraw::MpdZdcTowerDraw()
  : FairTask("MpdZdcTowerDraw", 0),
    fVerbose(0),
    fShadow(kFALSE),
    fResetRequiredFlag(kFALSE),
    fDigitList(NULL),
    fEventManager(NULL),
    fEneArr(NULL),
    fZdcMinEnergyThreshold(0),
    fq(NULL)
{
}

// -----   Standard constructor   ------------------------------------------
MpdZdcTowerDraw::MpdZdcTowerDraw(const char* name, Double_t zdcMinEnergyThreshold, Bool_t shadow, Int_t verbose)
  : FairTask(name, verbose),
    fVerbose(verbose),
    fShadow(shadow),
    fResetRequiredFlag(kFALSE),
    fDigitList(NULL),
    fEventManager(NULL),
    fZdcMinEnergyThreshold(zdcMinEnergyThreshold),
    fq(NULL)
{
}

// -------------------------------------------------------------------------
InitStatus MpdZdcTowerDraw::Init()
{
    if (fVerbose > 0) cout<<"MpdZdcTowerDraw::Init()"<<endl;

    fEventManager = MpdEventManager::Instance();
    if (fVerbose > 1) cout<<"MpdZdcTowerDraw::Init() get instance of EventManager: "<<fEventManager<<endl;

    fEventManager->fgRedrawRecoPointsReqired = kTRUE;

    FairRootManager* fManager = FairRootManager::Instance();
    if (fVerbose > 1) cout<<"MpdZdcTowerDraw::Init() get instance of FairRootManager: "<<fManager<<endl;

    fDigitList = (TClonesArray*) fManager->GetObject("ZdcDigit");
    if (fDigitList == 0)
    {
        LOG(error)<<"MpdZdcTowerDraw::Init() branch ZdcDigit not found! Task will be deactivated";
        SetActive(kFALSE);
    }
    
    SetNumModules(96);
    SetModuleZLen(40);
    
    fEneArr = new Double_t[GetNumModules()*2];
    for (Int_t i = 0; i < GetNumModules()*2; i++)
        SetEneArr(i,0);
    
    fq = 0;
    
    return kSUCCESS;
}

void MpdZdcTowerDraw::Exec(Option_t* option)
{   
    if (IsActive())
    {   
        Reset();
        if (fVerbose > 0) cout<<"MpdZdcTowerDraw::Exec() current visibility level = "<<gGeoManager->GetVisLevel()<<endl;

        if (fEventManager->fgShowRecoPointsIsShow)
        {
            UInt_t fNhits = fDigitList->GetEntriesFast();
            if (fVerbose > 0) cout<<"MpdZdcTowerDraw::Exec() Number of ZDC hits = " << fNhits << endl;

            for (Int_t i = 0; i < GetNumModules()*2; i++)
                SetEneArr(i,0);

            for (UInt_t iPnt = 0; iPnt < fNhits; iPnt++)
            {
                MpdZdcDigi* dgt = (MpdZdcDigi*) fDigitList->At(iPnt);
                //cout<<"GetELoss() = "<<dgt->GetELoss()<<endl;

                //cout<<"GetDetectorID() = "<<dgt->GetDetectorID()<<endl;
                //cout<<"GetModuleID() = "<<dgt->GetModuleID()<<endl;                
                UInt_t iCur = GetNumModules() * (dgt->GetDetectorID()-1) + dgt->GetModuleID()-1;
                //cout<<"iCur = " <<iCur<<endl;
                SetEneArr(iCur, GetEneArrValue(iCur) + dgt->GetELoss());
            }
            
            //cut off energies under threshold
            //if (GetZdcMinEnergyThreshold()!=0)
            //    for (UInt_t i = 0; i < GetNumModules()*2; ++i) {           
            //        if (GetEneArrValue(i) < GetZdcMinEnergyThreshold()) SetEneArr(i,0);            
            //}
            
            //search for maximum bin
            SetMaxE(0);
            for (UInt_t i = 0; i < 2*GetNumModules(); i++)
            {
                Double_t E = GetEneArrValue(i);
                //cout<<"E = "<<E<<endl;
                if (E > GetMaxE()) SetMaxE(E);
            }
            if (fVerbose > 0) cout<<"MpdZdcTowerDraw::Exec() maxE = "<<GetMaxE()<<endl;
                        
            DrawTowers();
            
            //gGeoManager->cd("/cave_1/zdc01Empty_1"); 
            //TGeoNode *zdc01 = gGeoManager->GetCurrentNode();
            //zdc01->GetVolume()->SetTransparency(60);
            //gGeoManager->cd("/cave_1/zdc01Empty_2"); 
            //zdc01 = gGeoManager->GetCurrentNode();
            //zdc01->GetVolume()->SetTransparency(60);
            
            //_test
            /*TEveCalo3D* calo3d = new TEveCalo3D();
            //calo3d->SetBarrelRadius(129.00);
            calo3d->SetBarrelRadius(50.0);
            //calo3d->SetEndCapPos(268.36);
            Double_t zdcZPos = 410.5;
            Double_t zdcZLen = 40.0;
            
            calo3d->SetEndCapPos(zdcZPos+zdcZLen);
            //calo3d->SetEta(-100,100);
            cout<<"calo3d->GetEndCapPos() = "<<calo3d->GetEndCapPos()<<endl;
            cout<<"calo3d->GetBackwardEndCapPos() = "<<calo3d->GetBackwardEndCapPos()<<endl;
            cout<<"calo3d->GetEta() = "<<calo3d->GetEta()<<endl;
            cout<<"calo3d->GetEtaMax() = "<<calo3d->GetEtaMax()<<endl;//6.28319
            cout<<"calo3d->GetEtaMin() = "<<calo3d->GetEtaMin()<<endl;//-6.28319
            cout<<"calo3d->GetEtaRng() = "<<calo3d->GetEtaRng()<<endl;
            cout<<"calo3d->GetPhi() = "<<calo3d->GetPhi()<<endl;
            cout<<"calo3d->GetPhiMax() = "<<calo3d->GetPhiMax()<<endl;
            cout<<"calo3d->GetPhiMin() = "<<calo3d->GetPhiMin()<<endl;
            cout<<"calo3d->GetPhiRng() = "<<calo3d->GetPhiRng()<<endl;
            cout<<"calo3d->GetForwardEndCapPos() = "<<calo3d->GetForwardEndCapPos()<<endl;
            cout<<"calo3d->GetTransitionEta() = "<<calo3d->GetTransitionEta()<<endl;
            cout<<"calo3d->GetTransitionEtaBackward() = "<<calo3d->GetTransitionEtaBackward()<<endl;
            cout<<"calo3d->GetTransitionEtaForward() = "<<calo3d->GetTransitionEtaForward()<<endl;
            cout<<"calo3d->GetTransitionTheta() = "<<calo3d->GetTransitionTheta()<<endl;
            cout<<"calo3d->GetTransitionThetaBackward() = "<<calo3d->GetTransitionThetaBackward()<<endl;
            cout<<"calo3d->GetTransitionThetaForward() = "<<calo3d->GetTransitionThetaForward()<<endl;           
            cout<<"calo3d->GetMaxValAbs() = "<<calo3d->GetMaxValAbs()<<endl;           
            cout<<"calo3d->GetMaxTowerH() = "<<calo3d->GetMaxTowerH()<<endl;
            
            //calo3d->SetFrameTransparency(100);
            
            // Create, fill and project a 2D histogram.
            TH2F *h2 = new TH2F("h2","",200,-10,10,200,-TwoPi(),TwoPi());
            Float_t px, py;
            for (Int_t i = 0; i < 160000; i++) {
                gRandom->Rannor(px,py);
                if (Abs(px)>calo3d->GetTransitionEta())
                    h2->Fill(px,py);
            }
            
            TEveCaloDataHist* data = new TEveCaloDataHist();
            data->AddHistogram(h2);//ecalHist);
            data->RefSliceInfo(0).Setup("ECAL", 0.3, kRed);
            //data->RefSliceInfo(1).Setup("HCAL", 0.1, kBlue);
            data->GetEtaBins()->SetTitleFont(120);
            data->GetEtaBins()->SetTitle("h");
            data->GetPhiBins()->SetTitleFont(120);
            data->GetPhiBins()->SetTitle("f");
            data->IncDenyDestroy();
            gEve->AddToListTree(data, kFALSE);
            
            TEveViewer* v= gEve->GetDefaultViewer();
            
            // Create a scene and a viewer in the given slot.
            TEveScene* s = gEve->SpawnNewScene("Scene ZDC");
            v->AddScene(s);
            calo3d->SetData(data);
            s->AddElement(calo3d);

            //gEve->GetBrowser()->GetTabRight()->SetTab(1);
            gEve->Redraw3D(kFALSE);*/
            //_____
        }
        else
        {
            if (GetResetRequiredFlag())
            {
                for (Int_t i = 0; i < GetNumModules()*2; i++)
                    SetEneArr(i,1);

                SetMaxE(1);
                DrawTowers();
                for (Int_t i = 0; i < GetNumModules()*2; i++)
                    SetEneArr(i,0);

                SetMaxE(0);
                SetResetRequiredFlag(kFALSE);
            }
        }

        TEvePointSet* q = new TEvePointSet(GetName(), fDigitList->GetEntriesFast(), TEvePointSelectorConsumer::kTVT_XYZ);
        q->SetOwnIds(kTRUE);
        
        fEventManager->AddEventElement(q, RecoPointList);

        fq = q;

        gEve->FullRedraw3D();
    }
}

void MpdZdcTowerDraw::DrawTowers()
{
    gGeoManager->cd("/cave_1");
    TGeoNode* caveNode = gGeoManager->GetCurrentNode();
    
    for (UInt_t zdcId = 0; zdcId < 2; zdcId++)
    {
        TString zdc_path = Form("%s/zdc01_%d", ZDC_PARENT, zdcId+1);
        if (gGeoManager->cd(zdc_path) == false)
        {
            cout<<"ERROR: MpdZdcTowerDraw::DrawTowers(): Path '"<<zdc_path<<"' not found"<<endl;
            return;
        }
        TGeoNode* zdcNode = gGeoManager->GetCurrentNode();
        TGeoVolume* zdcVolumeClone = zdcNode->GetVolume()->CloneVolume();
        TObjArray* zdcArr= zdcNode->GetVolume()->GetNodes();
        
        for (UInt_t module = 0; module < zdcNode->GetVolume()->GetNdaughters(); module++)
        {
            TGeoNode* moduleNode = (TGeoNode*) zdcArr->UncheckedAt(module);

            TGeoNode* moduleNodeCopy;
            if (GetShadowFlag())
            {
                RecursiveChangeNodeTransparent(moduleNode, 0);
                moduleNodeCopy =  moduleNode->MakeCopyNode();
            }
            
            UInt_t iCur = zdcId * GetNumModules() + moduleNode->GetNumber()-1;
            TGeoBBox* box = (TGeoBBox*) moduleNode->GetVolume()->GetShape()->Clone();
            //cout<<"GetEneArrValue(iCur) = "<< GetEneArrValue(iCur)<<"; box->GetName() = "<<box->GetName()<<"; moduleNode->GetMotherVolume()->GetName() = "<<moduleNode->GetMotherVolume()->GetName()<<"; moduleNode->GetName() = "<<moduleNode->GetName()<<"; moduleNode->GetNumber() = "<<moduleNode->GetNumber()<<"; iCur = "<<iCur<<endl;
                                        
            TGeoMatrix* mat =moduleNode->GetMatrix()->MakeClone();
            if (GetEneArrValue(iCur) != 0)
            {                 
                box->SetBoxDimensions(box->GetDX(), box->GetDY(), GetModuleZLen() * GetEneArrValue(iCur) / GetMaxE());
                ((TGeoTranslation*)mat)->SetDz(GetModuleZLen() * GetEneArrValue(iCur) / GetMaxE() - GetModuleZLen());
            }
            
            zdcVolumeClone->RemoveNode(moduleNode);
            TGeoVolume* moduleVolumeCopy = moduleNode->GetVolume()->MakeCopyVolume((TGeoShape*)box);
            moduleVolumeCopy->SetVisibility(kTRUE);
            if (GetEneArrValue(iCur) == 0)
                moduleVolumeCopy->SetVisibility(kFALSE);            
                
            //the internal structure of Module does note alow to change it's shape, so we delete it for the towers visualization                         
            TObjArray* arr = moduleVolumeCopy->GetNodes();
            UInt_t count = moduleVolumeCopy->GetNdaughters();
            for (UInt_t sa = 0; sa < count; sa++)
            {
                TGeoNode* node = (TGeoNode*) arr->UncheckedAt(0);
                moduleVolumeCopy->RemoveNode(node);
            }
            if (GetShadowFlag())
                RecursiveChangeNodeTransparent(moduleNodeCopy, 98);
            
            zdcVolumeClone->AddNode(moduleVolumeCopy, moduleNode->GetNumber(),mat);
            if(GetShadowFlag())
                zdcVolumeClone->AddNode(moduleNodeCopy->GetVolume(), moduleNode->GetNumber(),moduleNode->GetMatrix());
        }

        caveNode->GetVolume()->AddNode(zdcVolumeClone, zdcNode->GetNumber(), zdcNode->GetMatrix());
        caveNode->GetVolume()->RemoveNode(zdcNode);
        //zdcNode->GetVolume()->SetInvisible();//VisibleDaughters(kTRUE);
        //zdcNode->SetVisibility(kFALSE);
        //zdcNode->VisibleDaughters(kTRUE);
        //RecursiveChangeNodeTransparent(zdcNode, 80);
        //RecursiveChangeNodeTransparent(zdcNode, 0);
    }

    SetResetRequiredFlag(kTRUE);
}

void MpdZdcTowerDraw::RecursiveChangeNodeTransparent(TGeoNode* node, int transparency)
{
    for (int i = 0; i < node->GetNdaughters(); i++)
    {
        TGeoNode* child = node->GetDaughter(i);
        TGeoVolume* curVolume = child->GetVolume();

        curVolume->SetTransparency(transparency);

        if (child->GetNdaughters() != 0)
            RecursiveChangeNodeTransparent(child, transparency);
    }
}

// -----   Destructor   ----------------------------------------------------
MpdZdcTowerDraw::~MpdZdcTowerDraw()
{
    fDigitList->Delete();
}

// -------------------------------------------------------------------------

/** Action after each event**/
void MpdZdcTowerDraw::Finish()
{
}

// -------------------------------------------------------------------------
void MpdZdcTowerDraw::Reset()
{     
    if (fq != 0)
    {           
        fq->Reset();
        gEve->RemoveElement(fq, fEventManager->EveRecoPoints);                
    }    
}

ClassImp(MpdZdcTowerDraw);
