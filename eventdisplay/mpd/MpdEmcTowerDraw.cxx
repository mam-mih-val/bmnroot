// -------------------------------------------------------------------------
// -----                        EmcTowerDraw source file               -----
// -------------------------------------------------------------------------

//#define DEBUG_EMC_TOWERS
#include "MpdEmcTowerDraw.h"
#include "MpdEmc.h"
#include "MpdEmcDigit.h"

#include "MpdEventManagerEditor.h"
#include "FairRunSim.h"

#include "TEveManager.h"    // for gEve
#include "TGeoManager.h"    // for gGeoManager
#include "TEveTreeTools.h"  // for TEvePointSelectorConsumer
#include "TGeoArb8.h"       // for TGeoTrap
#include <TMath.h>
#include <TROOT.h>          // for gROOT
#include <TStopwatch.h>

//#include <assert.h>
#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
MpdEmcTowerDraw::MpdEmcTowerDraw()
  : FairTask("MpdEmcTowerDraw", 0),
    fEmcMinEnergyThreshold(0),
    fVerbose(0),
    fDigitList(NULL),
    fEventManager(NULL),
    fResetRequiredFlag(kFALSE),
    fRMinEmc(0),
    fRMaxEmc(0),
    fBoxHeight(0),
    fSector1StartAngle(0),
    fNBoxes(0),
    fNTubes(0),
    fNSectors(0),
    fN3Dbins(0),
    fEneArr(NULL),
    fGeoPar(new MpdEmcGeoPar),
    fq(NULL)
{
}

// -----   Standard constructor   ------------------------------------------
MpdEmcTowerDraw::MpdEmcTowerDraw(const char* name, Double_t emcMinEnergyThreshold, Int_t verbose)
  : FairTask(name, verbose),
    fEmcMinEnergyThreshold(emcMinEnergyThreshold),
    fVerbose(verbose),
    fDigitList(NULL),
    fEventManager(NULL),
    fResetRequiredFlag(kFALSE),
    fRMinEmc(0),
    fRMaxEmc(0),
    fBoxHeight(0),
    fSector1StartAngle(0),
    fNBoxes(0),
    fNTubes(0),
    fNSectors(0),
    fN3Dbins(0),
    fEneArr(NULL),
    fGeoPar(new MpdEmcGeoPar),
    fq(NULL)
{
}

// -------------------------------------------------------------------------
InitStatus MpdEmcTowerDraw::Init()
{
    if (fVerbose > 0) cout<<"MpdEmcTowerDraw::Init()"<<endl;

    fEventManager = MpdEventManager::Instance();
    fEventManager->fgRedrawRecoPointsReqired = kTRUE;
    if (fVerbose > 1) cout<<"MpdEmcTowerDraw::Init() get instance of EventManager: "<<fEventManager<<endl;

    FairRootManager* fManager = FairRootManager::Instance();
    if (fVerbose > 1) cout<<"MpdEmcTowerDraw::Init() get instance of FairRootManager: "<<fManager<<endl;

    fDigitList = (TClonesArray*) fManager->GetObject("EmcDigit");
    if (fDigitList == 0)
    {
        LOG(error)<<"MpdEmcTowerDraw::Init() branch EmcDigit not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }
     
    SetRMinEmc(GetEmcGeoPar()->GetRmin() * 0.1);
    SetRMaxEmc(GetEmcGeoPar()->GetRmax() * 0.1);    
    SetNBoxes(GetEmcGeoPar()->GetNModInSuperModByZ() * GetEmcGeoPar()->GetNModInSuperModByPhi());
    SetNTubes(GetEmcGeoPar()->GetNsupMod() * GetEmcGeoPar()->GetNrows());
    SetNSectors(GetEmcGeoPar()->GetNsec());
    SetNRowsByPhi(GetEmcGeoPar()->GetNsupMod()); //4
    SetNRowsByZ(GetEmcGeoPar()->GetNrows()); //23
    SetLenSuperModule(GetEmcGeoPar()->GetLengthOfSuperModuleByZ()); //12(cm)
    SetLenSector(GetEmcGeoPar()->GetLength()); //276(cm)
    SetNRowInSuperModByPhi(GetEmcGeoPar()->GetNModInSuperModByPhi()); //3
    SetNRowInSuperModByZ(GetEmcGeoPar()->GetNModInSuperModByZ()); //3
    SetNMiddleBoxesInSuperMod((GetNRowInSuperModByPhi()-2) * GetNRowInSuperModByZ()); //3

    //values of PHI1 and PHI2 are taken from emc_tr_400_3.geo:
    SetSector1StartAngle(83.5714285714*TMath::DegToRad());
    SetSector1EndAngle(96.4285714286*TMath::DegToRad());
    SetBoxHeight(FindBoxesHeights());

    SetN3Dbins(2*GetNSectors()*GetNTubes()*GetNBoxes()); //2*28*4*23*3*3
    if (fVerbose > 0) cout<<"MpdEmcTowerDraw::Init() number of 3d bins = "<<GetN3Dbins()<<endl;

    fEneArr = new Double_t[GetN3Dbins()];
    for (Int_t i = 0; i < GetN3Dbins(); i++)
        SetEneArr(i,0);
    
    fq = 0;
    
    return kSUCCESS;
}

void MpdEmcTowerDraw::Exec(Option_t* option)
{   
    if (!IsActive())
        return;

    Reset();
    if (fVerbose > 0) cout<<"MpdEmcTowerDraw::Exec() current visibility level = "<<gGeoManager->GetVisLevel()<<endl;

    if (fEventManager->fgShowRecoPointsIsShow)
    {
        if (gGeoManager->GetVisLevel() == 6)
        {
            //only if the boxes are visible
            FillEnergyLossArray();

            //cut off energies under threshold
            if (GetEmcMinEnergyThreshold() != 0)
                for (UInt_t i = 0; i < GetN3Dbins(); i++)
                    if (GetEneArrValue(i) < GetEmcMinEnergyThreshold()) SetEneArr(i, 0);

            //search for maximum bin
            for (UInt_t i = 0; i < GetN3Dbins(); i++)
            {
                Double_t E = GetEneArrValue(i);
                if (E > GetMaxE()) SetMaxE(E);
            }

            if (fVerbose > 0) cout<<"MpdEmcTowerDraw::Exec() maxE = "<<GetMaxE()<<endl;

            DrawBoxTowers();
            gGeoManager->cd("/cave_1/emc1EmptyChamber1_1");
            TGeoNode *chamberNode = gGeoManager->GetCurrentNode();
            chamberNode->GetVolume()->SetTransparency(60);
            //gGeoManager->cd("/cave_1/emc1EmptyChamber2_1");
            //chamberNode = gGeoManager->GetCurrentNode();
            //chamberNode->GetVolume()->SetTransparency(60);
            //FairDetector *Emc1= new MpdEmc("EMC1", kTRUE);
            //Emc1->SetGeometryFileName("emc_tr_400_3.geo");
            //((TEveElement*)Emc1)->SetRnrState(kFALSE);
            //FairRunSim::Instance()->AddModule(Emc1);
            //gEve->AddGlobalElement(gsre);

            //TEveElement* Emc1Elem = gsre->FindChild("EMC1");
            //elTRD->SetRnrState(kFALSE);
            SetResetRequiredFlag(kTRUE);
        }
    }
    else
    {
        if (GetResetRequiredFlag())
        {
            ResetBoxTowers();
            SetResetRequiredFlag(kFALSE);
        }
    }
    TEvePointSet* q = new TEvePointSet(GetName(), fDigitList->GetEntriesFast(), TEvePointSelectorConsumer::kTVT_XYZ);
    q->SetOwnIds(kTRUE);

    fEventManager->AddEventElement(q, RecoPointList);

    fq = q;
    gEve->FullRedraw3D();
}

void MpdEmcTowerDraw::FillEnergyLossArray()
{ 
    TStopwatch timer;
    timer.Start();

    //calculate energy loss for each bin
    UInt_t fNhits = fDigitList->GetEntriesFast();
    if (fVerbose > 0) cout<<"MpdEmcTowerDraw::FillEnergyLossArray() Number of EMC hits = "<<fNhits<<endl;

    for (UInt_t iPnt = 0; iPnt < fNhits; iPnt++)
    {
        MpdEmcDigit* dgt = (MpdEmcDigit*) fDigitList->At(iPnt);             
        TString binPath = FindBoxPathZPhi(dgt->GetZcenter(), dgt->GetPhiCenter());

#ifdef DEBUG_EMC_TOWERS  
        cout<<"w/o  geo:"<<binPath<<endl;            
        TString binPathGeo = FindBoxPathZPhiWithGeoManager((GetRMaxEmc() + GetRMinEmc())/2.0, dgt->GetZcenter(),dgt->GetPhiCenter());
        cout<<"with geo:"<<binPathGeo<<endl;
        cout<<endl;
        assert(binPath == binPathGeo);
#endif              
        UInt_t iCur = FindN3DbinBoxId(binPath);
            
        if ((iCur >= 0) && (iCur<GetN3Dbins()))
            SetEneArr(iCur, GetEneArrValue(iCur) + dgt->GetE());
        else
            cout<<"MpdEmcTowerDraw::FillEnergyLossArray() wrong path = "<<binPath<< "; at Zcenter = "<<dgt->GetZcenter()<<" and PhiCenter = "<<dgt->GetPhiCenter()<<endl;
    }  

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout<<endl<<endl<<"MpdEmcTowerDraw::FillEnergyLossArray() Real time "<<rtime<<" s, CPU time "<<ctime<<" s"<<endl;
}

void MpdEmcTowerDraw::DrawBoxTowers()
{    
    TStopwatch timer;
    timer.Start();

    gGeoManager->cd("/cave_1/emc1Chamber1_1"); 
    TGeoNode* chamberNode = gGeoManager->GetCurrentNode();
    TObjArray* chHArr= chamberNode->GetVolume()->GetNodes();

    TObjArray *boxArr, *moduleArr, *tubeArr, *sectorArr;
    for (UInt_t chH = 0; chH < 2; chH++)
    {
        TGeoNode* chHNode = (TGeoNode*) chHArr->UncheckedAt(chH);
        sectorArr = chHNode->GetVolume()->GetNodes();
        TGeoVolume* chHVolumeClone = chHNode->GetVolume()->CloneVolume();
        for (UInt_t sector = 0; sector < chHNode->GetVolume()->GetNdaughters(); sector++)
        {
            TGeoNode* sectorNode = (TGeoNode*) sectorArr->UncheckedAt(sector);
            tubeArr = sectorNode->GetVolume()->GetNodes();
            TGeoVolume* sectorVolumeClone = sectorNode->GetVolume()->CloneVolume();
            for (UInt_t tube = 0; tube < sectorNode->GetVolume()->GetNdaughters(); tube++)
            {
                TGeoNode* tubeNode = (TGeoNode*) tubeArr->UncheckedAt(tube);
                if (((TString)tubeNode->GetName()).Contains("St_"))
                {
                    tubeNode->SetVisibility(kFALSE);
                    continue; //ignore emc1TubeSt_i
                }

                moduleArr = tubeNode->GetVolume()->GetNodes();  
                TGeoVolume* tubeVolumeClone = tubeNode->GetVolume()->CloneVolume();
                for (UInt_t module = 0; module < tubeNode->GetVolume()->GetNdaughters(); module++)
                {
                    TGeoNode* moduleNode = (TGeoNode*) moduleArr->UncheckedAt(module);
                    if (((TString)moduleNode->GetName()).Contains("Pad_"))
                    {
                        moduleNode->SetVisibility(kFALSE);
                        continue; //ignore emc1ModulePad_i
                    }

                    boxArr = moduleNode->GetVolume()->GetNodes();
                    TGeoVolume* moduleVolumeClone = moduleNode->GetVolume()->CloneVolume();
                    for (Int_t box = 0; box < moduleNode->GetVolume()->GetNdaughters(); box++)
                    {
                        TGeoNode* boxNode = (TGeoNode*) boxArr->UncheckedAt(box);
                        TString curPath = Form("/cave_1/emc1Chamber1_1/%s/%s/%s/%s/%s",
                                               chHNode->GetName(), sectorNode->GetName(), tubeNode->GetName(), moduleNode->GetName(), boxNode->GetName());
                        if (!gGeoManager->CheckPath(curPath))
                        {
                            cout<<"MpdEmcTowerDraw: wrong path: "<<curPath;
                            continue;
                        }

                        UInt_t id = FindN3DbinBoxId(curPath);
                        TGeoTrap* trap = (TGeoTrap*) boxNode->GetVolume()->GetShape()->Clone();
                        Double_t (&fXY)[8][2] = *reinterpret_cast<Double_t (*)[8][2]>(trap->GetVertices());
                        if (GetEneArrValue(id) != 0)
                        {
                            //we move only the points below the y=0 plane (outer side)
                            Double_t h = -GetBoxHeight()/2*(2*GetEneArrValue(id)/GetMaxE()-1);
                            trap->SetVertex(0, fXY[0][0], h);
                            trap->SetVertex(3, fXY[3][0], h);
                            trap->SetVertex(4, fXY[4][0], h);
                            trap->SetVertex(7, fXY[7][0], h);
                            //boxNode->GetMatrix()->Print();
                            moduleVolumeClone->RemoveNode(boxNode);
                            TGeoVolume* boxVolumeCopy = boxNode->GetVolume()->MakeCopyVolume((TGeoShape*)trap);
                            moduleVolumeClone->AddNode(boxVolumeCopy, boxNode->GetNumber(), boxNode->GetMatrix());
                        }
                        else
                            boxNode->SetVisibility(kFALSE);
                    }
                    tubeVolumeClone->RemoveNode(moduleNode);
                    tubeVolumeClone->AddNode(moduleVolumeClone, moduleNode->GetNumber(),moduleNode->GetMatrix());
                }
                sectorVolumeClone->RemoveNode(tubeNode);
                sectorVolumeClone->AddNode(tubeVolumeClone, tubeNode->GetNumber(), tubeNode->GetMatrix());
            }
            chHVolumeClone->RemoveNode(sectorNode);
            chHVolumeClone->AddNode(sectorVolumeClone, sectorNode->GetNumber(), sectorNode->GetMatrix());
        }
        chamberNode->GetVolume()->AddNode(chHVolumeClone, chHNode->GetNumber(), chHNode->GetMatrix());
    }

    TGeoNode* chHNode = (TGeoNode*) chHArr->UncheckedAt(0);
    chamberNode->GetVolume()->RemoveNode(chHNode);
    chHNode = (TGeoNode*) chHArr->UncheckedAt(0);
    chamberNode->GetVolume()->RemoveNode(chHNode);

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout<<endl<<endl<<"MpdEmcTowerDraw::DrawSectorTowers() Real time "<<rtime<<" s, CPU time "<<ctime<<" s"<<endl;
    return;
}

void MpdEmcTowerDraw::ResetBoxTowers()
{
    TStopwatch timer;
    timer.Start();

    gGeoManager->cd("/cave_1/emc1Chamber1_1");
    TGeoNode* emc1ChamberNode = gGeoManager->GetCurrentNode();
    TObjArray *chHArr= emc1ChamberNode->GetVolume()->GetNodes();

    TObjArray *boxArr, *moduleArr, *tubeArr, *sectorArr;
    for (UInt_t chH = 0; chH < emc1ChamberNode->GetVolume()->GetNdaughters(); chH++)
    {
        TGeoNode* chHNode = (TGeoNode*) chHArr->UncheckedAt(chH);
        sectorArr = chHNode->GetVolume()->GetNodes();
        for (UInt_t sector = 0; sector < chHNode->GetVolume()->GetNdaughters(); sector++)
        {
            TGeoNode* sectorNode = (TGeoNode*) sectorArr->UncheckedAt(sector);
            tubeArr = sectorNode->GetVolume()->GetNodes();
            for (UInt_t tube = 0; tube < sectorNode->GetVolume()->GetNdaughters(); tube++)
            {
                TGeoNode* tubeNode = (TGeoNode*) tubeArr->UncheckedAt(tube);
                if (((TString)tubeNode->GetName()).Contains("St_"))
                {
                    tubeNode->SetVisibility(kTRUE);
                    continue; //ignore emc1TubeSt_i
                }

                moduleArr = tubeNode->GetVolume()->GetNodes();
                for (UInt_t module = 0; module < tubeNode->GetVolume()->GetNdaughters(); module++)
                {
                    TGeoNode* moduleNode = (TGeoNode*) moduleArr->UncheckedAt(module);
                    if (((TString)moduleNode->GetName()).Contains("Pad_"))
                    {
                        moduleNode->SetVisibility(kTRUE);
                        continue; //ignore emc1ModulePad_i
                    }

                    boxArr = moduleNode->GetVolume()->GetNodes();
                    for (Int_t box = 0; box < moduleNode->GetVolume()->GetNdaughters(); box++)
                    {
                        TGeoNode* boxNode = (TGeoNode*) boxArr->UncheckedAt(box);
                        TGeoTrap* trap = (TGeoTrap*) boxNode->GetVolume()->GetShape()->Clone();
                        Double_t (&fXY)[8][2] = *reinterpret_cast<Double_t (*)[8][2]>(trap->GetVertices());
                        //we move only the points below the y=0 plane (outer side)
                        Double_t h = -GetBoxHeight()/2;
                        trap->SetVertex(0, fXY[0][0], h);
                        trap->SetVertex(3, fXY[3][0], h);
                        trap->SetVertex(4, fXY[4][0], h);
                        trap->SetVertex(7, fXY[7][0], h);
                        //boxNode->GetMatrix()->Print();
                        boxNode->GetVolume()->SetShape((TGeoShape*)trap);
                        boxNode->SetVisibility(kTRUE);
                    }
                }
            }
        }
    }

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout<<endl<<endl<<"MpdEmcTowerDraw::ResetBoxTowers() Real time "<<rtime<<" s, CPU time "<<ctime<<" s"<<endl;
    return;
}

Double_t MpdEmcTowerDraw::FindBoxesHeights()
{
    //get the initial height of the box to be able to reset later
    gGeoManager->cd("/cave_1/emc1Chamber1_1/emc1ChH_1/emc1Sector_1/emc1Tube_1/emc1Module_1/emc1_box_1");
    TGeoTrap* trap = (TGeoTrap*) gGeoManager->GetCurrentNode()->GetVolume()->GetShape();

    Double_t (&fXY)[8][2] = *reinterpret_cast<Double_t (*)[8][2]>(trap->GetVertices());
    //cout<<"Box Height = "<<-fXY[0][1]*2.0<<endl;

    return -fXY[0][1]*2.0;
}

UInt_t MpdEmcTowerDraw::FindN3DbinBoxId(TString path)
{
    Int_t ip = path.Index("ChH");
    UInt_t chHId = TString(path(ip+4,1)).Atoi();
    ip = path.Index("Sector");
    Int_t ip1 = path.Index("/", ip);
    UInt_t sectorId = TString(path(ip+7,ip1)).Atoi();
    ip = path.Index("Tube");
    ip1 = path.Index("/", ip);
    UInt_t tubeId = TString(path(ip+5,ip1)).Atoi();
    ip = path.Index("bt_box");

    //for bt_ boxes (side boxes) we shift the id by the number of middle boxes (3)
    UInt_t boxId;
    if (ip != -1) boxId= TString(path(ip+7)).Atoi()+GetNMiddleBoxesInSuperMod();
    else
    {
        ip = path.Index("box");
        boxId= TString(path(ip+4)).Atoi();
    }

    return (GetNBoxes() * GetNTubes() * GetNSectors() * (chHId-1)) + (GetNBoxes() * GetNTubes() * (sectorId-1)) + (GetNBoxes() * (tubeId-1)) + (boxId-1);
}

TString MpdEmcTowerDraw::FindPathN3DbinBoxId(UInt_t id)
{
    UInt_t chHId = (id / (GetNBoxes() * GetNTubes() * GetNSectors()))+1;
    id -= ((chHId-1) * GetNBoxes() * GetNTubes() * GetNSectors());
    UInt_t sectorId  = id / (GetNBoxes() * GetNTubes())+1;
    id -= ((sectorId-1) * GetNBoxes() * GetNTubes());
    UInt_t tubeId = id / GetNBoxes() +1;
    UInt_t boxId = id % GetNBoxes() +1;

    //for bt_ boxes (side boxes) we shift the id by the number of middle boxes (3)
    if (boxId>GetNMiddleBoxesInSuperMod())
        return Form("/cave_1/emc1Chamber1_1/emc1ChH_%d/emc1Sector_%d/emc1Tube_%d/emc1Module_1/emc1_bt_box_%d", chHId, sectorId, tubeId, boxId - GetNMiddleBoxesInSuperMod());
    else
        return Form("/cave_1/emc1Chamber1_1/emc1ChH_%d/emc1Sector_%d/emc1Tube_%d/emc1Module_1/emc1_box_%d", chHId, sectorId, tubeId, boxId);
}

TString MpdEmcTowerDraw::FindBoxPathZPhiWithGeoManager(Double_t r, Double_t z, Double_t phi)
{
    Double_t x = r*TMath::Cos(phi);
    Double_t y = r*TMath::Sin(phi);
    gGeoManager->FindNode(x,y,z); 

    TString path(gGeoManager->GetPath());
    if (gGeoManager->cd(path))
    {
        gGeoManager->CdUp();
        return gGeoManager->GetPath();
    }

    return "";
}

TString MpdEmcTowerDraw::FindBoxPathZPhi(Double_t z, Double_t phi)
{
    UInt_t chHId = (z > 0)? 2 : 1;//current ChH id
    Double_t phiAbs; // current angle from starting angle PHI1 of the first sector (emc1Sector_1), radians 0 
    // (clockwise in emc1ChH_1, counterclockwise in emc1ChH_2)
    if (chHId == 1)
        phiAbs = phi>GetSector1StartAngle() ? phi-GetSector1StartAngle() : (TMath::TwoPi()+(phi-GetSector1StartAngle()));
    else        
        phiAbs = phi>GetSector1EndAngle() ? (TMath::TwoPi()-(phi-GetSector1EndAngle())) : (GetSector1EndAngle()-phi);
    
    UInt_t sectorId = UInt_t (phiAbs / TMath::TwoPi() * GetNSectors()) + 1;
    //finding tube id
    //phiT - current angle between PHI1 of the beginning of sector and Phi angle
    Double_t phiT = phiAbs - TMath::TwoPi()*(sectorId-1)/GetNSectors();
    ///  current id of the row by phi in sector
    Double_t rowIdByPhi = UInt_t(GetNRowsByPhi()*GetNSectors() * phiT/(TMath::TwoPi()))+1;
    UInt_t tubeId;
    if (chHId == 1)
        tubeId = UInt_t((GetLenSector()-TMath::Abs(z))/GetLenSuperModule())+(rowIdByPhi-1)*GetNRowsByZ()+1;
    else
        tubeId = UInt_t(TMath::Abs(z)/GetLenSuperModule())+(rowIdByPhi-1)*GetNRowsByZ()+1;
    
    //finding box id
    UInt_t boxId;
    Double_t phiB = phiT - TMath::TwoPi()*(rowIdByPhi-1)/(GetNSectors()*GetNRowsByPhi());
    UInt_t supModRowIdByPhi = UInt_t(GetNRowInSuperModByPhi()*GetNSectors()*GetNRowsByPhi()*phiB/(TMath::TwoPi()))+1;
    UInt_t supModRowIdByZ;
    Double_t tubeZ = (tubeId-UInt_t((tubeId-1)/GetNRowsByZ())*GetNRowsByZ()-1)*GetLenSuperModule();
    if (chHId==1)
        supModRowIdByZ = UInt_t((GetLenSector()-TMath::Abs(z)-tubeZ)/GetLenSuperModule()*GetNRowInSuperModByZ())+1;
    else
        supModRowIdByZ = UInt_t((TMath::Abs(z) - tubeZ)/GetLenSuperModule()*GetNRowInSuperModByZ())+1;
    
    if (supModRowIdByPhi == 1)//bt_ box 1-3
        boxId=supModRowIdByZ+GetNMiddleBoxesInSuperMod();
    else 
        if(supModRowIdByPhi==GetNRowInSuperModByPhi())//bt_box 4-6
            boxId=supModRowIdByZ+GetNRowInSuperModByZ()+GetNMiddleBoxesInSuperMod();
        else//middle boxes 1-3
            boxId=UInt_t(GetNMiddleBoxesInSuperMod()/GetNRowInSuperModByZ())*(supModRowIdByPhi-2)+supModRowIdByZ;

    //for bt_ boxes (side boxes) we shift the id by the number of middle boxes (3)
    if (boxId>GetNMiddleBoxesInSuperMod())
        return Form("/cave_1/emc1Chamber1_1/emc1ChH_%d/emc1Sector_%d/emc1Tube_%d/emc1Module_1/emc1_bt_box_%d",chHId,sectorId,tubeId,boxId - GetNMiddleBoxesInSuperMod());
    else
        return Form("/cave_1/emc1Chamber1_1/emc1ChH_%d/emc1Sector_%d/emc1Tube_%d/emc1Module_1/emc1_box_%d",chHId,sectorId,tubeId,boxId);
}

// -----   Destructor   ----------------------------------------------------
MpdEmcTowerDraw::~MpdEmcTowerDraw()
{
    fDigitList->Delete();
    delete fGeoPar;
    delete fEneArr;
}

// -------------------------------------------------------------------------

/** Action after each event**/
void MpdEmcTowerDraw::Finish()
{
}

// -------------------------------------------------------------------------
void MpdEmcTowerDraw::Reset()
{     
    if (fq != 0)
    {           
        fq->Reset();
        gEve->RemoveElement(fq, fEventManager->EveRecoPoints);       
        gROOT->Reset();
    }    
}

ClassImp(MpdEmcTowerDraw);
